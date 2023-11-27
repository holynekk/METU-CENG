#include "ad.h"

#include <xc.h>

/*
 * INFO
 * 
 * Voltage references are set to device high and low.
 * 
 * All a/d port pins are set to analog input.
 * 
 * T_AD = 32*T_OSC (0.8us)
 * -- It is the a/d conversion clock period.
 * -- Minimum value is 0.7us for oscillator based clock (TABLE 28-27).
 * -- Possible values are either 32 or 64 times T_OSC for PIC18F8722 (TABLE 21-1).
 * -- 32*T_OSC is chosen for higher response time.
 * 
 * T_ACQ = 4*T_AD (3.2us)
 * -- It is the acquisition time required for a/d conversion.
 * -- T_AD is defined as the a/d conversion time per bit (p. 275).
 * -- We need a resolution of 16, which can be achieved with 4 bits.
 * -- 4*T_AD is enough for the acquisition time.
 */

extern uint8_t desired_cursor_index;

void ad_init(void) {
    // Potentiometer input signal is on RH4, which corresponds to a/d input channel 12.
    LATHbits.LH4 = 0;
    TRISHbits.RH4 = 1;
    ADCON0bits.CHS = 12;
    // Set T_AD to 32*T_OSC.
    ADCON2bits.ADCS = 0b010;
    // Set T_ACQ to 4*T_AD.
    ADCON2bits.ACQT = 0b010;
    // Set controls.
    ADCON0bits.ADON = 1;
    // Configure timer.
    T2CONbits.T2CKPS = 0b11; // 1:16 pre-scale.
    T2CONbits.TOUTPS = 0b0010; // 1:4 post-scale.
    // Enable interrupts.
    TMR2IF = 0;
    TMR2IE = 1;
    ADIF = 0;
    ADIE = 1;
    // Begin a/d conversions right away.
    ADCON0bits.GO = 1;
}



void ad_isr_adif(void) {
    ADIF = 0;
    // ADRES is left justified.
    // We can divide the result into 16 parts by getting the most significant 4 bits.
    desired_cursor_index = ADRESH >> 4;
    // Wait 2*T_AD before beginning next conversion.
    TMR2 = 0;
    T2CONbits.TMR2ON = 1;
}

void ad_isr_tmr2if(void) {
    TMR2IF = 0;
    // New conversion can begin.
    T2CONbits.TMR2ON = 0;
    ADCON0bits.GO = 1;
}
