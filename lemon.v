/*
	usage: top entity
	description: lemon is an excellent name
 */
 
module lemon
(
	// clock and reset
	input 			CLK_50M,			
	input				CLK_IN,	     
	output 			CLK_OUT,
	input 			RST_N,	     
	
	// toggle switch
	input  [1:0]	SW,      

	// led
	output [3:0]	LED,   

	// sdram interface
	inout  [15:0]	DRAM_DQ,
	output [11:0]	DRAM_ADDR,
	output [1:0]	DRAM_DQM,
	output 			DRAM_WE_N, 
	output 			DRAM_CAS_N,
	output 			DRAM_RAS_N,
	output 			DRAM_CS_N,
	output [1:0]	DRAM_BA,
	output 			DRAM_CLK,
	output 			DRAM_CKE,
	
	// flash interface
	inout  [7:0]  	FLASH_DATA,
	output [21:0] 	FLASH_ADDR,
	output			FLASH_WR_N,
	output			FLASH_SEL_N,
	output			FLASH_RD_N,
	output 			Flash_WP,
	output 			Flash_RST_N,
	output 			Flash_BYTE,

	// lcd interface
	inout [15:0]	LCD_DATA,
	output 			LCD_RS,
	output 			LCD_RST_N,
	output 			LCD_CS_N,
	output 			LCD_RD_N,
	output 			LCD_WR_N,

	// touch screen interface
	input 			TOUCH_INT,
	output 			TOUCH_RST_N,
	inout 			TOUCH_SDA,
	output			TOUCH_SCL,
	
	// on board key interface
	input [3:0]		KEY,
	
	// adapter board key interface
	input 			SW1,
	input				SW2,
	input				SW_CODE,
	input				SW_CODE_L,
	input				SW_CODE_R,
	
	/* 
	 * gpio interface
	 * for gpio_pa is on the inside
	 * for gpio_pb is on the outside
	 * with io[0] is near GND, with io[15] is near VCC
	 */
	inout [15:0]	GPIO_PA,    
	inout [15:0]	GPIO_PB,
	inout [15:0]	GPIO_PC,
	inout [15:0]	GPIO_PD
	);
	
	// nios2 instance
	nios2 nios2_inst
	(
		.clk_clk(CLK_100M) ,					// input  clk_clk_sig
		.reset_reset_n(RST_N) ,				// input  reset_reset_n_sig
		
		.flash_generic_tristate_controller_tcm_write_n_out(FLASH_WR_N) ,			// output [0:0] flash_generic_tristate_controller_tcm_write_n_out_sig
		.flash_generic_tristate_controller_tcm_chipselect_n_out(FLASH_SEL_N) ,	// output [0:0] flash_generic_tristate_controller_tcm_chipselect_n_out_sig
		.flash_generic_tristate_controller_tcm_read_n_out(FLASH_RD_N) ,			// output [0:0] flash_generic_tristate_controller_tcm_read_n_out_sig
		.flash_generic_tristate_controller_tcm_data_out(FLASH_DATA) ,				// inout [7:0] flash_generic_tristate_controller_tcm_data_out_sig
		.flash_generic_tristate_controller_tcm_address_out(FLASH_ADDR) ,			// output [21:0] flash_generic_tristate_controller_tcm_address_out_sig
		
		.keyirq_export(keyirq) ,			// input [2:0] keyirq_export_sig
		.keyvalue_export(keyvalue) ,		// input [2:0] keyvalue_export_sig
		
		.lcd_lcd_cs_n(LCD_CS_N) ,			// output  lcd_lcd_cs_n_sig
		.lcd_lcd_wr_n(LCD_WR_N) ,			// output  lcd_lcd_wr_n_sig
		.lcd_lcd_rd_n(LCD_RD_N) ,			// output  lcd_lcd_rd_n_sig
		.lcd_lcd_rs(LCD_RS) ,				// output  lcd_lcd_rs_sig
		.lcd_lcd_data(LCD_DATA) ,			// inout [15:0] lcd_lcd_data_sig

		.sdram_addr(DRAM_ADDR) ,			// output [11:0] sdram_addr_sig
		.sdram_ba(DRAM_BA) ,					// output [1:0] sdram_ba_sig
		.sdram_cas_n(DRAM_CAS_N) ,			// output  sdram_cas_n_sig
		.sdram_cke(DRAM_CKE) ,				// output  sdram_cke_sig
		.sdram_cs_n(DRAM_CS_N) ,			// output  sdram_cs_n_sig
		.sdram_dq(DRAM_DQ) ,					// inout [15:0] sdram_dq_sig
		.sdram_dqm(DRAM_DQM) ,				// output [1:0] sdram_dqm_sig
		.sdram_ras_n(DRAM_RAS_N) ,			// output  sdram_ras_n_sig
		.sdram_we_n(DRAM_WE_N) ,			// output  sdram_we_n_sig
		
		.touch_irq_export(touchirq) ,		// input  touch_irq_export_sig
		.touch_scl_export(TOUCH_SCL) ,	// output  touch_scl_export_sig
		.touch_sda_export(TOUCH_SDA) 	,	// inout  touch_sda_export_sig
		
		.switch_4wire_export(switch) ,	// output [3:0] switch_4wire_export_sig
		.fre_out_kw_export(kw) ,			// output [31:0] fre_out_kw_export_sig
		.samp_clk_kw_export(kw2) ,			// output [31:0] samp_clk_kw_export_sig
		.times_export(T) ,					// output [31:0] times_export_sig
		.dout_export(tro_out) ,				// input [15:0] dout_export_sig
		.vpp_export(Vpp) ,					// input [11:0] vpp_export_sig
		.vpp_found_export(Vpp_found) 		// input  vpp_found_export_sig

	);
	
	////////////////////////////////system////////////////////////////////////////////
	
	wire [3:0]keyvalue;
	wire [3:0]keyirq;
	
	key key_inst(
		.clk(CLK_100M) ,			// input  clk_sig
		.sw_code(SW_CODE) ,		// input  sw_code_sig
		.sw_code_l(SW_CODE_L) ,	// input  sw_code_l_sig
		.sw_code_r(SW_CODE_R) ,	// input  sw_code_r_sig
		.sw1(SW1) ,					// input  sw1_sig
		.sw2(SW2) ,					// input  sw2_sig
		.key(KEY) ,					// input [3:0] key_sig
		.value(keyvalue) ,		// output [2:0] value_sig
		.irq(keyirq) 				// output [2:0] irq_sig
	);
	
	
	wire touchirq;
	
	ctouchint ctouchint_inst(
		.clk(CLK_100M) ,			// input  clk_sig
		.raw(TOUCH_INT) ,			// input  raw_sig
		.irq(touchirq) 			// output  irq_sig
	);
	
	
	assign Flash_WP = 1'b1;
	assign Flash_RST_N = RST_N;
	assign Flash_BYTE = 1'b0;
	assign LCD_RST_N = RST_N;
	assign TOUCH_RST_N = RST_N;

	
	wire CLK_100M;
	wire CLK_1M;
	
	pll pll_inst
	(
		.inclk0(CLK_50M),			// input  inclk0_sig
		.c0(CLK_100M),				// output  c0_sig
		.c1(DRAM_CLK), 			// output  c1_sig
		.c2(CLK_1M)
	);

	assign LED = 4'b1111;

//////////////////////////////////扫频输出：20 ~ 10MHz//////////////////////////////////////////
	
	wire [31:0] kw;		//扫频信号-频率控制字
	wire [13:0] Sin_out; //扫频信号输出
	
	DDS1 Sin_saopin
	(
		.clk(CLK_100M) ,	// input  clk_sig
		.rst_n(RST_N) ,	// input  rst_n_sig
		.KW(kw) ,			// input [31:0] KW_sig
		.Sine(Sin_out) ,	// output [13:0] Sine_sig
		.Last_bit() 		// output  Last_bit_sig
	);
	
	// DAC904
	assign GPIO_PB[14:1] = Sin_out[13:0];	//输出给904
	//assign CLK_OUT = CLK_100M;
	assign CLK_OUT = CLK_sample;
	
////////////////////////////ADS805：读取输入信号（采样时钟）并获得最大值//////////////////////////////
	
	wire [11:0] Data_in;
	wire [11:0] adc_out;
	wire [31:0] kw2;		//805采样时钟-频率控制字
	wire [31:0] T;			//每个周期中正弦波点数（计数次数）
	wire [11:0] Vpp;
	wire CLK_sample;
	wire Vpp_found;
	
	
	assign Data_in = GPIO_PD[13:2];
	//assign GPIO_PA[0] = CLK_sample;	//由于采样频率可变，因此采样时钟单独给805
	
	DDS1 CLKsample		//小于100k实时采样，大于100k等效采样
	(
		.clk(CLK_100M) ,		// input  clk_sig
		.rst_n(RST_N) ,		// input  rst_n_sig
		.KW(kw2) ,				// input [31:0] KW_sig
		.Sine() ,				// output [13:0] Sine_sig
		.Last_bit(CLK_sample)// output  Last_bit_sig
	);
	
	ADS805_1 ADS805_rdata
	(
		.clk_sample(CLK_sample) ,// input  clk_sample_sig
		.data_in(Data_in) ,		 // input [11:0] data_in_sig
		.adc_clk() ,				 // output  adc_clk_sig
		.data_out(adc_out) 		 // output [11:0] data_out_sig
	);

	Vpp_in Vpp_input
	(
		.clk_sample(CLK_sample) ,// input  clk_sample_sig
		.Datain(adc_out) ,		 // input [11:0] Datain_sig
		.Times(T) ,					 // input [31:0] Times_sig
		.Max() ,						 // output [11:0] Max_sig
		.Min() ,						 // output [11:0] Min_sig
		.Vpp(Vpp) ,					 // output [11:0] Vpp_sig
		.Vpp_found(Vpp_found) 	 // output  Vpp_found_sig
	);
	
///////////////////////////////////switch按键控制//////////////////////////////////////////////
	
	wire [3:0] switch;
	reg  [3:0] switch_reg;
	
	always@(posedge CLK_100M)
	begin
		switch_reg <= switch;
	end
	
	assign GPIO_PB[0]  = switch_reg[0];	//switch1-输入出R1:0接1短--1
	assign GPIO_PC[0]  = switch_reg[1];	//switch2-电子开关1:0高1低--4
	assign GPIO_PD[15] = switch_reg[2];	//switch3-电子开关2:0入1出--2
	assign GPIO_PC[1]  = switch_reg[3];	//switch4-输出处R2:0接1断--3
	
/////////////////////////////////////ADS1118///////////////////////////////////////////////

	wire [15:0] tro_out;
	wire Din;
	wire Dout;
	wire CS;
	wire SCLK;
		
	assign GPIO_PA[14] = Din;
	assign Dout = GPIO_PA[13];
	assign GPIO_PA[15]  = SCLK;
	assign GPIO_PB[15]  = CS;
	
	ADS1118 ADS1118_u1
	(
		.CLK_50M(CLK_50M) ,	// input  CLK_50M_sig
		.rst_n(RST_N) ,		// input  rst_n_sig
		.DOUT(Dout) ,			// input  DOUT_sig
		.sclk(SCLK) ,			// output  sclk_sig
		.DIN(Din) ,				// output  DIN_sig
		.CS(CS) ,				// output  CS_sig
		.data_out(tro_out) 	// output [15:0] data_out_sig
	);
	
endmodule 
