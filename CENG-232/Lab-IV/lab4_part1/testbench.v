`timescale 1ns / 1ps

module testbench_4_1;
	// Inputs and outputs
	reg CLK; reg tmp; integer i; real grade;
	/*for ROM */ reg [2:0] romAddr; wire [7:0] romDataOut;
	/*for XOR_RAM*/ reg ramMode; reg [2:0] ramAddr; reg [7:0] dataIn; reg [7:0] mask; wire [7:0] ramDataOut;
	/*for EncodedMemory*/ reg Mode; reg [2:0] Index; reg [7:0] Number; wire [7:0] Result;

	//instances
	ROM INSTANCE_ROM(romAddr, romDataOut);
	XOR_RAM INSTANCE_XOR_RAM(ramMode, ramAddr, dataIn, mask, CLK, ramDataOut);
	EncodedMemory INSTANCE_EncodedMemory(Mode, Index, Number, CLK, Result);
	
	//Set initial values
	initial CLK = 0;	
	always #5 CLK = ~CLK;
  
	initial begin
		grade = 0; //reset grade
		
		//initialize all inputs
		ramMode=1'b1; ramAddr=3'b000; dataIn=8'b00000000; mask=8'b00000000; 
		Mode=1'b1; Index=3'b000; Number=8'b00000000;
		
		
		//start testbench on ROM
		$display("Starting Testbench");
		#1//1 
		romAddr=3'b000; 
		#1//2
		if(romDataOut==8'b00000000) grade=grade+1; else $display("%t,Error in case 1",$time);/*case 1*/
		#1//3 
		romAddr=3'b001; 
		#1//4
		if(romDataOut==8'b01010101) grade=grade+1; else $display("%t,Error in case 2",$time);/*case 2*/ 
		#1//5 
		romAddr=3'b010; 
		#1//6
		if(romDataOut==8'b10101010) grade=grade+1; else $display("%t,Error in case 3",$time);/*case 3*/
		#1//7 
		romAddr=3'b011;
		#1//8
		if(romDataOut==8'b00110011) grade=grade+1; else $display("%t,Error in case 4",$time);/*case 4*/
		#1//9 
		romAddr=3'b100;
		#1//10
		if(romDataOut==8'b11001100) grade=grade+1; else $display("%t,Error in case 5",$time);/*case 5*/
		#1//11 
		romAddr=3'b101;
		#1//12
		if(romDataOut==8'b00001111) grade=grade+1; else $display("%t,Error in case 6",$time);/*case 6*/
		#1//13 
		romAddr=3'b110;
		#1//14
		if(romDataOut==8'b11110000) grade=grade+1; else $display("%t,Error in case 7",$time);/*case 7*/
		#1//15 
		romAddr=3'b111;
		#1//16
		if(romDataOut==8'b11111111) grade=grade+1; else $display("%t,Error in case 8",$time);/*case 8*/
		
		
		//start testbench on XOR_RAM 
		
		//initial values of ram cells should be 0.
		ramMode=1'b1; //read mode
		tmp=0;
		for (i=0;i<=7;i=i+1)
		begin
			ramAddr = i;
			#0.5;
			if(ramDataOut == 0) begin end
			else tmp=1;
		end
		
		if(tmp==0) grade=grade+3;
		else $display("%t,Error in case 9",$time);/*case 9*/
		
		#2;//22
		
		ramMode=1'b0;//write mode
		ramAddr=3'b000;
		dataIn=8'b11001100;
		mask=  8'b10101010;
		#1//23
		//Result of previous write operation should not be changed before positive edge
		ramMode=1'b1;//read mode
		#1//24
		if (ramDataOut==8'b00000000) grade=grade+1; else $display("%t,Error in case 10",$time);/*case 10*/
		#3//27
		
		ramMode=1'b0;//write mode
		#9//36
		
		ramMode=1'b1;//read mode
		#1//37
		//Result should change after positive edge CLK. 11001100 XOR 10101010 = 01100110
		if (ramDataOut==8'b01100110) grade=grade+1;else $display("%t,Error in case 11",$time); /*case 11*/
		
		#1//38
		ramMode=1'b0;//write mode
		ramAddr=3'b001;
		dataIn=8'b01001100;
		mask=  8'b11111010;
		#8//46
		
		//ramDataOut should not be changed in write mode
		if (ramDataOut==8'b01100110) grade=grade+3; else $display("%t,Error in case 12",$time);/*case 12*/
		
		
		//start testbench on EncodedMemory 

		#3//49
		Mode=1'b0;//write mode
		Index=3'b111;
		Number=8'b10101010;
		
		while($time<=119)
		begin
			#10
			Index=Index+1;
		end
		
		//time is 129
		
		Mode=1'b1;//read mode
		Index=3'b111;
		while($time<=144)
		begin
			#1
			Index=Index+1;
			#1
			case(Index)
			3'b000:if (Result==8'b10101010)  grade=grade+2; else $display("%t,Error in case 13",$time);/*case 13*/ 
			3'b001:if (Result==8'b11111111)  grade=grade+2; else $display("%t,Error in case 14",$time);/*case 14*/ 
			3'b010:if (Result==8'b00000000)  grade=grade+2; else $display("%t,Error in case 15",$time);/*case 15*/ 
			3'b011:if (Result==8'b10011001)  grade=grade+2; else $display("%t,Error in case 16",$time);/*case 16*/ 
			3'b100:if (Result==8'b01100110)  grade=grade+2; else $display("%t,Error in case 17",$time);/*case 17*/ 
			3'b101:if (Result==8'b10100101)  grade=grade+2; else $display("%t,Error in case 18",$time);/*case 18*/ 
			3'b110:if (Result==8'b01011010)  grade=grade+2; else $display("%t,Error in case 19",$time);/*case 19*/ 
			3'b111:if (Result==8'b01010101)  grade=grade+2; else $display("%t,Error in case 20",$time);/*case 20*/ 
			endcase
		end
		#20
		$display("Result is:%.2f",grade/32*100); //max "grade" variable is 32. max result is 100 points.
		$finish;
	end
	 
endmodule

