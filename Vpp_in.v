module Vpp_in(
 input  clk_sample,
 input  [11:0] Datain,
 input  [31:0] Times,	//自定义上限次数；＜100k：200次；＞100k：~~
 output [11:0] Max,
 output [11:0] Min,
 output [11:0] Vpp,
 output reg Vpp_found	//是否找到Vpp？找到为1，没找到为0.
);

reg [11:0] Max_reg;
reg [11:0] Min_reg;
reg [31:0] cnt;
reg [11:0] Max_reg_buff;
reg [11:0] Min_reg_buff;
reg [31:0] T_old;

always @(posedge clk_sample)
begin
	if(cnt < Times)			//小于计数次数上限，逐个数据比较
	begin
		if(Datain > Max_reg)
			Max_reg <= Datain;
		if(Datain < Min_reg)
			Min_reg <= Datain;
		cnt <= cnt + 1'b1;
		Vpp_found <= 1'b0;
	end
	
	else if(cnt >= Times)
	begin
		Max_reg_buff <= Max_reg;
		Min_reg_buff <= Min_reg;
		Vpp_found <= 1'b1;
		cnt <= 1'b0;
		Max_reg <= 1'b0;
		Min_reg <= 12'hfff;
	end

	if(Times != T_old)
	begin
		cnt <= 1'b0;
		Max_reg <= 1'b0;
		Min_reg <= 12'hfff;
	end
	T_old <= Times;

end

assign Max = Max_reg_buff;
assign Min = Min_reg_buff;
assign Vpp = Max - Min;
endmodule


