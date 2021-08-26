`timescale 1ns / 1ps
module QueueCounter(
	input [7:0] llmt,
	input [7:0] ulmt,
	input en,
	input mode, // 1:delete, 0:insert
	input clk,
	output reg [7:0] head,
	output reg [7:0] tail,
	output reg empty,
	output reg full);
	
	//you may write your code here
	
	//---------------------
	integer count = 0;
	//---------------------
	
	initial
	begin
		head = llmt;
		tail = llmt;
		empty = 1;
		full = 0;
	end

	//you may write your code here

	//---------------------
	always@(posedge clk)
		begin
			if (en == 1)
				begin 
					if (mode == 0)
						begin 
							if (full == 0)
								begin
									tail = tail + 1 > ulmt ? llmt : tail + 1;
									count = count + 1;
								end
							else;
						end
					else
						begin 
							if (empty == 0)
								begin
									head = head + 1 > ulmt ? llmt : head + 1;
									count = count - 1;
								end
							else;
						end
					
					// empty-full calculations
					if (count == 0)
						begin 
							empty = 1;
						end
					else 
						begin 
							empty = 0;
						end
					if (empty == 0 && head == tail)
						begin 
							full = 1;
						end
					else if(empty == 0 && head != tail)
						begin
							full = 0;
						end
				end
			else;
		end
	//---------------------

endmodule


module Numerator(
	input clinic, 
	input mode, 
	input en, 
	input clk,
	output [7:0] ann,
	output [7:0] print);

	//write your code here
	wire [7:0] aann1;
	wire [7:0] pprint1;
	wire [7:0] aann2;
	wire [7:0] pprint2;
	
	wire ok1;
	wire ok2;
	wire ok3;
	wire ok4;
	
	QueueCounter room_zero(8'b101, 8'b1001, en, mode, clk, aann1, pprint1, ok1, ok2);
	QueueCounter room_one(8'b1010, 8'b1110, en, mode, clk, aann2, pprint2, ok3, ok4);
	
	
	assign ann = (mode == 0) ? (clinic ? aann2 : aann1) : ann;
	assign print = (mode == 1) ? (clinic ? pprint2 : pprint1) : print;
	
endmodule
