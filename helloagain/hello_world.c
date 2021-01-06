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

#define Res1 10	//R_in�ο�����
#define Res2 1
#define Av 99	//�Ǳ�Ŵ����Ŵ�ֵ
#define Aver_time 15 //��β���ȡƽ��ֵ

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

/*
 * U_A����ο�����̽����õ����---in
 * U_B����ο�����������õ����---in
 * U_1����ο������·���õ����---out
 * U_2����ο�����������õ����---out
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

	//��ʼ��
	lcdRectClear(0, 0, 799, 479, WHITE);
	fre_control = 1000;
	Rang_down_data=10.0;
	Rang_up_data=1000000.0;//1M
	IOWR(SWITCH_4WIRE_BASE,0,0);

	while (1)
	{
		if(!Troubleshooting && stop_flag){

			IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0000
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

			//R_in����
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
//				lcdDispFloatBig(197, 297, BLACK, WHITE, R_in);
				Round_U1 = 0;U_A_total = 0;U_B_total = 0;
			}

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
			U_2_total = U_1_total + U_2;
			Round_U2++;
			if(Round_U2 == Aver_time){
				R_out = (fabs(U_1_total-U_2_total)/U_2_total)*Res2;
				U_inf_total = U_1_total;
//				lcdDispFloatBig(197, 342, BLACK, WHITE, R_out, 8);
				Round_U2 = 0;U_1_total = 0;U_2_total = 0;

				//Gain����
				Gain=U_inf_total/fabs(U_0_total-U_r_total)*Av;

				//����Ƶ�ʲ��� && ��Ƶ���Ի���
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
			if(Gain>max_of_DB) //�����������Сֵ
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

		//���ӹ���
		else{

		}
	}

	return 0;
}

/*******************************************ɨƵ�źŲ�������*******************************************************/

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

		//ɨƵ�ź�Ƶ�ʿ�����
		KW_word = (freq_sweep*pow(2,32))/100000000;
		IOWR(FRE_OUT_KW_BASE,0,KW_word);
		//ADS805����ʱ��Ƶ�ʿ�����
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
