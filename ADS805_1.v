module ADS805_1	//是否需要输出adc_clk？原是将adc_clk作为dac的输入
(
	input clk_sample,
	input [11:0] data_in,	
	output adc_clk,
	output [11:0] data_out
);

reg [11:0] adc_in;
reg [11:0] adc_delay1;
reg [11:0] adc_delay2;

always @(posedge clk_sample)
begin
	adc_delay1 <= data_in;
	adc_delay2 <= adc_delay1;
	adc_in <= adc_delay2;
end

assign data_out = adc_in;
assign adc_clk= (!clk_sample);

endmodule
