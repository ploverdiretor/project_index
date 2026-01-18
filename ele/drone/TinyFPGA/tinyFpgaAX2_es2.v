module TinyFPGA_A2 (
  output pin1,				//BLDC_for_development
  input pin2,
  inout pin3_sn,
  inout pin4_mosi,
  output pin5,				//BLDC_ch1
  output pin6,				//BLDC_ch2
  inout pin7_done,
  inout pin8_pgmn,
  inout pin9_jtgnb,
  inout pin10_sda,
  inout pin11_scl,
  //inout pin12_tdo,
  //inout pin13_tdi,
  //inout pin14_tck,
  //inout pin15_tms,
  inout pin16,
  inout pin17,
  inout pin18_cs,
  inout pin19_sclk,
  inout pin20_miso,
  output pin21,				//BLDC_ch3
  output pin22				//BLDC_ch4
);

  // left side of board
  //assign pin1 = 1'bz;
  assign pin2 = 1'bz;
  assign pin3_sn = 1'bz;
  assign pin4_mosi = 1'bz;
  //assign pin5 = 1'bz;				//BLDC_ch1
  //assign pin6 = 1'bz;				//BLDC_ch2
  assign pin7_done = 1'bz;
  assign pin8_pgmn = 1'bz;
  assign pin9_jtgnb = 1'bz;
//  assign pin10_sda = 1'bz;
//  assign pin11_scl = 1'bz;
  assign pin10_sda = (sda_oe==1)? 1'b0: 1'bz; //assign pin10_sda = 1'bz;
  assign pin11_scl = (scl_oe==1)? 1'b0: 1'bz; //assign pin11_scl = 1'bz;  
  // right side of board
  //assign pin12_tdo = 1'bz;
  //assign pin13_tdi = 1'bz;
  //assign pin14_tck = 1'bz;
  //assign pin15_tms = 1'bz;
  assign pin16 = 1'bz;
  assign pin17 = 1'bz;
  assign pin18_cs = 1'bz;
  assign pin19_sclk = 1'bz;
  assign pin20_miso = 1'bz;
  //assign pin21 = 1'bz;				//BLDC_ch3
  //assign pin22 = 1'bz;				//BLDC_ch4
  //---------------------
  
//------------------------------------------------------------ clk generator 2.08MHz
  wire clk;
  OSCH #(
    .NOM_FREQ("2.08")
  ) internal_oscillator_inst (
    .STDBY(1'b0), 
    .OSC(clk)
  ); 
//-----------------

//------------------------------------------------------------ I2C_IP connect
	wire scl_oe;
	wire sda_oe;
	wire [7:0] data_out;
  wire start,stop,r_w,data_vld;
  parameter data_in = 8'b00000001;
  parameter ready = 1'b1;
  i2c_slave i2c_slave0(.XRESET(~pin2), .ready(ready), .start(start), .stop(stop), .data_in(data_in), .data_out(data_out),
	.r_w(r_w), .data_vld(data_vld), .scl_in(pin11_scl), .scl_oe(scl_oe), .sda_in(pin10_sda), .sda_oe(sda_oe));
//meta stable countermeasure-----------------
reg tmp_data_vld;
reg cap_data_vld;
reg [7:0] tmp_data_out;
reg [7:0] cap_data_out;
always @(posedge clk or negedge pin2)begin
	if (pin2==1'b0)begin
		tmp_data_vld <= 1'b0;
		tmp_data_out <= 8'b0;
	end else begin
		tmp_data_vld <= data_vld;
		tmp_data_out <= data_out;
	end
end
always @(posedge clk or negedge pin2)begin
	if (pin2==1'b0)begin
		cap_data_vld <= 1'b0;
		cap_data_out <= 8'b0;
	end else begin
		cap_data_vld <= tmp_data_vld;
		cap_data_out <= tmp_data_out;
	end
end
//------------------------------------------------------------
reg [2:0] vld_trig;
reg [2:0] dat_count;
reg [7:0] status;
reg [7:0] pwm_dat00;				//BLDC_ch1
reg [7:0] pwm_dat01;				//BLDC_ch2
reg [7:0] pwm_dat10;				//BLDC_ch3
reg [7:0] pwm_dat11;				//BLDC_ch4
reg pin5;				//BLDC_ch1
reg pin6;				//BLDC_ch2
reg pin21;				//BLDC_ch3
reg pin22;				//BLDC_ch4
reg one_shot;
assign pin1 = (pin2 == 1'b1)? (status[7] && status[6] && status[5] && status[4] && status[3] && status[2] && status[1] && status[0]): ( r_w || data_vld || start || stop);;
//------------------------------------------------------------
parameter min_duty = 32'b0000_1000_0001_0000;
parameter max_duty = 32'b0001_0000_0010_0000;
reg [31:0] base_clk_counter;
reg [31:0] pwm_duty00;				//BLDC_ch1
reg [31:0] pwm_duty01;				//BLDC_ch2
reg [31:0] pwm_duty10;				//BLDC_ch3
reg [31:0] pwm_duty11;				//BLDC_ch4
wire [31:0] temp_duty00;				//BLDC_ch1
wire [31:0] temp_duty01;				//BLDC_ch2
wire [31:0] temp_duty10;				//BLDC_ch3
wire [31:0] temp_duty11;				//BLDC_ch4
assign temp_duty00 = min_duty + {4'b0000,pwm_dat00[7:0],4'b0000};
assign temp_duty01 = min_duty + {4'b0000,pwm_dat01[7:0],4'b0000};
assign temp_duty10 = min_duty + {4'b0000,pwm_dat10[7:0],4'b0000};
assign temp_duty11 = min_duty + {4'b0000,pwm_dat11[7:0],4'b0000};
always @(posedge clk or negedge pin2)begin
	if (pin2==1'b0)begin
		base_clk_counter <= 24'b0;
		vld_trig <= 3'b0;
		status <= 8'b0;
		pwm_dat00 <= 8'b0;				//BLDC_ch1
		pwm_dat01 <= 8'b0;				//BLDC_ch2
		pwm_dat10 <= 8'b0;				//BLDC_ch3
		pwm_dat11 <= 8'b0;				//BLDC_ch4
		dat_count <= 3'b000;
	end else begin
		vld_trig[2:0] <= {vld_trig[1:0],cap_data_vld};
		if (vld_trig == 3'b110)begin							//I2Cのデータ確定信号を検出
			if (cap_data_out[7] == 1'b0)begin					//最上位ビットが0ならデータ   0x00～0x7F	
				case(dat_count)
					3'b001:begin				//BLDC_ch1
						pwm_dat00 <= cap_data_out;
						dat_count <= 3'b010;
						end
					3'b010:begin				//BLDC_ch2
						pwm_dat01 <= cap_data_out;
						dat_count <= 3'b011;
						end
					3'b011:begin				//BLDC_ch3
						pwm_dat10 <= cap_data_out;
						dat_count <= 3'b100;
						end
					3'b100:begin				//BLDC_ch4
						pwm_dat11 <= cap_data_out;
						dat_count <= 3'b101;
						end
					default:     dat_count <= 3'b000;
				endcase
			end else begin											//最上位ビットが1ならステータス  7:status/data	6...2:reserved   1:OneShot   0:PWM_ON
				dat_count <= 3'b001;
				status <= cap_data_out;
				if (cap_data_out[1] == 1'b1)begin
					one_shot <= 1'b1;
				end
			end
		end
		//
		if (base_clk_counter == 32'b1010_1000_0000_0000)begin  //周期20ms の最終クロック-------------
			if (status[0] == 1'b1)begin							//PWM ONなら出力に1を代入
				pin5 <= 1'b1;				//BLDC_ch1
				pin6 <= 1'b1;				//BLDC_ch2
				pin21 <= 1'b1;				//BLDC_ch3
				pin22 <= 1'b1;				//BLDC_ch4
			end else begin
				pin5 <= 1'b0;				//BLDC_ch1
				pin6 <= 1'b0;				//BLDC_ch2
				pin21 <= 1'b0;				//BLDC_ch3
				pin22 <= 1'b0;				//BLDC_ch4
			end
			if (one_shot == 1'b1)begin								//PWM OneShot ONなら出力に1を代入、OneShotはOFF
				pin5 <= 1'b1;				//BLDC_ch1
				pin6 <= 1'b1;				//BLDC_ch2
				pin21 <= 1'b1;				//BLDC_ch3
				pin22 <= 1'b1;				//BLDC_ch4
				one_shot <= 1'b0;
			end
			//-----------------------------------	//カウンターを0に戻す
			base_clk_counter <= 32'b0;
			//-----------BLDC_ch1
			if (pwm_dat00 == 8'b0111_1111)begin
				pwm_duty00 <= max_duty;		//データが0x7Fだったらdutyはmax_duty
			end else begin
				pwm_duty00 <= temp_duty00;
			end
			//-----------BLDC_ch2
			if (pwm_dat01 == 8'b0111_1111)begin
				pwm_duty01 <= max_duty;		//データが0x7Fだったらdutyはmax_duty
			end else begin
				pwm_duty01 <= temp_duty01;
			end
			//-----------BLDC_ch3
			if (pwm_dat10 == 8'b0111_1111)begin
				pwm_duty10 <= max_duty;		//データが0x7Fだったらdutyはmax_duty
			end else begin
				pwm_duty10 <= temp_duty10;
			end
			//-----------BLDC_ch4
			if (pwm_dat11 == 8'b0111_1111)begin
				pwm_duty11 <= max_duty;		//データが0x7Fだったらdutyはmax_duty
			end else begin
				pwm_duty11 <= temp_duty11;
			end
		//周期の最終クロックでなければ-----------------------------
		end else begin
			if (base_clk_counter >= pwm_duty00)begin //PWMのdutyの最後で出力に0を代入
				pin5 <= 1'b0;	// BLDC_ch1
			end
			if (base_clk_counter >= pwm_duty01)begin //PWMのdutyの最後で出力に0を代入
				pin6 <= 1'b0;	// BLDC_ch2
			end
			if (base_clk_counter >= pwm_duty10)begin //PWMのdutyの最後で出力に0を代入
				pin21 <= 1'b0;	// BLDC_ch3
			end
			if (base_clk_counter >= pwm_duty11)begin //PWMのdutyの最後で出力に0を代入
				pin22 <= 1'b0;	// BLDC_ch4
			end
			// ---------------//周期カウンターをインクリメント
			base_clk_counter <= base_clk_counter + 1;
		end
	end
end
//-------------------------------------------------------------


endmodule
