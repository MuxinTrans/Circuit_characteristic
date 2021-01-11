/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "define.h"
#include "ctrl.h"
#include "system.h"
#include "math.h"
#include "touch.h"

#define Res1 20	//R_in参考电阻--103=10*10^3--10*2=20
#define Res2 2.2//R_out参考电阻--222=22*10^2--2.2*1=2.2
#define Av 1	//仪表放大器放大值--6
#define Aver_time 1 //多次测量取平均值
#define FS 4096	//ADS1118阈值，实际值为4.096V

int end_flag, draw_end, storge_end;
int KW_word, FRE_word;	//KW_word:扫频信号频率控制字；FRE_word:采样时钟频率控制字
int Times, rdaddress = 0;
double fre_control;
double U_A, U_A_total = 0, U_B, U_B_total = 0, U_1, U_1_total = 0, U_2, U_2_total = 0;
double U_inf_total = 0, U_0_total = 0, U_r_total = 0;
double R_in, R_out;
double Gain;
int Round_U1 = 0,Round_U2;

/*
 * U_A输入参考电阻短接所得的输出---in
 * U_B输入参考电阻接入所得的输出---in
 * U_1输出参考电阻断路所得的输出---out
 * U_2输出参考电阻接入所得的输出---out
 * U_inf = U_1;U_0 = U_A;U_r = U_B;
 */

int rda_end = 129;
double max_of_Vpp = -100;
//double fre_cut, fre_range;
int fre_cut;
float Vpp_pf[600], fre_trans_pf[600], fre_pf[600];

double getVpp(double vrd);
int Sweep(int Rang_down,int Rang_up);
void Paint_V_f(float fre_sweep,int Range_down,int Range_up);
void dis_init();
void io_init();
void flag_init();
double getVdc(u16 AD_value);
void getRin();
void getKw(double fre);
void test();


int main() {
	if (deviceInit() < 0) {
		printf("device initial failed\n");
		return -1;
	}
	lcdRectClear(0, 0, 799, 479, WHITE);
	dis_init();
	io_init();
	flag_init();
	Range_down_data = 100.0;
	Range_up_data = 1000000.0;//1M

	printf("Finish Initial!\n");

	while (1)
	{


		if((!Troubleshooting) && (!draw_flag)){
			fre_control = 1000;
//			IOWR(SWITCH_4WIRE_BASE,0,0);//继电器0000
//			usleep(80000);
			getKw(fre_control);

			//R_in测量
			getRin();//比原来的多了一行继电器定义
			lcdRectClear(383, 269, 455, 282, BACKGROUND);
			lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

			//R_out测量
			IOWR(SWITCH_4WIRE_BASE,0,0); //继电器0100--0000
			usleep(100000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_1 = getVpp(IORD(VPP_BASE,0));
			U_1_total = U_1_total + U_1;

			IOWR(SWITCH_4WIRE_BASE,0,8);//继电器1100-1000
			usleep(100000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_2 = getVpp(IORD(VPP_BASE,0));
			U_2_total = U_2_total + U_2;
			Round_U2++;
			if(Round_U2 == Aver_time){
				R_out = (fabs((U_1_total-U_2_total)/U_2_total))*Res2;
//				printf("U_1_total = %d, U_2_total = %d\n",U_1_total,U_2_total);
				U_inf_total = U_1_total;
				lcdRectClear(383, 319, 455, 332, BACKGROUND);
				lcdDispFloatSmall(383, 319, BLACK, WHITE, R_out);
				Round_U2 = 0;U_1_total = 0;U_2_total = 0;

				//Gain测量
				Gain=(U_inf_total/fabs(U_0_total-U_r_total))*Av;
				lcdRectClear(383, 369, 455, 382, BACKGROUND);
				lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);
			}

			usleep(100000);
		}

		//上限频率测量 && 幅频特性绘制
		else if((!Troubleshooting) && (draw_flag)){
			printf("Begin to draw!\n");
			end_flag = 0;
			lcdRectClear(260, 40, 764, 230, BACKGROUND);
			lcdDrawGrid(260, 50, 10, 28, 18, LGRAY);
			Sweep(Range_down_data,Range_up_data);

			IOWR(SWITCH_4WIRE_BASE,0,4);//继电器0000-->0100
			usleep(100000);
		}

		//发挥功能
		else if(Troubleshooting && (!draw_flag)){
			test();
			usleep(100000);
/*
			//低通部分，直流检测（设fre=1000Hz;
			IOWR(SWITCH_4WIRE_BASE,0,6);//继电器0010-->0110
			usleep(100000);
			fre_control = 1000;
			getKw(fre_control);
			usleep(750000);
			double V_DC = getVdc(IORD(DOUT_BASE, 0));	//单位：mV
			lcdRectClear(525, 310, 625, 330, BACKGROUND);
			lcdDispFloatSmall(525, 310, NAVY, WHITE, V_DC);

			/*
			 * V_DC正常电压：
			 * 异常电压【电阻故障】：
			 * R1断、R2短、R4断：		|| R1-R_in=15k | R2-R_in=0 | R4-R_in = 10.8k
			 * R1短、R3短：			|| R1-R_in=0 | R3-R_in=R_in
			 * R2断：
			 * R3断：
			 * R4断：
			 */
/*
			if(V_DC > 2100 || (V_DC < 1500 && V_DC > 5)){//电压不正常，电阻故障
				if(V_DC < 1200 && V_DC > 800){//典型值：1000——R2断路
					lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(V_DC < 300 && V_DC > 200){//典型值：250——R3断路
					lcdDispStringBig(560, 363, NAVY, WHITE, "R3");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(V_DC < 50 && V_DC > 5){//典型值：10——R4短路
					lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
					lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
				}
				else if(V_DC < 3200 && V_DC > 2900){//典型值：3000——R1短||R3短
					getRin();
					lcdRectClear(383, 269, 455, 282, BACKGROUND);
					lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);////////////////////////////////////////////

					double R_in_ref = R_in*10;
				    if(R_in_ref < 2.5 && R_in_ref > 0.5){//典型值:0.14——R1短路
						lcdDispStringBig(560, 363, NAVY, WHITE, "R1");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
						R_in_ref = 0;
					}
					else{
						lcdDispStringBig(560, 363, NAVY, WHITE, "R3");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
						R_in_ref = 0;
				    }
				}
				else if(V_DC < 2900 && V_DC > 2500){//典型值：2800——R1断|R2短|R4断
					getRin();
					lcdRectClear(383, 269, 455, 282, BACKGROUND);
					lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);//////////////////////////////////////////////

				    if(R_in < 0.25 && R_in > 0.05){//典型值:0.14——R2短路
						lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
					}
				    else if(R_in < 16.5 && R_in > 13.5){//典型值：15——R1断路
						lcdDispStringBig(560, 363, NAVY, WHITE, "R1");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
				    else if(R_in < 1.5 && R_in > 0.5){//典型值：0.8——R4断路
						lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
				}
			}

			else{//电容错误判断
				/*
				 * C1断-R_in=∞|C1双-低频情况下，R_in增大
				 * C2断-G=2.41|C2双-低频情况下，G增大
				 * C3断-高频情况下，G增大|C3双-高频情况下，G减小
				 */
/*
				getRin();
				lcdRectClear(383, 269, 455, 282, BACKGROUND);
				lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);//////////////////////////////////////

				if(R_in > 400){//典型值：∞——C1断
					lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(R_in < 12 && R_in > 8){//典型值：10.1？——C2断
					lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else{
					//高频情况：C3断|C3双
//					IOWR(SWITCH_4WIRE_BASE,0,4);//继电器0000-->0100
//					usleep(100000);
					fre_control = 100000;//100k
					getKw(fre_control);
					usleep(10000);
					getRin();

					IOWR(SWITCH_4WIRE_BASE,0,0); //继电器0100-->0000
					usleep(100000);
					while(!IORD(VPP_FOUND_BASE,0));
					U_1 = getVpp(IORD(VPP_BASE,0));
					U_1_total = U_1_total + U_1;
					Round_U2++;
					if(Round_U2 == Aver_time){
						U_inf_total = U_1_total;
						Round_U2 = 0;U_1_total = 0;
						Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;
						lcdRectClear(383, 369, 455, 382, BACKGROUND);
						lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);////////////////////////////////////
					}

					if(Gain > 150 && Gain < 180){//典型值：160-C3断
						lcdDispStringBig(560, 363, NAVY, WHITE, "C3");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
					}
					else if(Gain < 100 && Gain > 85){//典型值：80-C3双倍
						lcdDispStringBig(560, 363, NAVY, WHITE, "C3");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
					}
					else{
						//低频情况：C1双|C2双
						fre_control = 100;
						getKw(fre_control);
						usleep(50000);
						double R_in_before;
						R_in_before = R_in;
						getRin();

						IOWR(SWITCH_4WIRE_BASE,0,0); //继电器0100-->0000
						usleep(100000);
						while(!IORD(VPP_FOUND_BASE,0));
						U_1 = getVpp(IORD(VPP_BASE,0));
						U_1_total = U_1_total + U_1;
						Round_U2++;
						if(Round_U2 == Aver_time){
							U_inf_total = U_1_total;
							Round_U2 = 0;U_1_total = 0;
							Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;
							lcdRectClear(383, 369, 455, 382, BACKGROUND);
							lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);////////////////////////////////////
						}

						if(Gain > 80 && Gain < 110){//典型值：105-C2双倍
							lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
							lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
						}
						else{
							if((R_in - R_in_before) > 5){//R_in增大-C1双倍
								lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
								lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
							}
							else{
								//正常情况
								lcdRectClear(560, 363, 590, 390, WHITE);
								lcdDispStringBig(663, 363, NAVY, WHITE, "Normal");
							}
						}
					}
				}
			}
*/
		}
	}
	return 0;
}

/********************************************屏幕绘制函数*********************************************************/

void dis_init(){
	lcdDispNumtable(Num_X, Num_Y);
	lcdDrawGrid(260, 50, 10, 28, 18, LGRAY);
	lcdDrawRect(260, 40, 764, 230, BLACK);
	lcdDispStringSmall(230, 15, BLACK, WHITE, "Normalized_Vpp");
	lcdDispStringSmall(250, 45, BLACK, WHITE, "1");

	lcdDrawRect(50, 330, 200, 450, BLACK);
	lcdDrawHorLine(370, 50, 200, BLACK);
	lcdDrawHorLine(410, 50, 200, BLACK);
	lcdDrawVerLine(125, 370, 450, BLACK);
	lcdDispStringSmall(65, 385, BLACK, WHITE, "F_L/Hz");
	lcdDispStringSmall(65, 425, BLACK, WHITE, "F_H/Hz");
	lcdDispDecSmall(129, 425, BLACK, WHITE, 1000000);
	lcdDispDecSmall(129, 385, BLACK, WHITE, 100);


	lcdDrawRect(260, 250, 764, 450, BLACK);
	lcdDrawHorLine(300, 260, 511, BLACK);
	lcdDrawHorLine(300, 513, 764, BLACK);
	lcdDrawHorLine(350, 260, 511, BLACK);
	lcdDrawHorLine(400, 260, 511, BLACK);
	lcdDrawVerLine(380, 250, 450, BLACK);
	lcdDrawVerLine(511, 250, 450, BLACK);
	lcdDrawVerLine(513, 250, 450, BLACK);
	lcdDrawVerLine(635, 300, 450, BLACK);
	lcdDispStringBig(267, 263, BLACK, WHITE, "R_in");
	lcdDispStringBig(267, 313, BLACK, WHITE, "R_out");
	lcdDispStringBig(267, 363, BLACK, WHITE, "Av");
	lcdDispStringBig(267, 413, BLACK, WHITE, "frecutoff");

	lcdDispStringBig(458, 263, BLACK, WHITE, "Kohm");
	lcdDispStringBig(458, 313, BLACK, WHITE, "Kohm");
	lcdDispStringBig(458, 363, BLACK, WHITE, "V/V");
	lcdDispStringBig(458, 413, BLACK, WHITE, "Hz");

	lcdDispStringBig(550, 263, BLACK, WHITE, "Error Diagnosis");
}

/********************************************IO部分初始化********************************************************/

void io_init(){
	fre_control = 1000;
	getKw(fre_control);
	IOWR(SWITCH_4WIRE_BASE,0,4);//继电器0000--》0100
	usleep(100000);
}

/********************************************标志位初始化*********************************************************/

void flag_init(){
	Troubleshooting = 0;
	end_flag = 0;
	draw_flag = 0;
	draw_end = 1;
	Range_w = 0;//修改F_L
	mode = 0;//测量模式
	lcdDispStringSmall(60, 344, RED, WHITE, "Measuring");
}

/*******************************************扫频信号产生函数*******************************************************/

int Sweep(int Rang_down,int Rang_up){
	float freq_sweep = Rang_down;
	IOWR(SWITCH_4WIRE_BASE,0,1);//继电器0101-0001
    usleep(100000);

    while(!end_flag){
    	if(Troubleshooting)
    		end_flag = 1;
//        IOWR(SWITCH_4WIRE_BASE,0,1);//继电器0101-0001
//        usleep(100000);
		if(freq_sweep <= 200)							//0~200
		{
			freq_sweep = freq_sweep+30;
      	}
        else if(freq_sweep <= 1000 && freq_sweep > 200)	//200~1000
        {
           	freq_sweep = freq_sweep+200;
        }
        else if(freq_sweep <= 50000 && freq_sweep > 1000)//1k~50k
        {
			freq_sweep = freq_sweep+1000;
		}
		else if(freq_sweep <= 100000 && freq_sweep>50000)//50k~100k
		{
			freq_sweep = freq_sweep+2000;
		}
		else if(freq_sweep <= Rang_up && freq_sweep > 100000)	 //100k~上限
		{
			freq_sweep = freq_sweep + 20000;
		}
		else if(freq_sweep > Rang_up)
		{
			freq_sweep = Rang_down;
		}
		getKw(freq_sweep);
		usleep(60000);

		Paint_V_f(freq_sweep, Rang_down, Rang_up);
    }
    return 0;
}

/******************************************R_in测量函数*********************************************************/

void getRin(){
	IOWR(SWITCH_4WIRE_BASE,0,4);//继电器0000-->0100
	usleep(100000);
	while(!IORD(VPP_FOUND_BASE,0));
	U_B = getVpp(IORD(VPP_BASE,0));
	U_B_total = U_B_total + U_B;

	IOWR(SWITCH_4WIRE_BASE,0,5);//继电器0001-->0101
	usleep(100000);
	while(!IORD(VPP_FOUND_BASE,0));
	U_A = getVpp(IORD(VPP_BASE,0));
	U_A_total = U_A_total + U_A;
	Round_U1++;
	if(Round_U1 == Aver_time){
		R_in = (fabs((U_A_total-U_B_total)/U_B_total))*Res1;
//		printf("U_A_total = %d, U_B_total = %d\n",U_A_total,U_B_total);
		U_0_total = U_A_total;U_r_total = U_B_total;
//		lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);
		Round_U1 = 0;U_A_total = 0;U_B_total = 0;
	}
}

/****************************************频率控制字获得函数********************************************************/

void getKw(double fre){
	//扫频信号频率控制字
	KW_word = (fre*pow(2,32))/400000000;
	IOWR(FRE_OUT_KW_BASE,0,KW_word);
	//ADS805采样时钟频率控制字
	if(fre >= 100000){
		FRE_word = (1/(1/fre+pow(10,-8))*pow(2,32))/400000000;
		IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
		Times = pow(10,8)/fre;
		IOWR(TIMES_BASE,0,4*Times);
	}
	else
	{
		FRE_word = ((fre*200)*pow(2,32))/400000000;
		IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
		Times = 400;
		IOWR(TIMES_BASE,0,Times);
	}
}

/****************************************幅频特性曲线绘制函数*******************************************************/

void Paint_V_f(float fre_sweep,int Range_down,int Range_up){
	while(!IORD(VPP_FOUND_BASE,0));//继电器0101
	double addata_in = getVpp(IORD(VPP_BASE,0));

	//寻找截止频率点+存储数据准备绘图
	if(draw_end){
		Vpp_pf[rdaddress+1] = addata_in;
//		fre_trans_pf[rdaddress] = 95*log10(fre_sweep)+16;//非均匀量化
		fre_trans_pf[rdaddress] = 126*log10(fre_sweep)+8;//非均匀量化
		fre_pf[rdaddress] = fre_sweep;
		rdaddress++;

		if(addata_in > max_of_Vpp){
			max_of_Vpp = addata_in;
		}

		if(rdaddress == rda_end){
			storge_end = 1;
			rdaddress = 0;
			for(int i = 1; i < rda_end - 1; i++){
				if((Vpp_pf[i]/Vpp_pf[i-1] >= 2) && (Vpp_pf[i]/Vpp_pf[i+1] >= 2)){
					Vpp_pf[i] = (Vpp_pf[i-1]+Vpp_pf[i])/2;
				}
				if(Vpp_pf[i-1] > max_of_Vpp*0.7 && Vpp_pf[i+1] <= max_of_Vpp*0.7){
					fre_cut = fre_pf[i];
					lcdRectClear(383, 419, 455, 432, BACKGROUND);
					lcdDispDecSmall(383, 419, BLACK, WHITE, fre_cut);
				}
			}
			printf("Store over!\n");
		}
	}

	if(storge_end){
		printf("Begin drawing V-f!\n");
		storge_end = 0;
		draw_end = 0;
		for(int i = 0; i < rda_end-1; i++){
			// Vpp_draw[i] = 230-180*Vpp_pf[i]/max_of_Vpp--y
			// fre_draw[i] = 764-504*fre_trans_pf[i]/fre_range--x--原值
			if(Vpp_pf[i+1] >= 0 && (fre_pf[i] <= fre_pf[i+1])){
//				printf("fre_trans_pf[%d] = %d\n",i,fre_trans_pf[i]);
				lcdDrawLine(fre_trans_pf[i], 230-180*(Vpp_pf[i]/max_of_Vpp), fre_trans_pf[i+1], 230-180*(Vpp_pf[i+1]/max_of_Vpp), BLACK);
			}
		}
		printf("Finish drawing!\n");
		draw_end = 1;
		end_flag = 1;
		max_of_Vpp = -100;
	}
}

/*************************************将ADS805读取到的值转化为真实电压值-单位：V****************************************************/

double getVpp(double vrd){
	double vin;
	vin = vrd * 1.312-1.323;
	if(vin <= 0){
		vin = vrd;
	}
//	vin = vin/1000;
	return vin;
}

/****************************************将ADS1118采到的值变成电压/mV*******************************************************/

double getVdc(u16 AD_value){
    //FS是指电压等级
    double AD_Voltage;
    if(AD_value>=0x8000)
    {
    	AD_value=0xFFFF-AD_value;//把0xFFFF改成0x10000
        AD_Voltage=(-1.0)*((AD_value*FS/0x8000));
    }
    else
    	AD_Voltage=(1.0)*((AD_value*FS/32768));
    return AD_Voltage;
}

/**********************************************发挥功能部分测试程序********************************************************/
/*----------------------------------------计算并显示R_in|R_out|Gain|V_DC----------------------------------------------*/
void test(){
	fre_control = 1000;
	getKw(fre_control);

	//R_in测量
	getRin();//继电器0000-0001-->0100-0101
	lcdRectClear(383, 269, 455, 282, BACKGROUND);
	lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

	//R_out测量
	IOWR(SWITCH_4WIRE_BASE,0,0); //继电器0100-->0000
	usleep(100000);
	while(!IORD(VPP_FOUND_BASE,0));
	U_1 = getVpp(IORD(VPP_BASE,0));
	U_1_total = U_1_total + U_1;

	IOWR(SWITCH_4WIRE_BASE,0,8);//继电器1100-->1000
	usleep(100000);
	while(!IORD(VPP_FOUND_BASE,0));
	U_2 = getVpp(IORD(VPP_BASE,0));
	U_2_total = U_2_total + U_2;
	Round_U2++;
	if(Round_U2 == Aver_time){
		R_out = (fabs((U_1_total-U_2_total)/U_2_total))*Res2;
		U_inf_total = U_1_total;
		lcdRectClear(383, 319, 455, 332, BACKGROUND);
		lcdDispFloatSmall(383, 319, BLACK, WHITE, R_out);
		Round_U2 = 0;U_1_total = 0;U_2_total = 0;

		//Gain测量
		Gain=(U_inf_total/fabs(U_0_total-U_r_total))*Av;
		lcdRectClear(383, 369, 455, 382, BACKGROUND);
		lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);
	}

	IOWR(SWITCH_4WIRE_BASE,0,6);//继电器0010-->0110
	usleep(100000);
	double V_DC = getVdc(IORD(DOUT_BASE, 0));	//单位：mV
	lcdRectClear(525, 310, 625, 330, BACKGROUND);
	lcdDispFloatSmall(525, 310, NAVY, WHITE, V_DC);
}
