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

#define Res1 10	//R_in参考电阻
#define Res2 1
#define Av 99	//仪表放大器放大值
#define Aver_time 15 //多次测量取平均值

int begin_flag,end_flag=0;
int draw_flag,stop_flag;
int draw_end = 1, storge_end;
double Rang_down_data,Rang_up_data;
int KW_word,FRE_word;	//KW_word:扫频信号频率控制字；FRE_word:采样时钟频率控制字
int Times;
double fre_control;
double U_A,U_A_total,U_B,U_B_total,U_1,U_1_total,U_2,U_2_total;
double U_inf_total,U_0_total,U_r_total;
double R_in,R_out;
double Gain;
int Round_U1 = 0,Round_U2;

/*
 * U_A输入参考电阻短接所得的输出---in
 * U_B输入参考电阻接入所得的输出---in
 * U_1输出参考电阻断路所得的输出---out
 * U_2输出参考电阻接入所得的输出---out
 * U_inf = U_1;U_0 = U_A;U_r = U_B;
 */

int Troubleshooting;

double getVpp(double vrd);
int Sweep(int Rang_down,int Rang_up);

int main() {
	printf("Hello lemon\n");
	if (deviceInit() < 0) {
		printf("device initial failed\n");
		return -1;
	}

	//初始化
	lcdRectClear(0, 0, 799, 479, WHITE);
	fre_control = 1000;
	Rang_down_data=10.0;
	Rang_up_data=1000000.0;//1M
	IOWR(SWITCH_4WIRE_BASE,0,0);

	while (1)
	{
		if(!Troubleshooting && stop_flag){

			IOWR(SWITCH_4WIRE_BASE,0,0);//继电器0000
			usleep(80000);
	        KW_word = (fre_control*pow(2,32))/100000000;
			IOWR(FRE_OUT_KW_BASE,0,KW_word);
			if(fre_control >= 100000){
				FRE_word = (1/(1/fre_control+pow(10,-8))*pow(2,32))/100000000;
				IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
				Times = pow(10,8)/fre_control;
				IOWR(TIMES_BASE,0,2*Times);
			}
			else
			{
				FRE_word = ((fre_control*200)*pow(2,32))/100000000;
				IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
				Times = 200;
				IOWR(TIMES_BASE,0,Times);
			}

			//R_in测量
			while(!IORD(VPP_FOUND_BASE,0));
			U_B = getVpp(IORD(VPP_BASE,0));
			U_B_total = U_B_total + U_B;

			IOWR(SWITCH_4WIRE_BASE,0,1);//继电器0001
			usleep(60000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_A = getVpp(IORD(VPP_BASE,0));
			U_A_total = U_A_total + U_A;
			Round_U1++;
			if(Round_U1 == Aver_time){
				R_in = (fabs(U_A_total-U_B_total)/U_B_total)*Res1;
				U_0_total = U_A_total;U_r_total = U_B_total;
//				lcdDispFloatBig(197, 297, BLACK, WHITE, R_in);
				Round_U1 = 0;U_A_total = 0;U_B_total = 0;
			}

			//R_out测量
			IOWR(SWITCH_4WIRE_BASE,0,4); //继电器0100
			usleep(60000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_1 = getVpp(IORD(VPP_BASE,0));
			U_1_total = U_1_total + U_1;

			IOWR(SWITCH_4WIRE_BASE,0,12);//继电器1100
			usleep(60000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_2 = getVpp(IORD(VPP_BASE,0));
			U_2_total = U_1_total + U_2;
			Round_U2++;
			if(Round_U2 == Aver_time){
				R_out = (fabs(U_1_total-U_2_total)/U_2_total)*Res2;
				U_inf_total = U_1_total;
//				lcdDispFloatBig(197, 342, BLACK, WHITE, R_out, 8);
				Round_U2 = 0;U_1_total = 0;U_2_total = 0;

				//Gain测量
				Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;

				//上限频率测量 && 幅频特性绘制
				if(draw_flag){
					end_flag = 0;
//					lcdRectClear(130, 10, 580, 210, BACKGROUND);
//					lcdDrawGrid(130, 10, 8, 18, 25, LGRAY);
					Sweep(Rang_down_data,Rang_up_data);

					while(!IORD(VPP_FOUND_BASE,0));
					U_2 = getVpp(IORD(VPP_BASE,0));
					Gain = U_2/22.246;
				}

				/*
				if(draw_flag)
				{

				/*

		while(!IORD(VPP_FOUND_BASE,0));
		AD_805_data=IORD(VPP_BASE,0)*1.312-1.323;
		Gain=AD_805_data/22.246;

		if(draw_end)
		{
			datasheet_pf[rdaddress]=Gain;//302.0-92.0*log10(AD_805_data);
			addr_pf[rdaddress]=95*log10(freq_sweep)+16;
			freq_pf[rdaddress]=freq_sweep;
			rdaddress++;
			if(Gain>max_of_DB) //求数据最大最小值
			{
				max_of_DB=Gain;
			}
			if(rdaddress==rda_end) //
			{
				storge_end=1;
				rdaddress=0;
				for(int j=1;j<rda_end-1;j++)
				{
					if(datasheet_pf[j-1]>max_of_DB*0.7&&datasheet_pf[j+1]<=max_of_DB*0.7)
					{
						freq_sweep_1=freq_pf[j];
						lcdDispFloatBig(197, 423, BLACK, WHITE, freq_sweep_1,8);
					}
				}
			 }
		}
				 */
/*
					IOWR(SWITCH_4WIRE_BASE,0,0);//TODO SWITCH
					usleep(70000);
				}
				 */

			}
		}

		//发挥功能
		else{

		}
	}

	return 0;
}

/*******************************************扫频信号产生函数*******************************************************/

int Sweep(int Rang_down,int Rang_up){
	float freq_sweep = Rang_down - 20;
    IOWR(SWITCH_4WIRE_BASE,0,5);	//继电器0101

    while(!end_flag){//暂时完成扫频信号的产生与扫频信号对应的频率控制字、805采样时钟控制字
        IOWR(SWITCH_4WIRE_BASE,0,5);//继电器0101
		if(freq_sweep <= 200)							//0~200
		{
			freq_sweep = freq_sweep + 30;
      	}
        else if(freq_sweep <= 1000 && freq_sweep > 200)	//200~1000
        {
           	freq_sweep = freq_sweep + 200;
        }
        else if(freq_sweep <= 50000 && freq_sweep > 1000)//1k~50k
        {
			freq_sweep = freq_sweep + 1000;
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

		//扫频信号频率控制字
		KW_word = (freq_sweep*pow(2,32))/100000000;
		IOWR(FRE_OUT_KW_BASE,0,KW_word);
		//ADS805采样时钟频率控制字
		if(freq_sweep >= 100000){
			FRE_word = (1/(1/freq_sweep+pow(10,-8))*pow(2,32))/100000000;
			IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
			Times = pow(10,8)/freq_sweep;
			IOWR(TIMES_BASE,0,2*Times);
		}
		else
		{
			FRE_word = ((freq_sweep*200)*pow(2,32))/100000000;
			IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
			Times = 200;
			IOWR(TIMES_BASE,0,Times);
		}

		usleep(60000);
    }
}

double getVpp(double vrd){
	double vin;
	vin = vrd * 1.312-1.323;
	return vin;
}
