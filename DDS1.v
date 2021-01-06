module DDS1
(
input  clk,
input  rst_n,
input  [31:0] KW,
output [13:0] Sine,
output Last_bit
);

reg Last_bit_reg;
reg [31:0] cnt;

always @(posedge clk)
begin 
	cnt <= cnt + KW;
	if(Sine >= 14'd8192)		//2^13 = 8192
		Last_bit_reg <= 1'b1;
	else if(Sine < 14'd8193)
		Last_bit_reg <= 1'b0; 
end 

assign Last_bit = Last_bit_reg;
wire [13:0] addr = cnt[31:18];

Rom_sin	Sin_1 (
	.address ( addr ),
	.clock ( clk ),
	.q ( Sine )
	);

endmodule
