`timescale 1ns / 1ps
module IntersectionSimulator(
	input [2:0] mode, //1xx:display, 000:remA, 001:remB, 010:addA, 011:addB
	input [4:0] plateIn,
	input action,
	input clk,
	output reg  greenForA,
	output reg  greenForB,
	output reg 	rushHourWarning,
	output reg [3:0]  hour,
	output reg [5:0]  minute,	
	output reg [5:0]  second,
	output reg	am_pm, //0:am, 1:pm
	output reg [4:0]  numOfCarsA,
	output reg [4:0]  numOfCarsB,
	output reg [6:0]  remainingTime,
	output reg [4:0]  blackListDisplay
	);
	
	//You may write your code anywhere
	
	reg [4:0] RoadA [0:29];
	reg [4:0] RoadB [0:29];
	reg [4:0] Blacklist [0:29];
	integer black_number;
	integer current_black_number;
	integer i;
	integer loop_variable;
	
	integer flag_for_ligth = 0;
	
	integer time_for_A = 40;
	integer time_for_B = 50;
	integer aaa = 0;

	initial begin
		greenForA=1;
		greenForB=0;
		rushHourWarning=0;
		hour=6;
		minute=0;
		second=0;
		am_pm=0;
		numOfCarsA=0;
		numOfCarsB=0;
		remainingTime=50;
		blackListDisplay=0;
		
		// My code
		black_number = 0;
		current_black_number = 0;
		
		for(i = 0; i < 30; i=i+1)
			begin
				RoadA[i] = 0;
				RoadB[i] = 0;
				Blacklist[i] = 0;
			end
	end

	// Asynchronous operations
	// ------------------------------------------------------------------------
	always@(posedge action)
	begin
		// Removing Cars
		if (mode == 3'b000)
			begin
				if (numOfCarsA > 0)
					begin 
						numOfCarsA = numOfCarsA - 1;
						if(!greenForA)
							begin
								Blacklist[black_number] = RoadA[0];
								black_number = black_number + 1;
							end
						else;
						for(loop_variable = 0; loop_variable < 30; loop_variable = loop_variable+1)
							begin
								RoadA[loop_variable] = RoadA[loop_variable+1];
							end
						RoadA[numOfCarsA] = 0;
					end
				else;
			end
		else if (mode == 3'b001)
			begin
				if (numOfCarsB > 0)
					begin 
						numOfCarsB = numOfCarsB - 1;
						if(!greenForB)
							begin
								Blacklist[black_number] = RoadB[0];
								black_number = black_number + 1;
							end
						else;
						for(loop_variable = 0; loop_variable < 30; loop_variable = loop_variable+1)
							begin
								RoadB[loop_variable] = RoadB[loop_variable+1];
							end
						RoadB[numOfCarsB] = 0;
					end
				else;
			end
		// Adding Cars
		else if (mode == 3'b010)
			begin
				RoadA[numOfCarsA] = plateIn;
				numOfCarsA = numOfCarsA + 1;
			end
		else if (mode == 3'b011)
			begin
				RoadB[numOfCarsB] = plateIn;
				numOfCarsB = numOfCarsB + 1;
			end
	end
	// ------------------------------------------------------------------------
	always @(posedge clk)
		begin
			// Reset blacklist
			// ******************************
			if (am_pm == 1 && hour == 11 && minute == 59 && second == 59)
				begin
					for(loop_variable = 0; loop_variable < 30; loop_variable = loop_variable+1)
						begin
							Blacklist[loop_variable] = 0;
						end
					black_number = 0;
					current_black_number = 0;
					blackListDisplay = 0;
				end
			else;
			// ******************************
			if (mode === 3'b1xx)
				// Display Blacklist
				// """""""""""""""""""""""""""""""""
				begin
					if (black_number)
						begin
							blackListDisplay = Blacklist[current_black_number];
							current_black_number = (current_black_number == black_number-1) ? 0: current_black_number+1;
						end
					else;
				end
				// """""""""""""""""""""""""""""""""
			else
				begin
					current_black_number = 0; // In order to reset initial value of loop.
					// Incremeting Time
					// ------------------------------------
					if (hour == 11 && minute == 59 && second == 59)
						begin
							am_pm = !am_pm;
						end
					else;
					if (second == 59)
						begin
							second = 0;
							if (minute == 59)
								begin
									minute = 0;
									if (hour == 12)
										begin
											hour = 1;
										end
									else
										begin
											hour = hour + 1;
										end
								end
							else
								begin
									minute = minute + 1;
								end
						end
					else 
						begin
							second = second + 1;
						end
					// ------------------------------------
					// RushHour Decision
					// ++++++++++++++++++++++++++++++++++++
					if ((am_pm == 0 && (hour == 7 || hour == 8)) || (am_pm == 1 && (hour == 5 || hour == 6)))
						begin
							rushHourWarning = 1;
						end
					else
						begin
							rushHourWarning = 0;
						end
					// ++++++++++++++++++++++++++++++++++++
					// Decrementing Remaining Time
					// ------------------------------------
					if (remainingTime == 1)
						begin
							if (greenForA)
								begin
									flag_for_ligth = 1;
									greenForA = 0;
								end
							else
								begin
									flag_for_ligth = 2;
									greenForB = 0;
								end
							remainingTime = 0;
						end
					else if (remainingTime == 0)
						begin
							if (flag_for_ligth == 1)
								begin
									greenForB = 1;
									remainingTime = time_for_A;
									
									//----------
									if (numOfCarsB >= 20)
										begin
											time_for_B = time_for_B - 5;
										end
									else if(numOfCarsB >= 11 && numOfCarsB <= 19)
										begin
											time_for_B = time_for_B;
										end
									else
										begin
											time_for_B = time_for_B + 5;
										end
									if (rushHourWarning)
										begin
											if (time_for_B > 70)
												begin
													time_for_B = 70;
												end
											else if(time_for_B < 40)
												begin
													time_for_B = 40;
												end
											else;
										end
									else
										begin
											if (time_for_B > 80)
												begin
													time_for_B = 80;
												end
											else if(time_for_B < 50)
												begin
													time_for_B = 50;
												end
											else;
										end
									//----------
								end
							else
								begin
									greenForA = 1;
									remainingTime = time_for_B;
									//---------
									if (numOfCarsA >= 20)
										begin
											time_for_A = time_for_A - 5;
										end
									else if(numOfCarsA >= 11 && numOfCarsA <= 19)
										begin
											time_for_A = time_for_A;
										end
									else
										begin
											time_for_A = time_for_A + 5;
										end
									if (rushHourWarning)
										begin
											if (time_for_A > 60)
												begin
													time_for_A = 60;
												end
											else if(time_for_A < 30)
												begin
													time_for_A = 30;
												end
											else;
										end
									else
										begin
											if (time_for_A > 70)
												begin
													time_for_A = 70;
												end
											else if(time_for_A < 40)
												begin
													time_for_A = 40;
												end
											else;
										end
									//----------
								end
						end
					else
						begin
							remainingTime = remainingTime - 1;
						end
					// ------------------------------------
				end
		end
endmodule
