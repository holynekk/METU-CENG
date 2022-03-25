PROCESSOR 18F8722
    
#include <xc.inc>

; configurations
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

; global variable declarations
GLOBAL var1

; allocating memory for variables
PSECT udata_acs
    var1:
	DS     1    ; allocates 1 byte

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
	
PSECT CODE
main:
    ; some code to initialize and wait 1000ms here, maybe
    call ms1000_passed
    ; a loop here, maybe
    loop:
        ; inside the loop, once it is confirmed 500ms passed
        call ms500_passed
        goto loop

end resetVec