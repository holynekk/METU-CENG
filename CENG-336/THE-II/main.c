///////////////////////////////////////////////////////////////////////////////
// include headers
#include "line.h"
#include "player.h"
#include <p18cxxx.h>
#include <xc.h>
// define clock frequency macro
#define _XTAL_FREQ 40000000
// line 10 added for configuring memory
#pragma config OSC = HSPLL
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//___________________________________GROUP14___________________________________
// MEMBERS:___________________Can Beydogan__________________2309755____________
//____________________________Mustafa Alperen Bitirgen______2231496____________
//____________________________Musa Kenan Marasli____________2037075__________
//____________________________Mert Kaan Yilmaz______________2381093__________
///////////////////////////////////////////////////////////////////////////////

/*
 * Timer0 Calculation
 * Device oscillator frequency = 10MHz  Period = 0.1us
 * From base value 6 to get an overflow we need 250 cycles = 25us
 * Prescaled by 256 = 6.4ms
 * We need to set as 500ms for the first level, so in the Timer0_interrupted
 * function we scaled it to 500 by counting interrupts.
 * 
 * Note: We reduced the counter number to compensate for display delay.
 * 
 */

///////////////////////////////////////////////////////////////////////////////
// initial player configs
player myplayer = {.health = 0x09, .level = 0x01, .remaining_notes = 0x05};
///////////////////////////////////////////////////////////////////////////////
// initial flags
uint8_t is_game_over = 0, is_crossed = 0;
///////////////////////////////////////////////////////////////////////////////
// timer dependent variable configs
uint8_t timer0interrupted = 0, timer0counter = 0;
uint8_t timer0_interval =
    0x32; // set it as 0x32, 0X28 and 0x1e for levels 1, 2, and 3 respectively. 
unsigned short timer1_random_value = 0;
///////////////////////////////////////////////////////////////////////////////
// #of pushable notes
uint8_t pushable_notes = 0x05;
///////////////////////////////////////////////////////////////////////////////
// hex codes for 7-segment display characters
unsigned char seven_segment[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                                 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // numbers
unsigned char end_display[] = {0x79, 0x54, 0x5E};        // displaying "end"
unsigned char lose_display[] = {0x38, 0x5C, 0x6D, 0x79}; // displaying "lose"
///////////////////////////////////////////////////////////////////////////////
// individual lines and line array structure
line myline0, myline1, myline2, myline3, myline4;
line* all_lines[5] = {&myline0, &myline1, &myline2, &myline3, &myline4};
///////////////////////////////////////////////////////////////////////////////

void display();
void show_leds();

/////////////////////////////////////////////////////////////////////____START____
//____________initialize the ports_____________________________________________
void init_ports() {
    // set tri-states
    TRISA = 0xe0; // 0-4 are LEDs
    TRISB = 0xe0; // 0-4 are LEDs
    TRISC = 0xe1; // 0-4 are LEDs
    TRISD = 0xe0; // 0-4 are LEDs
    TRISE = 0xe0; // 0-4 are LEDs
    TRISF = 0xe0; // 0-4 are LEDs
    TRISG = 0x1f; // 0-4 are used to match the note the circle.
    TRISJ = 0x00; // seven segment display
    TRISH = 0xe0; // for disp's
    // reset ports
    PORTA = PORTB = PORTC = PORTD = PORTE = PORTF = PORTG = PORTH = 0X00;
}
///////////////////////////////////////////////////////////////////////____END____

/////////////////////////////////////////////////////////////////////____START____
//____________timers :: interrupts_____________________________________________
void init_timer0() {
    T0CON = 0x44; // 0100-0100
    INTCONbits.TMR0IF = 0x00;
    INTCONbits.TMR0IE = 0x01;
    INTCONbits.GIE = 0x01;
    // INTCONbits.PEIE = 0x01;
    T0CONbits.TMR0ON = 0x01;
    TMR0 = 6; // 100
}

void init_timer1() {
    // Set is as 16-bit timer
    T1CON = 0b11110000; // 0011-0000 
    TMR1 = 0xC8; // A random value to start
    // timer1 interrupt is enabled
    TMR1IE = 1 ;
    // Start timer1
    TMR1ON = 1;
}

uint8_t pressing = 0x00;
uint8_t portg_input = 0x00;

void Timer0interrupted();

void __interrupt() isr(void) {
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0x00;
        timer0interrupted = 0x01;
        TMR0 = 6;
        T0CON |= 0b111;
        T0PS3 = 0;
    }
    // Check for timer1 interrupt
    if (PIR1bits.TMR1IF) {
        TMR1 = 0xC8; // 200
        // Set timer1 interrupt bits
        TMR1IF = 0;
        TMR1IE = 1;
    }
}

line * timer1_helper() {
    // Son 3 bit BAK
    uint8_t result = timer1_random_value % 8;
//    uint8_t result = timer1_random_value & 0b111;
    result = result % 5;
    
    // RESULT =  % 5
    timer1_random_value = (timer1_random_value >> (2*myplayer.level-1))|(timer1_random_value << (16 - 2*myplayer.level-1));
    
    // SWITCH (RESULT)
    switch(result) {
        case 0:
            return &myline0;
        case 1:
            return &myline1;
        case 2:
            return &myline2;
        case 3:
            return &myline3;
        case 4:
            return &myline4;
    }
    // return line
}

void Timer0interrupted() {
    timer0interrupted = 0;
    timer0counter++;

    // 500 ms elapsed
    if (timer0counter % timer0_interval == 0) {
        // reset counter
        timer0counter = 0;
        is_crossed = 0;

        for (uint8_t i = 0; i < 5; i++) {
            if (advance_notes(all_lines[i])) {
                is_crossed = 1;
            }
        }
        if (is_crossed) {
            --myplayer.health;
            --myplayer.remaining_notes;
        }
        if (!myplayer.health) {
            game_over(0);
            return;
        }
        if (!myplayer.remaining_notes) {
            next_level();
            return;
        }
        if (pushable_notes > 0) {
            add_note(timer1_helper());
            --pushable_notes;
        }
        show_leds();
    }
} 

//////////////////////////////////////////////////////////////////////____END____

/////////////////////////////////////////////////////////////////////____START____
//____________level configs____________________________________________________
// called when all notes are finished in a level.
// begins next level or ends the game.
void next_level() {
    if (myplayer.level == 0x03) {
        // finished the game
        game_over(1);
    } else if (myplayer.level == 0x02) {
        // pass to level 3
        myplayer.level++;
        myplayer.remaining_notes = 15;
        timer0_interval = 0x1e;
        // set for line iteration and pushable notes
        pushable_notes = 0x0f;
        for(uint8_t i = 0; i < 5; i++) {
            all_lines[i]->begin = 0;
            all_lines[i]->end = 0;
            for (uint8_t i = 0; i < 15; i++) {
                all_lines[i]->notes[i].position = 0;
            }
        }
    } else {
        // pass to level 2
        myplayer.level++;
        myplayer.remaining_notes = 10;
        timer0_interval = 0x28;
        // set for line iteration and pushable notes
        pushable_notes = 0x0a;
        for(uint8_t i = 0; i < 5; i++) {
            all_lines[i]->begin = 0;
            all_lines[i]->end = 0;
            for (uint8_t i = 0; i < 15; i++) {
                all_lines[i]->notes[i].position = 0;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////____END____

/////////////////////////////////////////////////////////////////////____START____
//____________display___________________________________________________________
// adjusts the 7-segment display
// on game over => display End/Lose
// while playing => display level and health of the player
void display() {
    if (myplayer.level == 0x03 && myplayer.remaining_notes == 0x00) {
        // successful finish => display "End"
        // E
        PORTJ = end_display[2];
        PORTHbits.RH2 = 0x01;
        __delay_ms(5);
        PORTHbits.RH2 = 0x00;

        // n
        PORTJ = end_display[1];
        PORTHbits.RH1 = 0x01;
        __delay_ms(5);
        PORTHbits.RH1 = 0x00;

        // d
        PORTJ = end_display[0];
        PORTHbits.RH0 = 0x01;
        __delay_ms(5);
        PORTHbits.RH0 = 0x00;
    } else if (myplayer.health == 0) {
        // unsuccessfull finish => display LOSE
        // L
        PORTJ = lose_display[3];
        PORTHbits.RH3 = 0x01;
        __delay_ms(5);
        PORTHbits.RH3 = 0x00;

        // o
        PORTJ = lose_display[2];
        PORTHbits.RH2 = 0x01;
        __delay_ms(5);
        PORTHbits.RH2 = 0x00;

        // s
        PORTJ = lose_display[1];
        PORTHbits.RH1 = 0x01;
        __delay_ms(5);
        PORTHbits.RH1 = 0x00;

        // e
        PORTJ = lose_display[0];
        PORTHbits.RH0 = 0x01;
        __delay_ms(5);
        PORTHbits.RH0 = 0x00;
    } else {
        // display level and health
        PORTJ = seven_segment[myplayer.health];
        PORTHbits.RH0 = 0x01;
        __delay_ms(5);
        PORTHbits.RH0 = 0x00;

        PORTJ = seven_segment[myplayer.level];
        PORTHbits.RH3 = 0x01;
        __delay_ms(5);
        PORTHbits.RH3 = 0x00;
    }
}
///////////////////////////////////////////////////////////////////////____END____

/////////////////////////////////////////////////////////////////////____START____
//____________show leds________________________________________________________
// adjusts the leds according to the note flow
void show_leds() {
    LATA = LATB = LATC = LATD = LATE = LATF = 0x00;

    // set Line 0
    line_iterator line_it = begin(&myline0);
    while (is_valid(&line_it)) {
        switch (get_note(&line_it)->position) {
        case 0:
            LATAbits.LATA0 = 1;
            break;
        case 1:
            LATBbits.LATB0 = 1;
            break;
        case 2:
            LATCbits.LATC0 = 1;
            break;
        case 3:
            LATDbits.LATD0 = 1;
            break;
        case 4:
            LATEbits.LATE0 = 1;
            break;
        case 5:
            LATFbits.LATF0 = 1;
            break;
        }
        next(&line_it);
    }


    // set Line 1
    line_it = begin(&myline1);
    while (is_valid(&line_it)) {
        switch (get_note(&line_it)->position) {
        case 0:
            LATAbits.LATA1 = 1;
            break;
        case 1:
            LATBbits.LATB1 = 1;
            break;
        case 2:
            LATCbits.LATC1 = 1;
            break;
        case 3:
            LATDbits.LATD1 = 1;
            break;
        case 4:
            LATEbits.LATE1 = 1;
            break;
        case 5:
            LATFbits.LATF1 = 1;
            break;
        }
        next(&line_it);
    }

    // set Line 2
    line_it = begin(&myline2);
    while (is_valid(&line_it)) {
        switch (get_note(&line_it)->position) {
        case 0:
            LATAbits.LATA2 = 1;
            break;
        case 1:
            LATBbits.LATB2 = 1;
            break;
        case 2:
            LATCbits.LATC2 = 1;
            break;
        case 3:
            LATDbits.LATD2 = 1;
            break;
        case 4:
            LATEbits.LATE2 = 1;
            break;
        case 5:
            LATFbits.LATF2 = 1;
            break;
        }
        next(&line_it);
    }

    // set Line 3
    line_it = begin(&myline3);
    while (is_valid(&line_it)) {
        switch (get_note(&line_it)->position) {
        case 0:
            LATAbits.LATA3 = 1;
            break;
        case 1:
            LATBbits.LATB3 = 1;
            break;
        case 2:
            LATCbits.LATC3 = 1;
            break;
        case 3:
            LATDbits.LATD3 = 1;
            break;
        case 4:
            LATEbits.LATE3 = 1;
            break;
        case 5:
            LATFbits.LATF3 = 1;
            break;
        }
        next(&line_it);
    }

    // set Line 4
    line_it = begin(&myline4);
    while (is_valid(&line_it)) {
        switch (get_note(&line_it)->position) {
        case 0:
            LATAbits.LATA4 = 1;
            break;
        case 1:
            LATBbits.LATB4 = 1;
            break;
        case 2:
            LATCbits.LATC4 = 1;
            break;
        case 3:
            LATDbits.LATD4 = 1;
            break;
        case 4:
            LATEbits.LATE4 = 1;
            break;
        case 5:
            LATFbits.LATF4 = 1;
            break;
        }
        next(&line_it);
    }
}
///////////////////////////////////////////////////////////////////////____END____

/////////////////////////////////////////////////////////////////////____START____
//_____________Game Over________________________________________________________
// called on game over => is_success: 1 if player won, 0 otherwise
void game_over(uint8_t is_success) {
    is_game_over = 1;
    // Reset ports
    PORTA = PORTB = PORTC = PORTD = PORTE = PORTF = PORTG = PORTH = 0X00;
    // Turn off the timer0
    T0CONbits.TMR0ON = 0x00;
    // Reset all lines
    for(uint8_t i = 0; i < 5; i++) {
        all_lines[i]->begin = 0;
        all_lines[i]->end = 0;
        for (uint8_t i = 0; i < 15; i++) {
            all_lines[i]->notes[i].position = 0;
        }
    }
}
///////////////////////////////////////////////////////////////////////____END____

/////////////////////////////////////////////////////////////////////____START____
// starts by config setups
// wait till PORTC pressed-and-released
// round-robin loop for game state and interrupt adjustments
void main(void) {
    INTCON = 0x00;
    init_ports();
    // Initialize timer1 to create random number
    GIE=1;
    init_timer1();
    while (PORTC != 0x01)
        ;
    while (PORTC == 0x01)
        ;
    // Disable timer1 and save its value right after RC0 button released
    TMR1ON = 0;
    TMR1IE = 0;
    timer1_random_value = TMR1;
    GIE=0;
    // Set RC0 as output
    TRISCbits.RC0 = 0;
    // Initialize timer0
    init_timer0();
    // Round-robin game logic
    while (1) {
        if (!is_game_over) { // check if the game is over
            
            // display relevant information at each step of round-robin
            if (pressing) {
                if (PORTG == 0x00) {
                    // Reset RG flag
                    pressing = 0x00;
                    switch(portg_input) {
                        case 0b00000001:
                            button_clicked(&myplayer, &myline0);
                            break;
                        case 0b00000010:
                            button_clicked(&myplayer, &myline1);
                            break;
                        case 0b00000100:
                            button_clicked(&myplayer, &myline2);
                            break;
                        case 0b00001000:
                            button_clicked(&myplayer, &myline3);
                            break;
                        case 0b00010000:
                            button_clicked(&myplayer, &myline4);
                            break;
                    }
                } else;
            } else {
                if (PORTG != 0x00) {
                    pressing = 1;
                    portg_input = PORTG;
                } else;
            }
            
            if (timer0interrupted) {
                Timer0interrupted();
            }
            
            display();

        } else { // game over :: wait for RC0 input to restart
            TRISCbits.RC0 = 1;
            while (PORTC != 0x01) {
                display();
            }
            while (PORTC == 0x01) {
                display();
            }
            // Reset player info
            myplayer.health = 0x09;
            myplayer.remaining_notes = 0x05;
            myplayer.level = 0x01;
            pushable_notes = 5;
            // Reset game over flag to replay
            is_game_over = 0;
            // Reset ports
            init_ports();
            // Reset RC0 as output
            TRISCbits.RC0 = 0;
            // Reset timer0 related flags and bits
            timer0counter = 0;
            timer0interrupted = 0;
            TMR0 = 0;
            T0CON |= 0b111; 
            T0CONbits.TMR0ON = 0x01;
        }
    }
    return;
}
///////////////////////////////////////////////////////////////////////____END____
