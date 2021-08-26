`timescale 1ns / 1ps
// TO USE THIS TESTBENCH on XILINX:
// Select "Simulation -> Run All" from menu
//Check 1xx:display, 0: remA, 1:remB, 2:addA, 3:addB
module testbench_grader2(
    );
	 
	wire        greenForA, greenForB, rushHourWarning, am_pm;
	wire [3:0]  hour;
	wire [5:0]  minute;	
	wire [5:0]  second;
	reg  [4:0]  plateIn;
	wire [4:0]  numOfCarsA,numOfCarsB,blackListDisplay;
	wire [6:0]  remainingTime;
	reg  [2:0]  mode; //1xx:display, 0: remA, 1:remB, 2:addA, 3:addB
	reg action;
	reg clk;
	
	//locals
	integer i,k;
	integer result;
	reg tmp;
	reg [100:0] str;
	reg        greenForAPrev, greenForBPrev, rushHourWarningPrev;
	reg [3:0]  hourPrev;
	reg [5:0]  minutePrev;	
	reg [5:0]  secondPrev;
	reg [4:0]  numOfCarsAPrev,numOfCarsBPrev;
	reg [6:0]  remainingTimePrev;	
	reg [4:0] blackListCheck [1:10];
	//end locals

	// Instantiate the Unit Under Test (UUT)
   IntersectionSimulator uut(mode, plateIn, action, clk, greenForA, greenForB, rushHourWarning, hour, minute, second, am_pm, numOfCarsA, numOfCarsB, remainingTime, blackListDisplay);

	// At 5, 15, 25, .. clk will change 0->1
	// At 10, 20, 30, .. clk will change 1->0
   initial clk = 0;
	always #5 clk = ~clk;
	

	task switchTheLight;
	begin
		k=0;
		if (greenForA)
			while(greenForA && k<1000)
			begin
				k=k+1;
				#1;
			end //while
		else
			while(greenForB && k<1000)
			begin
				k=k+1;
				#1;
			end //while	
		#10; //red red condition
	end
	endtask

	
	initial begin
	result=0;
	
	//CASE 1:Add 5 cars (1,2,3,4,5) to road A at green light  (currently roadA=Green, roadB=Red)
	mode=3; plateIn=0;action=0;
	#1;
	str="CASE 1";
	mode=2; //addA
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		plateIn=i;
		action=1;
		#1;
	end
	if (numOfCarsA==5) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	//CASE 2:Add 5 cars (6,7,8,9,10) to road B at red light (currently roadA=Green, roadB=Red)
	mode=3; plateIn=0;action=0;
	#1;
	str="CASE 2";
	mode=3; //addB
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		plateIn=i+5;
		action=1;
		#1;
	end
	if (numOfCarsB==5) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	switchTheLight;
	
	//CASE 3:Add additional 5 cars (11,12,13,14,15) to road A at red light (currently roadA=Red, roadB=Green)
	#1;
	str="CASE 3";
	mode=2; //addA
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		plateIn=i+10;
		action=1;
		#1;
	end
	if (numOfCarsA==10) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	//CASE 4:Add additional 5 cars (16,17,18,19,20) to road B at green light (currently roadA=Red, roadB=Green)
	#1;
	str="CASE 4";
	mode=3; //addB
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		plateIn=i+15;
		action=1;
		#1;
	end
	if (numOfCarsB==10) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	
	//CASE 5: Remove  5 cars from road A at red light (currently roadA=Red, roadB=Green). They should be added to the blacklist. 
	#1;
	str="CASE 5"; 
	mode=0; //remA
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		action=1;
		#1;
		blackListCheck[i]=i;
	end
	if (numOfCarsA==5) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	//CASE 6: Remove  5 cars from road B at green light (currently roadA=Red, roadB=Green)
	#1;
	str="CASE 6"; 
	mode=1; //remB 
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		action=1;
		#1;
	end
	if (numOfCarsB==5) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);	
	
	switchTheLight;
	
	//CASE 7: Remove  5 cars from road A at green light (currently roadA=Green, roadB=Red)
	#1;
	str="CASE 7"; 
	mode=0; //remA
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		action=1;
		#1;
	end
	if (numOfCarsA==0) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	//CASE 8: Remove  5 cars from road B at red light (currently roadA=Green, roadB=Red). They should be added to the blacklist. 
	#1;
	str="CASE 8"; 
	mode=1; //remB 
	for(i = 1; i <= 5; i=i+1) begin
		action=0;
		#1;
		action=1;
		#1;
		blackListCheck[i+5]=i+15;
	end
	if (numOfCarsB==0) 
	begin
		result = result + 1; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);	
	
	//CASE 9: Check Display Mode (Cars in BlackList: 1,2,3,4,5,16,17,18,19,20)
	mode=5; //Display
	#1;
	//save current values for Case 10
	greenForAPrev=greenForA; greenForBPrev=greenForB; rushHourWarningPrev=rushHourWarning;
	hourPrev=hour; minutePrev=minute; secondPrev=second;
	remainingTimePrev=remainingTime;	
	
	str="CASE 9_"; 
	
	
	for(i = 1; i <= 10; i=i+1) begin
		#10;
		if (blackListCheck[i]==blackListDisplay) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,i[3:0]);
		end else $display("%t: %s%d failed!!", $time, str,i[3:0]);
	end
	
	#10; //CASE 9_11: After all cars are displayed, listing starts from the beginning again.
	if (blackListCheck[1]==blackListDisplay) 
	begin
		result = result + 1; 
		$display("%t: %s%d passed.", $time, str,4'b1011);
	end else $display("%t: %s%d failed!!", $time, str,4'b1011);
	#10; //CASE 9_12
	if (blackListCheck[2]==blackListDisplay) 
	begin
		result = result + 1; 
		$display("%t: %s%d passed.", $time, str,4'b1100);
	end else $display("%t: %s%d failed!!", $time, str,4'b1100);
	
	
	//CASE 10: Time related outputs should not change in display mode. After display mode, these outputs.
	#100000;
	mode=2; //addA
	str="CASE 10_";
	#1;
	if (greenForAPrev==greenForA) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0001);
		end else $display("%t: %s%d failed!!", $time, str,4'b0001);
	if (greenForBPrev==greenForB) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0010);
		end else $display("%t: %s%d failed!!", $time, str,4'b0010);
	if (rushHourWarningPrev==rushHourWarning) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0011);
		end else $display("%t: %s%d failed!!", $time, str,4'b0011);
	if (hourPrev==hour) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0100);
		end else $display("%t: %s%d failed!!", $time, str,4'b0100);
	if (minutePrev==minute) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0101);
		end else $display("%t: %s%d failed!!", $time, str,4'b0101);
	if (secondPrev==second) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0110);
		end else $display("%t: %s%d failed!!", $time, str,4'b0110);
	if (remainingTimePrev==remainingTime) 
		begin
			result = result + 1; 
			$display("%t: %s%d passed.", $time, str,4'b0111);
		end else $display("%t: %s%d failed!!", $time, str,4'b0111);		
		
		
	//CASE 11: Each time Display mode is activated, the plates should be displayed from the beginning.
	#1;
	str="CASE 11_"; 
	mode=5; //Display mode is activated again(previous mode is addA)
	#10
	if (blackListCheck[1]==blackListDisplay) //first display item should be 1
	begin
		result = result + 2; 
		$display("%t: %s%d passed.", $time, str,4'b0001);
	end else $display("%t: %s%d failed!!", $time, str,4'b0001);			
	
	#10
	if (blackListCheck[2]==blackListDisplay) //second display item should be 2
	begin
		result = result + 1; 
		$display("%t: %s%d passed.", $time, str,4'b0010);
	end else $display("%t: %s%d failed!!", $time, str,4'b0010);		
	
	
	
	//CASE 12:blackListDisplay should continue to show the last displayed plate number until the midnight reset
	#1;
	str="CASE 12_"; 
	mode=2; //addA
	i=0;
	//go to 4:00:00 PM
	while( (hour != 4 || minute != 0 || am_pm != 1) && i< 700000)
	begin
		#1;
		i=i+1;
	end
	if (blackListCheck[2]==blackListDisplay) //second display the last displayed plate which is 2
	begin
		result = result + 2; 
		$display("%t: %s%d passed.", $time, str,4'b0001);
	end else $display("%t: %s%d failed!!", $time, str,4'b0001);		
	
	//go to 12:00:00 AM
	while( (hour != 12 || minute != 0 || am_pm != 0) && i< 700000)
	begin
		#1;
		i=i+1;
	end
	//Many hours have passed. It is now midnight 12:00:00 AM.
	#10;
	if (blackListDisplay==0) //should reset to 0 
	begin
		result = result + 2; 
		$display("%t: %s%d passed.", $time, str,4'b0010);
	end else $display("%t: %s%d failed!!", $time, str,4'b0010);		
			
	//CASE 13: The blacklist should be reset at midnight (12:00:00 AM).
	str="CASE 13"; 	
	mode=5; //Display
	#1;
	tmp=0;
	for(i = 1; i <= 30; i=i+1) begin
		#10;		
		if (blackListDisplay!==0) tmp=1'b1;
	end
	if (tmp==0) 
	begin
		result = result + 2; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
		
	
	//CASE 14:If there is one car in the list, blackListDisplay should only show the plate of that car on each cycle.
	#1;
	str="CASE 14";
	if (!(greenForA==1 && greenForB==0)) switchTheLight;
	// Currently(roadA=Green, roadB=Red)
	//ADD a car (plate = 17) to road B in red light.
	mode=3; //addB
	action=0;
	#1;
	plateIn=17;
	action=1;
	#1;
	//Remove the car (plate = 17) from road B in red light.
	mode=1; //remB 
	action=0;
	#1;
	action=1;
	#1;
	//Display the car  (plate=17) in display
	mode=5; //Display
	tmp=0;
	for(i = 1; i <= 30; i=i+1) begin
		#10;		
		if (blackListDisplay!==17) tmp=1'b1;
	end
	if (tmp==0) 
	begin
		result = result + 2; 
		$display("%t: %s passed.", $time, str);
	end else $display("%t: %s failed!!", $time, str);
	
	
	$display("Result is:%d",result); //max. 38
	$finish;
	
	end
      
endmodule
