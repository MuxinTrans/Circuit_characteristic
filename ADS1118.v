module ADS1118(
input wire CLK_50M,
input wire rst_n,
input wire DOUT,				 //ADS1118 16位数据输出
output wire sclk,
output reg DIN,				 //用于寄存器配置
output reg CS,				 //低电平有效
output reg [15:0] data_out
);

wire clk_s;

pll_1M	CLK_1M (
	.inclk0 ( CLK_50M ),
	.c0 ( clk_s )
	);

reg [15:0] cnt;

reg [15:0] rddata;
reg [15:0] datao_2;
reg [15:0] rddata_reg;
reg start;				//开始传输数据标志位
reg done;				//数据传输完成标志位
reg cke;					//SCLK有效标志位

wire [15:0] config_data;
reg  [15:0] config_data_reg;
assign config_data = 16'b1_100_001_1_111_0_1_01_0;		//AIN0输入

reg [4:0] data_cnt;

always @(posedge clk_s or negedge rst_n)
begin
if(!rst_n)
begin
	cnt <= 0;
end
else if(cnt <= 50)
		begin
			cnt <= cnt + 1'd1;
			CS <= 1'b1;
		end
	else if(cnt > 50 && cnt < 70)
		begin
			CS <= 1'b0;
			start <= 1;
			cnt <= cnt + 1'd1;
		end
	else if(cnt >= 70)
		begin
			cnt <= 0;
			start <= 0;
		end
end

/////////////////////////信号读取/////////////////////////////////////

always @(posedge CLK_50M or negedge rst_n)
begin
	if(!rst_n)
	begin
		datao_2 <= 0;
		data_out <= 0;
	end
	else
	begin
		datao_2 <= rddata;
		data_out <= datao_2;
	end
end

always @(posedge clk_s or negedge rst_n)
begin
	if(!rst_n)
	begin
		rddata_reg <= 0;
		done <= 0;
		cke <= 0;
	end
	else
	begin
		case(data_cnt)
			5'd0:
			begin
				cke <= 1'b0;
				done <= 1'b0;
			end
			5'd1: cke <= 1'b1;
			5'd2: rddata_reg[15] <= DOUT;
			5'd3: rddata_reg[14] <= DOUT;
			5'd4: rddata_reg[13] <= DOUT;
			5'd5: rddata_reg[12] <= DOUT;
			5'd6: rddata_reg[11] <= DOUT;
			5'd7: rddata_reg[10] <= DOUT;
			5'd8: rddata_reg[9]  <= DOUT;
			5'd9: rddata_reg[8]  <= DOUT;
			5'd10:rddata_reg[7]  <= DOUT;
			5'd11:rddata_reg[6]  <= DOUT;
			5'd12:rddata_reg[5]  <= DOUT;
			5'd13:rddata_reg[4]  <= DOUT;
			5'd14:rddata_reg[3]  <= DOUT;
			5'd15:rddata_reg[2]  <= DOUT;
			5'd16:rddata_reg[1]  <= DOUT;
			5'd17:
			begin
				rddata_reg[0] <= DOUT;
				cke <= 1'b0;
			end
			5'd18:
			begin
				done <= 1'b1;
				rddata <= rddata_reg;
			end
			default:;
		endcase
	end
end

assign sclk = clk_s & cke;

///////////////////////寄存器配置读取//////////////////////////////////

always @(posedge clk_s or negedge rst_n)
begin
	if(!rst_n)
		data_cnt <= 0;
	else if(!start)
		data_cnt <= 0;
	else if(data_cnt < 5'd23)
		data_cnt <= data_cnt + 1'b1;
end

always @(posedge clk_s or negedge rst_n)
begin
	if(!rst_n)
	begin
		config_data_reg <= 16'd0;
		DIN <= 1'd0;
	end
	else
	begin
		case(data_cnt)
			5'd0:
			begin
				config_data_reg <= config_data;
				DIN <= 1'b0;
			end
			5'd1: DIN <= config_data_reg[15];
			5'd2: DIN <= config_data_reg[14];
			5'd3: DIN <= config_data_reg[13];
			5'd4: DIN <= config_data_reg[12];
			5'd5: DIN <= config_data_reg[11];
			5'd6: DIN <= config_data_reg[10];
			5'd7: DIN <= config_data_reg[9];
			5'd8: DIN <= config_data_reg[8];
			5'd9: DIN <= config_data_reg[7];
			5'd10:DIN <= config_data_reg[6];
			5'd11:DIN <= config_data_reg[5];
			5'd12:DIN <= config_data_reg[4];
			5'd13:DIN <= config_data_reg[3];
			5'd14:DIN <= config_data_reg[2];
			5'd15:DIN <= config_data_reg[1];
			5'd16:DIN <= config_data_reg[0];
			default:DIN <= 1'b0;
		endcase
	end
end


endmodule
