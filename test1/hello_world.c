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
/*
#define Res1 10	//R_in�ο�����
#define Res2 1
#define Av 99	//�Ǳ�Ŵ����Ŵ�ֵ
#define Aver_time 15 //��β���ȡƽ��ֵ
#define FS 4096	//ADS1118��ֵ��ʵ��ֵΪ4.096V

int begin_flag,end_flag=0;
int draw_flag,stop_flag;
int draw_end = 1, storge_end;
double Rang_down_data,Rang_up_data;
int KW_word,FRE_word;	//KW_word:ɨƵ�ź�Ƶ�ʿ����֣�FRE_word:����ʱ��Ƶ�ʿ�����
int Times;
double fre_control;
double U_A,U_A_total,U_B,U_B_total,U_1,U_1_total,U_2,U_2_total;
double U_inf_total,U_0_total,U_r_total;
double R_in,R_out;
double Gain;
int Round_U1 = 0,Round_U2;
*/
/*
 * U_A����ο�����̽����õ����---in
 * U_B����ο�����������õ����---in
 * U_1����ο������·���õ����---out
 * U_2����ο�����������õ����---out
 * U_inf = U_1;U_0 = U_A;U_r = U_B;
 */
/*
int Troubleshooting;
int rda_end = 129;
double max_of_Vpp = -100;
double fre_cut,fre_range;
float Vpp_pf[600],fre_trans_pf[600],fre_pf[600];

double getVpp(double vrd);
int Sweep(int Rang_down,int Rang_up);
void Paint_V_f(float fre_sweep,int Range_down,int Range_up);*/
void dis_init();
/*
double getVdc(u16 AD_value);
void getRin();
void getKw(double fre);
*/

int main() {
	if (deviceInit() < 0) {
		printf("device initial failed\n");
		return -1;
	}
	lcdRectClear(0, 0, 799, 479, WHITE);
	dis_init();
/*	fre_control = 1000;
	Rang_down_data=10.0;
	Rang_up_data=1000000.0;//1M
	IOWR(SWITCH_4WIRE_BASE,0,0);

	Troubleshooting = 0;
	stop_flag = 1;
*/
	printf("Finish Initial\n");

	//���Ĵ���
	while (1)
	{
		/*
		if(!Troubleshooting && stop_flag){
			IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0000
			usleep(80000);
			getKw(fre_control);

			//R_in����
			getRin();//��ԭ���Ķ���һ�м̵�������
			lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

			//R_out����
			IOWR(SWITCH_4WIRE_BASE,0,4); //�̵���0100
			usleep(60000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_1 = getVpp(IORD(VPP_BASE,0));
			U_1_total = U_1_total + U_1;

			IOWR(SWITCH_4WIRE_BASE,0,12);//�̵���1100
			usleep(60000);
			while(!IORD(VPP_FOUND_BASE,0));
			U_2 = getVpp(IORD(VPP_BASE,0));
			U_2_total = U_2_total + U_2;
			Round_U2++;
			if(Round_U2 == Aver_time){
				R_out = (fabs(U_1_total-U_2_total)/U_2_total)*Res2;
				U_inf_total = U_1_total;
				lcdDispFloatSmall(383, 319, BLACK, WHITE, R_out);
				Round_U2 = 0;U_1_total = 0;U_2_total = 0;

				//Gain����
				Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;
				lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);

				//����Ƶ�ʲ��� && ��Ƶ���Ի���
				if(draw_flag){
					end_flag = 0;
					lcdDrawGrid(260, 50, 10, 28, 18, LGRAY);
					lcdRectClear(260, 40, 764, 230, BACKGROUND);
					Sweep(Rang_down_data,Rang_up_data);

					while(!IORD(VPP_FOUND_BASE,0));
					U_2 = getVpp(IORD(VPP_BASE,0));
					Gain = U_2/22.246;

					IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0000
					usleep(70000);
				}
			}
		}

		//���ӹ���
		else if(Troubleshooting && stop_flag){
			//��ͨ���֣�ֱ����⣨��fre=1000Hz;
			IOWR(SWITCH_4WIRE_BASE,0,2);//�̵���0010
			usleep(80000);
			fre_control = 1000;
			getKw(fre_control);
			usleep(750000);
			double V_DC = getVdc(IORD(DOUT_BASE, 0));	//��λ��mV
			lcdDispFloatSmall(525, 310, NAVY, WHITE, V_DC);
*/
			/*
			 * V_DC������ѹ��
			 * �쳣��ѹ��������ϡ���
			 * R1�ϡ�R2�̡�R4�ϣ�		|| R1-R_in=15k | R2-R_in=0 | R4-R_in = 10.8k
			 * R1�̡�R3�̣�			|| R1-R_in=0 | R3-R_in=R_in
			 * R2�ϣ�
			 * R3�ϣ�
			 * R4�ϣ�
			 */
/*
			if(V_DC > 2100 || (V_DC < 1500 && V_DC > 5)){//��ѹ���������������
				if(V_DC < 1200 && V_DC > 800){//����ֵ��1000����R2��·
					lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(V_DC < 300 && V_DC > 200){//����ֵ��250����R3��·
					lcdDispStringBig(560, 363, NAVY, WHITE, "R3");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(V_DC < 50 && V_DC > 5){//����ֵ��10����R4��·
					lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
					lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
				}
				else if(V_DC < 3200 && V_DC > 2900){//����ֵ��3000����R1��||R3��
					getRin();
//					lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

					double R_in_ref = R_in*10;
				    if(R_in_ref < 2.5 && R_in_ref > 0.5){//����ֵ:0.14����R1��·
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
				else if(V_DC < 2900 && V_DC > 2500){//����ֵ��2800����R1��|R2��|R4��
					getRin();
//					lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

				    if(R_in < 0.25 && R_in > 0.05){//����ֵ:0.14����R2��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
					}
				    else if(R_in < 16.5 && R_in > 13.5){//����ֵ��15����R1��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R1");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
				    else if(R_in < 1.5 && R_in > 0.5){//����ֵ��0.8����R4��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
				}
			}

			else{//���ݴ����ж�
			*/
				/*
				 * C1��-R_in=��|C1˫-��Ƶ����£�R_in����
				 * C2��-G=2.41|C2˫-��Ƶ����£�G����
				 * C3��-��Ƶ����£�G����|C3˫-��Ƶ����£�G��С
				 */
/*				getRin();
//				lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

				if(R_in > 400){//����ֵ���ޡ���C1��
					lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(R_in < 12 && R_in > 8){//����ֵ��10.1������C2��
					lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else{
					//��Ƶ�����C3��|C3˫
					IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0000
					usleep(50000);
					fre_control = 100000;//100k
					getKw(fre_control);
					usleep(10000);
					getRin();

					IOWR(SWITCH_4WIRE_BASE,0,4); //�̵���0100
					usleep(60000);
					while(!IORD(VPP_FOUND_BASE,0));
					U_1 = getVpp(IORD(VPP_BASE,0));
					U_1_total = U_1_total + U_1;
					Round_U2++;
					if(Round_U2 == Aver_time){
						U_inf_total = U_1_total;
						Round_U2 = 0;U_1_total = 0;
						Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;
//						lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);
					}

					if(Gain > 150 && Gain < 180){//����ֵ��160-C3��
						lcdDispStringBig(560, 363, NAVY, WHITE, "C3");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
					}
					else if(Gain < 100 && Gain > 85){//����ֵ��80-C3˫��
						lcdDispStringBig(560, 363, NAVY, WHITE, "C3");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
					}
					else{
						//��Ƶ�����C1˫|C2˫
						fre_control = 100;
						getKw(fre_control);
						usleep(50000);
						double R_in_before;
						R_in_before = R_in;
						getRin();

						IOWR(SWITCH_4WIRE_BASE,0,4); //�̵���0100
						usleep(60000);
						while(!IORD(VPP_FOUND_BASE,0));
						U_1 = getVpp(IORD(VPP_BASE,0));
						U_1_total = U_1_total + U_1;
						Round_U2++;
						if(Round_U2 == Aver_time){
							U_inf_total = U_1_total;
							Round_U2 = 0;U_1_total = 0;
							Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;
	//						lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);
						}

						if(Gain > 80 && Gain < 110){//����ֵ��105-C2˫��
							lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
							lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
						}
						else{
							if((R_in - R_in_before) > 5){//R_in����-C1˫��
								lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
								lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
							}
							else{
								//�������
								lcdRectClear(560, 363, 590, 390, WHITE);
								lcdDispStringBig(663, 363, NAVY, WHITE, "Normal");
							}
						}
					}
				}
			}
		}
		*/
	}
	return 0;
}

/********************************************��Ļ���ƺ���*********************************************************/

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

/*******************************************ɨƵ�źŲ�������*******************************************************/
/*
int Sweep(int Rang_down,int Rang_up){
	float freq_sweep = Rang_down - 20;
    IOWR(SWITCH_4WIRE_BASE,0,5);	//�̵���0101

    while(!end_flag){//��ʱ���ɨƵ�źŵĲ�����ɨƵ�źŶ�Ӧ��Ƶ�ʿ����֡�805����ʱ�ӿ�����
        IOWR(SWITCH_4WIRE_BASE,0,5);//�̵���0101
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
		else if(freq_sweep <= Rang_up && freq_sweep > 100000)	 //100k~����
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
*/
/******************************************R_in��������*********************************************************/
/*
void getRin(){
	IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0000
	while(!IORD(VPP_FOUND_BASE,0));
	U_B = getVpp(IORD(VPP_BASE,0));
	U_B_total = U_B_total + U_B;

	IOWR(SWITCH_4WIRE_BASE,0,1);//�̵���0001
	usleep(60000);
	while(!IORD(VPP_FOUND_BASE,0));
	U_A = getVpp(IORD(VPP_BASE,0));
	U_A_total = U_A_total + U_A;
	Round_U1++;
	if(Round_U1 == Aver_time){
		R_in = (fabs(U_A_total-U_B_total)/U_B_total)*Res1;
		U_0_total = U_A_total;U_r_total = U_B_total;
//		lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);
		Round_U1 = 0;U_A_total = 0;U_B_total = 0;
	}
}
*/
/****************************************Ƶ�ʿ����ֻ�ú���********************************************************/
/*
void getKw(double fre){
	//ɨƵ�ź�Ƶ�ʿ�����
	KW_word = (fre*pow(2,32))/100000000;
	IOWR(FRE_OUT_KW_BASE,0,KW_word);
	//ADS805����ʱ��Ƶ�ʿ�����
	if(fre >= 100000){
		FRE_word = (1/(1/fre+pow(10,-8))*pow(2,32))/100000000;
		IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
		Times = pow(10,8)/fre;
		IOWR(TIMES_BASE,0,2*Times);
	}
	else
	{
		FRE_word = ((fre*200)*pow(2,32))/100000000;
		IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
		Times = 200;
		IOWR(TIMES_BASE,0,Times);
	}

}
*/
/****************************************��Ƶ�������߻��ƺ���*******************************************************/
/*
void Paint_V_f(float fre_sweep,int Range_down,int Range_up){
	int rdaddress = 0;
	while(!IORD(VPP_FOUND_BASE,0));
	double addata_in = getVpp(IORD(VPP_BASE,0));

	//Ѱ�ҽ�ֹƵ�ʵ�+�洢����׼����ͼ
	if(draw_end){
		Vpp_pf[rdaddress] = addata_in;
		fre_trans_pf[rdaddress] = 95*log10(fre_sweep)+16;//�Ǿ�������
		fre_pf[rdaddress] = fre_sweep;
		rdaddress++;

		if(addata_in > max_of_Vpp){
			max_of_Vpp = addata_in;
		}

		if(rdaddress == rda_end){
			storge_end = 1;
			rdaddress = 0;
			for(int i = 1; i < rda_end - 1; i++){
				if(Vpp_pf[i-1] > max_of_Vpp*0.7 && Vpp_pf[i+1] <= max_of_Vpp*0.7){
					fre_cut = fre_pf[i];
					lcdDispFloatSmall(383, 419, BLACK, WHITE, fre_cut);
				}
			}
		}
	}

	if(storge_end){
		storge_end = 0;
		draw_end = 0;
		fre_range = (95*log10(Range_up)+16)-(95*log10(Range_down)+16);
		for(int i=0; i < rda_end-1; i++){
			// Vpp_draw[i] = 250-200*Vpp_pf[i]/max_of_Vpp--y
			// fre_draw[i] = 764-504*fre_trans_pf[i]/fre_range--x
			if(Vpp_pf[i+1] >= 0){
				lcdDrawLine(764-504*fre_trans_pf[i]/fre_range, 250-200*Vpp_pf[i]/max_of_Vpp, 764-504*fre_trans_pf[i+1]/fre_range, 250-200*Vpp_pf[i+1]/max_of_Vpp, BLACK);
			}
		}
		draw_end = 1;
		end_flag = 1;
		max_of_Vpp = -100;
	}
}
*/
/*************************************��ADS805��ȡ����ֵת��Ϊ��ʵ��ѹֵ*****************************************************/
/*
double getVpp(double vrd){
	double vin;
	vin = vrd * 1.312-1.323;
	return vin;
}
*/
/****************************************��ADS1118�ɵ���ֵ��ɵ�ѹ/mV*******************************************************/
/*
double getVdc(u16 AD_value){
    //FS��ָ��ѹ�ȼ�
    double AD_Voltage;
    if(AD_value>=0x8000)
    {
    	AD_value=0xFFFF-AD_value;//��0xFFFF�ĳ�0x10000
        AD_Voltage=(-1.0)*((AD_value*FS/0x8000));
    }
    else
    	AD_Voltage=(1.0)*((AD_value*FS/32768));
    return AD_Voltage;
}
*/
