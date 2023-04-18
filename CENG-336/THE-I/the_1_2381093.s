PROCESSOR 18F8722
    
#include <xc.inc>

; configurations
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

; global variable declarations
GLOBAL _t1, _t2, _t3    ; variables for time delay (busy wait)
GLOBAL var1
GLOBAL blink_flagB
GLOBAL blink_flagC
GLOBAL re4_pressed_0
GLOBAL re4_pressed_1
GLOBAL re4_pressed_2
GLOBAL rA4_pressed_0
GLOBAL rA4_pressed_1
GLOBAL arbitrary_num_holder
GLOBAL arbitrary_num_holder_C
GLOBAL at_Port_B
GLOBAL at_Port_C
GLOBAL at_Port_D
GLOBAL countdown



; allocating memory for variables
PSECT udata_acs
    var1:
	DS  1
    ; blinking bol variables ++++++++
    blink_flagB:
	DS  1
    blink_flagC:
	DS  1
    ; +++++++++++++++++++++++++++++++
    ; busy wait variables -----------
    _t1:
        DS 1    ; allocate 1 byte
    _t2:
        DS 1    ; allocate 1 byte
    _t3:
        DS 1    ; allocate 1 byte
    ; -------------------------------
    re4_pressed_0:
	DS 1
    re4_pressed_1:
	DS 1
    re4_pressed_2:
	DS 1
    rA4_pressed_0:
	DS 1
    rA4_pressed_1:
	DS 1
    arbitrary_num_holder:
	DS 1
    arbitrary_num_holder_C:
	DS 1
    C_holder:
	DS 1
    at_Port_B:
	DS 1
    at_Port_C:
	DS 1
    at_Port_D:
	DS 1
    countdown:
	DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto    main

; DO NOT DELETE OR MODIFY
; 500ms pass check for test scripts
ms500_passed:
    nop
    return

; DO NOT DELETE OR MODIFY
; 1sec pass check for test scripts
ms1000_passed:
    nop
    return
    
; ---------------------------------------------------------------
busy_delay_1sec:
    movlw 0x84      ; copy desired value to W
    movwf _t3       ; copy W into t3
    _loop3:
        movlw 0xAF      ; copy desired value to W
        movwf _t2       ; copy W into t2
        _loop2:
            movlw 0x8F      ; copy desired value to W
            movwf _t1       ; copy W into t1
            _loop1:
		decfsz _t1, 1   ; decrement t1, if 0 skip next 
		goto _loop1     ; else keep counting down
		decfsz _t2, 1   ; decrement t2, if 0 skip next 
		goto _loop2     ; else keep counting down
		decfsz _t3, 1   ; decrement t3, if 0 skip next 
		goto _loop3     ; else keep counting down
		return

countdown_busy_delay:
    movlw 0x37      ; copy desired value to W
    movwf _t3       ; copy W into t3
    _loopy3:
        movlw 0xAF      ; copy desired value to W
        movwf _t2       ; copy W into t2
        _loopy2:
            movlw 0x48      ; copy desired value to W
            movwf _t1       ; copy W into t1
            _loopy1:
		decfsz _t1, 1   ; decrement t1, if 0 skip next 
		goto _loop1     ; else keep counting down
		decfsz _t2, 1   ; decrement t2, if 0 skip next 
		goto _loop2     ; else keep counting down
		decfsz _t3, 1   ; decrement t3, if 0 skip next 
		goto _loop3     ; else keep counting down
		return
; ---------------------------------------------------------------
		
init:
    ; Clearing all values
    CLRF blink_flagB
    CLRF blink_flagC
    CLRF C_holder
    CLRF PORTA
    CLRF PORTB
    CLRF PORTC
    CLRF PORTD
    CLRF PORTE
    CLRF _t1
    CLRF _t2
    CLRF _t3
    CLRF countdown
    
    movlw 0x01
    movwf C_holder
    movwf countdown

    ; Settings PORTB, PORTC, and PORTD as input ports
    movlw 00000000B
    movwf TRISB
    movwf TRISC
    movwf TRISD
    
    ; Turn on RC[0-1], RB[0-3], and RD[0-7]
    movlw 00000011B
    movwf LATC
    movlw 00001111B
    movwf LATB
    movlw 11111111B
    movwf LATD
    
    ; Set RA[4] AND RE[4] as inputs.
    movlw 00010000B
    movlw TRISA
    movlw 00010000B
    movlw TRISE
    
    ; Call delay function for 1 second
    call busy_delay_1sec
    
    return
    
	
PSECT CODE
main:
    ; some code to initialize and wait 1000ms here, maybe
    call init
    
    ; call ms1000_passed for debugging purposes
    call ms1000_passed
    
    ; Default Configuration
    movlw 00000001B
    movwf LATB
    movlw 00000001B
    movwf LATC
    movlw 00000000B
    movwf LATD
    
    loop:
        ; inside the loop, once it is confirmed 500ms passed
	; Button presses -----------------------------------
	; This is a loop that waits for button presses and releases inside the inner most loop
	movlw 0x1A
	movwf _t3
	_loop4:
	    movlw 0x4F
	    movwf _t2
	    _loop5:
		movlw 0x45
		movwf _t1
		_loop6:
		    ; check if we already hit the Port D section
		    check_position_D:
			btfsc at_Port_D, 0
			goto congrats_you_are_in_portD
			goto check_position_C
		    ; check if we already hit the Port C section
		    check_position_C:
			btfsc at_Port_C, 0
			goto first_check_forC
			goto check_C_pressed_ra4
		    ; check if we already pressed ra4 at Port C
		    check_C_pressed_ra4:
			btfsc rA4_pressed_1, 0
			goto wait_rA4_release_1
			goto first_check_or_already_pressed
		    ; check if we already hit the Port B section
		    first_check_or_already_pressed:
			btfsc blink_flagB, 0
			goto wait_for_ra4_or_re4
			goto first_check_forB
		    ; Blinking started but no button has been pressed
		    first_check_forB:
			btfsc re4_pressed_0, 0
			goto wait_re4_release_0
			goto check_re4_0
		    
		    check_re4_0:
			btfsc PORTE, 4          ; if ra4 is currently not pressed, return
			goto wait_re4_release_0   ; if pressed, wait until released
			goto continue_loop	; if not pressed, continue loop
		    wait_re4_release_0:
			movlw 00000001B
			movwf re4_pressed_0
			btfsc PORTE, 4          ; if re4 is not released
			goto continue_loop   ; wait until it is released (else skip here)
			goto control_portB_blink
			
		    control_portB_blink:
			movlw 0x01
			movwf blink_flagB
			movlw 0x00
			movwf re4_pressed_0 ;----------------------------------------------------------
			movwf at_Port_B
			movff LATB, arbitrary_num_holder
			
		    ; Check if re4 pressed again
		    wait_for_ra4_or_re4:
			btfsc re4_pressed_1, 0
			goto wait_re4_release_1
			goto check_re4_1
		    check_re4_1:
			btfsc PORTE, 4
			goto wait_re4_release_1
			goto check_rA4_pressed_0
		    wait_re4_release_1:
			movlw 00000001B
			movwf re4_pressed_1
			btfsc PORTE, 4
			goto continue_loop
			goto terminate_portB_blink

		    ; Check if rA4 is pressed while portB is blinking
		    check_rA4_pressed_0:
			btfsc rA4_pressed_0, 0
			goto wait_rA4_release_0
			goto check_rA4_pressed_0_bum
		    check_rA4_pressed_0_bum:
			btfsc PORTA, 4
			goto wait_rA4_release_0
			goto continue_loop
		    wait_rA4_release_0:
			movlw 00000001B
			movwf rA4_pressed_0
			btfsc PORTA, 4
			goto continue_loop
			goto increase_PortB
		    increase_PortB:
			movlw 0x00
			movwf rA4_pressed_0
			movf arbitrary_num_holder, 0
			addwf arbitrary_num_holder, 1
			movlw 0x01
			addwf arbitrary_num_holder, 1
			; Check the 5th bit of PORT B
			btfsc arbitrary_num_holder, 4
			goto make_LATB_1_0
			goto continue_loop
		    make_LATB_1_0:
			movlw 00000001B
			movwf arbitrary_num_holder
			goto continue_loop

		    terminate_portB_blink:
			; Restore LATB
			movlw 0x00
			movwf blink_flagB
			movwf at_Port_B
			movff arbitrary_num_holder, LATB
			
			movlw 0x01
			movwf blink_flagC
			movwf at_Port_C
			movff LATC, arbitrary_num_holder_C
					
		    ;Wait for re4 or ra4 press again --------------------------------------------------------------
		    first_check_forC:
			btfsc re4_pressed_2, 0
			goto wait_re4_release_2
			goto check_re4_2
		    
		    check_re4_2:
			btfsc PORTE, 4
			goto wait_re4_release_2
			goto check_rA4_pressed_1
			
		    wait_re4_release_2:
			movlw 0x01
			movwf re4_pressed_2
			btfsc PORTE, 4
			goto continue_loop ; --- check ra4 pressed
			goto terminate_portC_blink

		    check_rA4_pressed_1:
			btfsc rA4_pressed_1, 0
			goto wait_rA4_release_1
			goto check_rA4_pressed_1_bum
			
		    check_rA4_pressed_1_bum:
			btfsc PORTA, 4
			goto wait_rA4_release_1
			goto continue_loop
		    
		    wait_rA4_release_1:
			movlw 0x01
			movwf rA4_pressed_1
			btfsc PORTA, 4
			goto continue_loop
			goto change_Port_C
			
		    change_Port_C:
			movlw 0x00
			movwf rA4_pressed_1
			btfsc PORTC, 0
			goto ligt_C_1
			goto ligt_C_0
			
		    ligt_C_0:
			movlw 00000001B
			movwf arbitrary_num_holder_C
			goto continue_loop
		    ligt_C_1:
			movlw 00000010B
			movwf arbitrary_num_holder_C
			goto continue_loop
			
		    terminate_portC_blink:
			movlw 0x00
			movwf blink_flagC
			movwf at_Port_C
			movff arbitrary_num_holder_C, LATC
			movlw 0x01
			movwf at_Port_D
		    ; Do calculation for number in PORT_D
		    calculate_PortD:
			btfsc PORTC, 0
			goto normal
			goto double_case_0
		    normal:
			movff PORTB, LATD
			goto congrats_you_are_in_portD
			
		    double_case_0:
			btfsc PORTB, 1
			goto double_case_1
			goto case_0
		    case_0:
			movlw 00000011B
			movwf LATD
			goto congrats_you_are_in_portD
			
		    double_case_1:
			btfsc PORTB, 2
			goto double_case_2
			goto case_1
		    case_1:
			movlw 00001111B
			movwf LATD
			goto congrats_you_are_in_portD
			
		    double_case_2:
			btfsc PORTB, 3
			goto otherwise
			goto case_2
		    case_2:
			movlw 00111111B
			movwf LATD
			goto congrats_you_are_in_portD
		    
		    otherwise:
			movlw 11111111B
			movwf LATD
			
		    ; go to portD
		    congrats_you_are_in_portD:
			; Get yourself a coffee and watch the timer goes down 
			goto continue_loop
		    
		    ; This will make our loop non-busy. If there is nothing to do,
		    ; continue with loop to keep program responsive for inputs
		    continue_loop:
			decfsz _t1, 1   ; decrement t1, if 0 skip next 
			goto _loop6     ; else keep counting down
			decfsz _t2, 1   ; decrement t2, if 0 skip next 
			goto _loop5     ; else keep counting down
			decfsz _t3, 1   ; decrement t3, if 0 skip next 
			goto _loop4     ; else keep counting down  
	; --------------------------------------------------
	call ms500_passed
	
	should_blinkB:
	    btfsc blink_flagB, 0
	    goto blink_B
	    goto should_blinkC
	    
	should_blinkC:
	    btfsc blink_flagC, 0
	    goto blink_C
	    goto should_count_down
	
	blink_B:
	    btfsc LATB, 0
	    goto dark_B
	    goto ligth_B
	ligth_B:
	    movff arbitrary_num_holder, LATB
	    goto loop
	dark_B:
	    movlw 0x00
	    movwf LATB
	    goto loop
	    
	blink_C:
	    movlw 0x00
	    cpfseq LATC
	    goto dark_C
	    goto ligth_C
	ligth_C:
	    movff arbitrary_num_holder_C, LATC
	    goto loop
	dark_C:
	    movlw 0x00
	    movwf LATC
	    goto loop

	; Check if we should start to count
	should_count_down:
	    btfsc at_Port_D, 0
	    goto countdown_process
	    goto do_nothing

	; Timer goes down here
	countdown_process:
	    call countdown_busy_delay
	    btfsc PORTD, 0
	    goto count_case_0
	    goto back_to_default
	    
	back_to_default:
	    movlw 00000001B
	    movwf LATB
	    movlw 00000001B
	    movwf LATC
	    ; Clear all the variables that we create (make them 0)
	    clrf blink_flagB
	    clrf blink_flagC
	    clrf re4_pressed_0
	    clrf re4_pressed_1
	    clrf re4_pressed_2
	    clrf rA4_pressed_0
	    clrf rA4_pressed_1
	    clrf arbitrary_num_holder
	    clrf arbitrary_num_holder_C
	    clrf at_Port_B
	    clrf at_Port_C
	    clrf at_Port_D
	    clrf countdown
	    call ms500_passed
	    goto loop
	    
	count_case_0:
	    btfsc PORTD, 1
	    goto count_case_1
	    goto count_down_0
	count_down_0:
	    movlw 00000000B
	    movwf LATD
	    goto loop
	    
	count_case_1:
	    btfsc PORTD, 2
	    goto count_case_2
	    goto count_down_1
	count_down_1:
	    movlw 00000001B
	    movwf LATD
	    goto loop
	    
	count_case_2:
	    btfsc PORTD, 3
	    goto count_case_3
	    goto count_down_2
	count_down_2:
	    movlw 00000011B
	    movwf LATD
	    goto loop
	    
	count_case_3:
	    btfsc PORTD, 4
	    goto count_case_4
	    goto count_down_3
	count_down_3:
	    movlw 00000111B
	    movwf LATD
	    goto loop
	    
	count_case_4:
	    btfsc PORTD, 5
	    goto count_case_5
	    goto count_down_4
	count_down_4:
	    movlw 00001111B
	    movwf LATD
	    goto loop
	
	count_case_5:
	    btfsc PORTD, 6
	    goto count_case_6
	    goto count_down_5
	count_down_5:
	    movlw 00011111B
	    movwf LATD
	    goto loop
	    
	count_case_6:
	    btfsc PORTD, 7
	    goto count_otherwise
	    goto count_down_6
	count_down_6:
	    movlw 00111111B
	    movwf LATD
	    goto loop
	    
	count_otherwise:
	    movlw 01111111B
	    movwf LATD
	    goto loop
	    
	do_nothing:
	    goto loop
	; --------------------------------------------------
	

end resetVec
	

