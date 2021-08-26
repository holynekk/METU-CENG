`timescale 1ns / 1ps
	module kb(
	input K,
	input B,
	input clk,
	output reg Q
	);
    
	initial begin
		Q = 0;
	end
	 
	// write your code here
	
	
	always@(posedge clk)
		begin
			if (K == 0 && B == 0) Q = ~Q;
			else if (K == 0 && B == 1) Q = 0;
			else if (K == 1 && B == 0) Q = 1;
			else Q = Q;
		end

endmodule


module ic232(
	input A0, 
	input A1, 
	input A2, 
	input clk,
	output Q0,
	output Q1,
	output Z
	);
   
	// write your code here
	
	wire w1;
	wire w2;
	
	assign w1 = ~(A0 | A1);
	assign w2 = w1 & ~A2;
	
	wire w3;
	wire w4;
	
	assign w3 = ~A0 | A1;
	assign w4 = w3 ^ A2;
	
	
	kb kb_above(w2, A1, clk, Q0);
	
	kb kb_bottom(~A2, w4, clk, Q1);
	
	assign Z = ~(Q0 ^ Q1);
	
endmodule