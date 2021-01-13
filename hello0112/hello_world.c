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

#define Res1 10	//R_in�ο�����--103=10*10^3--10*1=10
#define Res2 3//R_out�ο�����--222=22*10^2--3*1=3
#define Av 66	//�Ǳ�Ŵ����Ŵ�ֵ--66
#define Aver_time 10 //��β���ȡƽ��ֵ
#define FS 4096	//ADS1118��ֵ��ʵ��ֵΪ4.096V
#define Switchsleep 80000
#define Fresleep 10000

double U_A_store[30]={0}, U_B_store[30]={0}, U_1_store[30]={0}, U_2_store[30]={0};
int end_flag, draw_end, storge_end;
int KW_word, FRE_word;	//KW_word:ɨƵ�ź�Ƶ�ʿ����֣�FRE_word:����ʱ��Ƶ�ʿ�����
int Times, rdaddress = 0;
double fre_control;
double U_A, U_A_total = 0, U_B, U_B_total = 0, U_1, U_1_total = 0, U_2, U_2_total = 0;
double U_inf_total = 0, U_0_total = 0, U_r_total = 0,U_0,U_r,U_inf;
double R_in, R_out;
double Gain;
int Round_U1 = 0,Round_U2;
int switch_num = 4;

/*
 * U_A����ο�����̽����õ����---in
 * U_B����ο�����������õ����---in
 * U_1����ο������·���õ����---out
 * U_2����ο�����������õ����---out
 * U_inf = U_1;U_0 = U_A;U_r = U_B;
 */

int rda_end = 194;
//int rda_end = 129;
double max_of_Vpp = -100;
//double fre_cut, fre_range;
int fre_cut[100];
float Vpp_pf[600], fre_trans_pf[600], fre_pf[600];

double getVpp(double vrd);
int Sweep(int Rang_down,int Rang_up);
void Paint_V_f(float fre_sweep,int Range_down,int Range_up);
void dis_init();
void io_init();
void flag_init();
double getVdc(u16 AD_value);
void getRin();
void getRout();
void getKw(double fre);
void test();
void err_gain();
void err_rin();

int main() {
	if (deviceInit() < 0) {
		printf("device initial failed\n");
		return -1;
	}
	lcdRectClear(0, 0, 799, 479, WHITE);
	dis_init();
	io_init();
	flag_init();
	Range_down_data = 500;
	Range_up_data = 500000;//500k

	printf("Finish Initial!\n");

	while (1)
	{
		if((!Troubleshooting) && (!draw_flag) && (!ifVDC)){
			fre_control = 1000;/////////////////
			getKw(fre_control);

			//R_in����
			getRin();
//			printf("R_in = %f\n",R_in);
			if(R_in>45){
				R_in = 43+(R_in-43)*0.01;
			}
			if(R_in>12 && R_in < 20){
				R_in = 11.1+(R_in-11.1)*0.01;
			}
			lcdRectClear(383, 269, 455, 282, BACKGROUND);
			lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);
			//R_out���� +gain
			getRout();
		}

		//����Ƶ�ʲ��� && ��Ƶ���Ի���
		else if((!Troubleshooting) && (draw_flag)){
			printf("Begin to draw!\n");
			end_flag = 0;
//			lcdRectClear(260, 40, 764, 230, BACKGROUND);
//			lcdDrawGrid(260, 50, 10, 28, 18, LGRAY);
			Sweep(Range_down_data,Range_up_data);

//			IOWR(SWITCH_4WIRE_BASE,0,4);//�̵���0000-->0100
//			usleep(Switchsleep);
		}

		//���ӹ���
		else if(Troubleshooting && (!draw_flag) && (!ifVDC)){
			//��ͨ���֣�ֱ����⣨��fre=1000Hz;
			fre_control = 1000;
			getKw(fre_control);
			if(switch_num != 6){
				switch_num = 6;
				IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0010-->0110
				usleep(Switchsleep);
			}
			usleep(100000);
			double V_DC = getVdc(IORD(DOUT_BASE, 0));	//��λ��mV
//			lcdRectClear(525, 310, 625, 330, BACKGROUND);
//			lcdDispFloatSmall(525, 310, NAVY, WHITE, V_DC);

			/*
			 * V_DC������ѹ��611mV~612mV
			 * �쳣��ѹ��������ϡ���<600||>620
			 * R1�ϡ�R2�̡�R4�ϣ�903mV	|| R1-R_in=15k | R2-R_in=0 | R4-R_in = 10.8k
			 * R1�̣�3338mV
			 * R3�̣�3589mV		|| R1-R_in=0 | R3-R_in=R_in
			 * R2�ϣ�625mV
			 * R3�ϣ�26mV
			 * R4�̣�9mV
			 */

 			if(V_DC > 620 || (V_DC < 600 && V_DC > 3)){//��ѹ���������������
				if(V_DC < 650 && V_DC > 620){//����ֵ��625����R2��·
					lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(V_DC < 50 && V_DC > 15){//����ֵ��26����R3��·
					lcdDispStringBig(560, 363, NAVY, WHITE, "R3");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(V_DC < 15 && V_DC > 3){//����ֵ��9����R4��·
					lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
					lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
				}
				else if(V_DC < 3400 && V_DC > 3000){//����ֵ��3338����R1��
//					getRin();
//					if(R_in < 1.5 && R_in > 0.5){//////}

					lcdDispStringBig(560, 363, NAVY, WHITE, "R1");
					lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
				}
				else if(V_DC < 3700 && V_DC > 3400){//����ֵ��3589����R3��
//					getRin();
//					if(R_in > 2.2 && R_in < 3){//////}

					lcdDispStringBig(560, 363, NAVY, WHITE, "R3");
					lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");

				}
				else if(V_DC < 950 && V_DC > 850){//����ֵ��903����R1��|R2��|R4��
					err_rin();
					printf("  R_in = %f\n",R_in);
				    if(R_in < 0.3 && R_in > 0.01){//����ֵ:0.111~0.129����R2��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
					}
				    else if(R_in < 20 && R_in > 16.5){//����ֵ��15.5~15.9|18~22����R1��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R1");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
				    else if(R_in < 14 && R_in > 10.5){//����ֵ��10.5~11.9|11.682~12.918����R4��·//����׿��ѧ�����ϴ�-0.8
						lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }

/*
					getRin();
					lcdRectClear(383, 269, 455, 282, BACKGROUND);
					lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);//////////////////////////////////////////////

				    if(R_in < 0.25 && R_in > 0.05){//����ֵ:0.111~0.129����R2��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R2");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Short ");
					}
				    else if(R_in < 25 && R_in > 13.5){//����ֵ��15.5~15.9|18~22����R1��·
						lcdDispStringBig(560, 363, NAVY, WHITE, "R1");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
				    else if(R_in < 13.5 && R_in > 9){//����ֵ��10.5~11.9|11.682~12.918����R4��·//����׿��ѧ�����ϴ�-0.8
						lcdDispStringBig(560, 363, NAVY, WHITE, "R4");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				    }
*/
				}
			}

			else{//���ݴ����ж�
				printf("---------------C------------------\n");
				FRE_word = ((5000000)*pow(2,32))/400000000;
				IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
				Times = 10000;
				IOWR(TIMES_BASE,0,Times);
/*
				fre_control = 100;
				KW_word = (fre_control*pow(2,32))/400000000;
				IOWR(FRE_OUT_KW_BASE,0,KW_word);
				usleep(100000);

				err_rin();
				printf("R_in = %f\n",R_in);
				usleep(100000);

				printf("//////////////fre = 100k//////////////////\n");
				fre_control = 100000;//100k
				KW_word = (fre_control*pow(2,32))/400000000;
				IOWR(FRE_OUT_KW_BASE,0,KW_word);
				usleep(100000);

				err_gain();
				printf("Gain = %f\n",Gain);
				usleep(100000);

				printf("************* fre = 100Hz**************\n");
				fre_control = 100;
				KW_word = (fre_control*pow(2,32))/400000000;
				IOWR(FRE_OUT_KW_BASE,0,KW_word);
				usleep(100000);
				err_gain();
				printf("R_in = %f\n",R_in);
				printf("Gain = %f\n",Gain);
				Gain = 0;
*/
				/*
				 * C1��-R_in=��|C1˫-��Ƶ����£�R_in����
				 * C2��-G=2.41|C2˫-��Ƶ����£�G����
				 * C3��-��Ƶ����£�G����|C3˫-��Ƶ����£�G��С
				 */

				//1kHz
				printf("////////////// fre = 1k //////////////////\n");
				fre_control = 1000;
				KW_word = (fre_control*pow(2,32))/400000000;
				IOWR(FRE_OUT_KW_BASE,0,KW_word);
				usleep(80000);
				err_rin();
				printf("  R_in = %f\n",R_in);

				if(R_in < 0){//����ֵ���ޡ���C1��--��������֮��Ϊ��ֵ--��
					lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(R_in < 0.35 && R_in > 0.00001){//����ֵ��8.202~8.432|7.037~7.905||0.4~0.7����C2��
					lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
					lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
				}
				else if(R_in < 0.8 && R_in > 0.4){
					lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
					lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
				}
				else{
					printf("~~~~~~~~~~~~~~~ fre = 100k ~~~~~~~~~~~~~~~~\n");
					//��Ƶ�����C3��|C3˫
					fre_control = 100000;//100k
					KW_word = (fre_control*pow(2,32))/400000000;
					IOWR(FRE_OUT_KW_BASE,0,KW_word);

					err_gain();
					printf("Gain = %f\n",Gain);

					if(Gain > 162 && Gain < 169){//����ֵ��181.084~182.014|149.307~150.552-C3��
						lcdDispStringBig(560, 363, NAVY, WHITE, "C3");
						lcdDispStringBig(663, 363, NAVY, WHITE, " Open ");
					}
					else if(Gain > 90 && Gain < 100){//����ֵ��122.786~124.247|85.996~86.396-C3˫��
						lcdDispStringBig(560, 363, NAVY, WHITE, "C3");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
					}
					else if(Gain < 142 && Gain > 138){
						lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
						lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
					}
					else{
						lcdRectClear(560, 363, 590, 395, BACKGROUND);
						lcdRectClear(663, 363, 740, 395, BACKGROUND);
					}
/*
					else{
						//��Ƶ�����C1˫|C2˫
						printf("************* fre = 100Hz**************\n");
						fre_control = 100;
						KW_word = (fre_control*pow(2,32))/400000000;
						IOWR(FRE_OUT_KW_BASE,0,KW_word);
						err_rin();
						printf("R_in = %f\n",R_in);

						if(R_in > 2 && R_in < 5){//����ֵ��4.182~4.648|4.195~4.270-C2˫�� || ���棺173~174.486-VS-162~165
							lcdDispStringBig(560, 363, NAVY, WHITE, "C2");
							lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
						}
						else{
							err_gain();

							if((Gain > 40.2 && Gain < 45) || (R_in < 6.320 && R_in > 5.8)){//����ֵ��40.380~40.691-C1˫��||-VS-40.002~40.137
								lcdDispStringBig(560, 363, NAVY, WHITE, "C1");
								lcdDispStringBig(663, 363, NAVY, WHITE, "Double");
								usleep(1000000);
							}
							else{
								//�������
								lcdRectClear(560, 363, 590, 390, WHITE);
								lcdDispStringBig(663, 363, NAVY, WHITE, "Normal");
							}
						}
					}
					*/
				}
			}

 			printf("-----A new task!!!-------\n\n\n\n");
		}

		else if(ifVDC){
			if(switch_num != 6){
				switch_num = 6;
				IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0010-->0110
				usleep(Switchsleep);
			}
			double V_DC = getVdc(IORD(DOUT_BASE, 0));	//��λ��mV
			lcdRectClear(525, 310, 625, 330, BACKGROUND);
			lcdDispFloatSmall(525, 310, NAVY, WHITE, V_DC);
			usleep(1000000);
		}
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
	lcdDispDecSmall(129, 425, BLACK, WHITE, 500000);
	lcdDispDecSmall(129, 385, BLACK, WHITE, 500);


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

/********************************************IO���ֳ�ʼ��********************************************************/

void io_init(){
	fre_control = 1000;
	getKw(fre_control);
	switch_num = 4;
	IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0000-->0100
	usleep(Switchsleep);
}

/********************************************��־λ��ʼ��*********************************************************/

void flag_init(){
	Troubleshooting = 0;
	end_flag = 0;
	draw_flag = 0;
	draw_end = 1;
	Range_w = 0;//�޸�F_L
	mode = 0;//����ģʽ
	lcdDispStringSmall(60, 344, RED, WHITE, "Measuring");
}

/*******************************************ɨƵ�źŲ�������*******************************************************/
int Sweep(int Rang_down,int Rang_up){
	int freq_sweep = Rang_down;
	IOWR(SWITCH_4WIRE_BASE,0,1);//�̵���0101-0001
    usleep(100000);
	FRE_word = ((5000000)*pow(2,32))/400000000;
	IOWR(SAMP_CLK_KW_BASE,0,FRE_word);
	Times = 10000;
	IOWR(TIMES_BASE,0,Times);

    while(!end_flag){
    	IOWR(SWITCH_4WIRE_BASE,0,1);//�̵���0101-0001
    	if(Troubleshooting)
    		end_flag = 1;
//    	freq_sweep = (freq_sweep+1000)%500000;

		if(freq_sweep <= 3000 && freq_sweep > 450)		//500~3k:100-25
		{
			freq_sweep = freq_sweep+100;
      	}
        else if(freq_sweep <= 10000 && freq_sweep > 3000)	//3k~10k:500-14
        {
           	freq_sweep = freq_sweep+500;
        }
        else if(freq_sweep <= 100000 && freq_sweep > 10000)//10k~100k:1000-90
        {
			freq_sweep = freq_sweep+1000;
		}
		else if(freq_sweep <= 200000 && freq_sweep>100000)//100k~200k:2000-50
		{
			freq_sweep = freq_sweep+2000;
		}
		else if(freq_sweep <= Rang_up && freq_sweep > 200000)	 //100k~����/500k:20k-15
		{
			freq_sweep = freq_sweep + 20000;
		}
		else if(freq_sweep > Rang_up)
		{
			freq_sweep = Rang_down;
		}

    	//ɨƵ�ź�Ƶ�ʿ�����
    	KW_word = (freq_sweep*pow(2,32))/400000000;
    	IOWR(FRE_OUT_KW_BASE,0,KW_word);

		Paint_V_f(freq_sweep, Rang_down, Rang_up);
    }
    return 0;
}

/*
int Sweep(int Rang_down,int Rang_up){
	float freq_sweep = Rang_down;
	if(switch_num != 1){
		switch_num = 1;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0101-0001
		usleep(Switchsleep);
	}

    while(!end_flag){
    	if(Troubleshooting)
    		end_flag = 1;
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

/******************************************R_in&R_out��������*********************************************************/

void getRin(){
	double U_B_min = 1000000.0,U_B_max = -100.0;
	if(switch_num != 4){//�̽�
//		IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0000
		switch_num = 4;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0000-->0100
		usleep(Switchsleep);
	}
	while(!IORD(VPP_FOUND_BASE,0));
	U_B = getVpp(IORD(VPP_BASE,0));
	U_B_total = U_B_total + U_B;
	U_B_store[Round_U1] = U_B;

	double U_A_min = 1000000.0,U_A_max = -100.0;
	if(switch_num != 5){//����
//		IOWR(SWITCH_4WIRE_BASE,0,1);//�̵���0001
		switch_num = 5;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0001-->0101
		usleep(Switchsleep);
	}
	while(!IORD(VPP_FOUND_BASE,0));
	U_A = getVpp(IORD(VPP_BASE,0));
	U_A_total = U_A_total + U_A;
	U_A_store[Round_U1] = U_A;

	Round_U1++;
	if(Round_U1 == (Aver_time+2)){
		for(int i = 1; i < (Aver_time+2); i++){
			if(U_B_store[i] < U_B_min){
				U_B_min = U_A_store[i];
			}
			else if(U_B_store[i] > U_B_max){
				U_B_max = U_B_store[i];
			}
		}
		U_B_total = (U_B_total-U_B_max-U_B_min);

		for(int i = 1; i < (Aver_time+2); i++){
			if(U_A_store[i] < U_A_min){
				U_A_min = U_A_store[i];
			}
			else if(U_A_store[i] > U_A_max){
				U_A_max = U_A_store[i];
			}
		}
		U_A_total = (U_A_total-U_A_max-U_A_min);
		double R_in_and = 0;
		if(fabs(U_A_total-U_B_total)>0){
//			printf("(U_A_total) = %f\n",fabs(U_A_total));
			R_in_and = (U_A_total/fabs(U_A_total-U_B_total))*Res1;
			R_in = 47*R_in_and/(47-R_in_and);
		}
		U_0_total = U_A_total;U_r_total = U_B_total;
//		lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);
		Round_U1 = 0;U_A_total = 0;U_B_total = 0;
	}
}

void getRout(){
	double U_1_min = 1000000.0,U_1_max = -100.0;
	if(switch_num != 0){
//		IOWR(SWITCH_4WIRE_BASE,0,4); //�̵���0100--0110
		switch_num = 0;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0100--0000
		usleep(Switchsleep);
	}
	while(!IORD(VPP_FOUND_BASE,0));
	double rdn = getVpp(IORD(VPP_BASE,0));
//	lcdRectClear(400, 100, 500, 120, BACKGROUND);
//	lcdDispFloatSmall(400, 100, BLACK, WHITE, rdn);
//	U_1 = rdn-400;
	if(rdn > 2500){
		U_1 = getVpp(IORD(VPP_BASE,0))-300;
	}
	else{
		if(getVpp(IORD(VPP_BASE,0)) > 500){
			U_1 = getVpp(IORD(VPP_BASE,0))-400;
		}
	}

	U_1_total = U_1_total + U_1;
	U_1_store[Round_U2] = U_1;

	double U_2_min = 1000000.0,U_2_max = -100.0;
	if(switch_num != 8){
//		IOWR(SWITCH_4WIRE_BASE,0,12);//�̵���1100
		switch_num = 8;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���1100-1000
		usleep(Switchsleep);
	}
	while(!IORD(VPP_FOUND_BASE,0));
	if(getVpp(IORD(VPP_BASE,0)) > 500){
		U_2 = getVpp(IORD(VPP_BASE,0))-400;
	}
	else{
		U_2 = getVpp(IORD(VPP_BASE,0));
	}
	U_2_total = U_2_total + U_2;
	U_2_store[Round_U2] = U_2;
//	printf("U_1=%f,U_2=%f\n",U_1,U_2);
	Round_U2++;
	if(Round_U2 == (Aver_time+2)){
		for(int i = 1; i < (Aver_time+2); i++){
			if(U_1_store[i] < U_1_min){
				U_1_min = U_1_store[i];
			}
			else if(U_1_store[i] > U_1_max){
				U_1_max = U_1_store[i];
			}
		}
		U_1_total = (U_1_total-U_1_max-U_1_min);

		for(int i = 1; i < (Aver_time+2); i++){
			if(U_2_store[i] < U_2_min){
				U_2_min = U_2_store[i];
			}
			else if(U_2_store[i] > U_2_max){
				U_2_max = U_2_store[i];
			}
		}
		U_2_total = (U_2_total-U_2_max-U_2_min);

		R_out = (fabs((U_1_total-U_2_total)/U_2_total))*Res2;
		if(R_out > 1){
			R_out = R_out-0.75;
		}
		U_inf_total = U_1_total;
		lcdRectClear(383, 319, 455, 332, BACKGROUND);
		lcdDispFloatSmall(383, 319, BLACK, WHITE, R_out);
		Round_U2 = 0;U_1_total = 0;U_2_total = 0;

		//Gain����
		if(fabs(U_0_total-U_r_total)==0 && U_inf_total > 10){
			Gain = 1 + U_inf_total/(1000*1000*Aver_time);
		}
		else{
			Gain=(U_inf_total/fabs(U_0_total-U_r_total))*Av;
		}
		lcdRectClear(383, 369, 455, 382, BACKGROUND);
		lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);

//		if(fabs(U_0_total-U_r_total)>0){
//			Gain=(U_inf_total/fabs(U_0_total-U_r_total))*Av;
//			lcdRectClear(383, 369, 455, 382, BACKGROUND);
//			lcdDispFloatSmall(383, 369, BLACK, WHITE, Gain);
//		}
	}
}

/****************************************Ƶ�ʿ����ֻ�ú���********************************************************/

void getKw(double fre){
	//ɨƵ�ź�Ƶ�ʿ�����
	KW_word = (fre*pow(2,32))/400000000;
	IOWR(FRE_OUT_KW_BASE,0,KW_word);
	//ADS805����ʱ��Ƶ�ʿ�����
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

/****************************************��Ƶ�������߻��ƺ���*******************************************************/

void Paint_V_f(float fre_sweep,int Range_down,int Range_up){
	while(!IORD(VPP_FOUND_BASE,0));//�̵���0101
	double addata_in = getVpp(IORD(VPP_BASE,0));

	//Ѱ�ҽ�ֹƵ�ʵ�+�洢����׼����ͼ
	if(draw_end){
		Vpp_pf[rdaddress] = addata_in;
		fre_trans_pf[rdaddress] = 262+500*(fre_sweep-Range_down)/(Range_up-Range_down);
		fre_pf[rdaddress] = fre_sweep;
		rdaddress++;

		if(rdaddress == rda_end){
			storge_end = 1;
			rdaddress = 0;

			for(int i = 1; i < rda_end - 1; i++){
				if((Vpp_pf[i-1] < Vpp_pf[i])&&(Vpp_pf[i+1] < Vpp_pf[i])){
					Vpp_pf[i] = (Vpp_pf[i-1]+Vpp_pf[i+1])/2;
				}
				else if((Vpp_pf[i-1] > Vpp_pf[i])&&(Vpp_pf[i+1] > Vpp_pf[i])){
					Vpp_pf[i] = (Vpp_pf[i-1]+Vpp_pf[i+1])/2;
				}
			}
			for(int i = 0; i < rda_end; i++){
				if(Vpp_pf[i] > max_of_Vpp){
					max_of_Vpp = Vpp_pf[i];
				}
			}
			int j = 0;
			for(int i = 1; i < rda_end - 1; i++){
				if((Vpp_pf[i-1] > max_of_Vpp*0.7) && (Vpp_pf[i+1] <= max_of_Vpp*0.7)){
					fre_cut[j] = fre_pf[i];j++;
				}
				int fre_cut_trans;

				if(fre_cut[0]>30000&&fre_cut[0]<100000){
	//				fre_cut_trans = 4.5*fre_cut[0]-145000;
					fre_cut_trans = 3.6*fre_cut[0]-110800;
				}
				else{
					fre_cut_trans = fre_cut[0];
				}

//				fre_cut_trans = fre_cut[0];
				lcdRectClear(383, 419, 455, 432, BACKGROUND);
				lcdDispDecSmall(383, 419, BLACK, WHITE, fre_cut_trans);
				fre_cut_trans = 0;
			}
			printf("Store over!\n");
		}
	}

	if(storge_end){
		printf("Begin drawing V-f!\n");
		storge_end = 0;
		draw_end = 0;
		lcdRectClear(260, 40, 764, 230, BACKGROUND);
		lcdDrawGrid(260, 50, 10, 28, 18, LGRAY);
		for(int i = 0; i < rda_end-1; i++){
			// Vpp_draw[i] = 230-180*Vpp_pf[i]/max_of_Vpp--y
			// fre_draw[i] = 764-504*fre_trans_pf[i]/fre_range--x--ԭֵ
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


/*
void Paint_V_f(float fre_sweep,int Range_down,int Range_up){
	while(!IORD(VPP_FOUND_BASE,0));//�̵���0101-->0001
	double addata_in = getVpp(IORD(VPP_BASE,0));

	//Ѱ�ҽ�ֹƵ�ʵ�+�洢����׼����ͼ
	if(draw_end){
		Vpp_pf[rdaddress+1] = addata_in;
//		fre_trans_pf[rdaddress] = 126*log10(fre_sweep)+8;//�Ǿ�������--100~1M
		fre_trans_pf[rdaddress] = 100.8*log10(fre_sweep)+58.4;//�Ǿ�������--100~10M
		fre_pf[rdaddress] = fre_sweep;
		rdaddress++;

		if(rdaddress == rda_end){
			storge_end = 1;
			rdaddress = 0;
			for(int i = 1; i < rda_end - 1; i++){
				if(fre_pf[i] > 10000){
//
//					if(Vpp_pf[i-1] < Vpp_pf[i]){
//						Vpp_pf[i] = (Vpp_pf[i-1]+Vpp_pf[i])/2;
//					}
//
					if(Vpp_pf[i-1] < Vpp_pf[i]){
						Vpp_pf[i] = Vpp_pf[i-1];
					}
				}
			}
			for(int i = 0; i < rda_end; i++){
				if(Vpp_pf[i] > max_of_Vpp){
					max_of_Vpp = Vpp_pf[i];
				}
			}
			for(int i = 1; i < rda_end - 1; i++){
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
			// fre_draw[i] = 764-504*fre_trans_pf[i]/fre_range--x--ԭֵ
			if(Vpp_pf[i+1] >= 0 && (fre_pf[i] <= fre_pf[i+1])){
				lcdDrawLine(fre_trans_pf[i], 230-180*(Vpp_pf[i]/max_of_Vpp), fre_trans_pf[i+1], 230-180*(Vpp_pf[i+1]/max_of_Vpp), BLACK);
			}
		}
		printf("Finish drawing!\n");
		draw_end = 1;
		end_flag = 1;
		max_of_Vpp = -100;
	}
}

/*************************************��ADS805��ȡ����ֵת��Ϊ��ʵ��ѹֵ-��λ��V****************************************************/

double getVpp(double vrd){
	double vin;
	vin = vrd * 1.312-1.323;
	if(vin <= 0){
		vin = vrd;
	}
//	vin = vin/1000;
	return vin;
}

/****************************************��ADS1118�ɵ���ֵ��ɵ�ѹ/mV*******************************************************/

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

/**********************************************���ӹ��ܲ��ֲ��Գ���********************************************************/
/*----------------------------------------���㲢��ʾR_in|R_out|Gain|V_DC----------------------------------------------*/
void test(){
	fre_control = 1000;
	getKw(fre_control);

	//R_in����
	getRin();//�̵���0000-0001-->0100-0101
	lcdRectClear(383, 269, 455, 282, BACKGROUND);
	lcdDispFloatSmall(383, 269, BLACK, WHITE, R_in);

	//R_out����
	getRout();

	if(switch_num != 6){
		switch_num = 6;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0010-->0110
		usleep(Switchsleep);
	}
	double V_DC = getVdc(IORD(DOUT_BASE, 0));	//��λ��mV
	lcdRectClear(525, 310, 625, 330, BACKGROUND);
	lcdDispFloatSmall(525, 310, NAVY, WHITE, V_DC);
}

/*******************************************������ϲ��ִ���Gain&Rin**********************************************************/

void err_rin(){
	double U_B_2 = 0,U_A_2 = 0;
	if(switch_num != 4){
		switch_num = 4;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0000-->0100
		usleep(100000);
	}
	for(int i = 0; i < 10; i++){
		while(!U_B){
			while(!IORD(VPP_FOUND_BASE,0));
			U_B = getVpp(IORD(VPP_BASE,0));
		}
		U_B_2 = U_B+U_B_2;
		U_B = 0;
	}

	if(switch_num != 5){//����
		switch_num = 5;
		IOWR(SWITCH_4WIRE_BASE,0,switch_num);//�̵���0001-->0101
		usleep(100000);
	}
	for(int i = 0;i < 10;i++){
		while(!U_A){
			while(!IORD(VPP_FOUND_BASE,0));
			U_A = getVpp(IORD(VPP_BASE,0));
		}
		U_A_2 = U_A_2+U_A;
		U_A = 0;
	}

	double R_in_and = 0;
	if(fabs(U_A_2-U_B_2)>0){
//		printf("U_A = %f, U_B = %f\n",U_A,U_B);
		R_in_and = (U_A_2/fabs(U_A_2-U_B_2))*Res1;
		printf("R_in_and = %f",R_in_and);
		R_in = 47*R_in_and/(47-R_in_and);
	}
}

void err_gain(){
	double U_B_2 = 0,U_A_2 = 0,rdn_2 = 0;
	IOWR(SWITCH_4WIRE_BASE,0,4);//�̵���0000-->0100
	usleep(100000);
	for(int i = 0; i < 10; i++){
		while(!U_B){
			while(!IORD(VPP_FOUND_BASE,0));
			U_B = getVpp(IORD(VPP_BASE,0));
		}
		U_B_2 = U_B+U_B_2;
		U_B = 0;
	}

	IOWR(SWITCH_4WIRE_BASE,0,5);//�̵���0001-->0101
	usleep(100000);
	for(int i = 0; i < 10; i++){
		while(!U_A){
			while(!IORD(VPP_FOUND_BASE,0));
			U_A = getVpp(IORD(VPP_BASE,0));
		}
		U_A_2 = U_A+U_A_2;
		U_A = 0;
	}
	U_0 = U_A_2;U_r = U_B_2;
	double R_in_and = 0;
	if(fabs(U_A_2-U_B_2)>0){
//		printf("U_A = %f, U_B = %f\n",U_A,U_B);
		R_in_and = (U_A_2/fabs(U_A_2-U_B_2))*Res1;
		R_in = 47*R_in_and/(47-R_in_and);
	}

	//-----U1
	IOWR(SWITCH_4WIRE_BASE,0,0);//�̵���0100--0000
	usleep(100000);
	double rdn = 0;
	for(int i = 0; i < 10; i++){
		while(!rdn){
			while(!IORD(VPP_FOUND_BASE,0));
			rdn = getVpp(IORD(VPP_BASE,0));
		}
		if(rdn > 2500){
			rdn = rdn-300;
		}
		else if(rdn < 2500 && rdn > 500){
			rdn = rdn-400;
		}
		rdn_2 = rdn+rdn_2;
		rdn = 0;
	}

	U_inf = rdn_2;
	Gain=(U_inf/fabs(U_0-U_r))*Av;
}
