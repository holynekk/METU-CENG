#include "ad.h"
#include "lcd.h"
#include "Includes.h"
#include <xc.h>

// Enum that's going to be used while considering which mode we are in
enum mode {
    MODE_TEXT_ENTRY,
    MODE_CHARACTER_DEFINE,
    MODE_SCROLL
};

// Direction enum that's going to be used to consider which way we want to move our cursor in move_cursor function
enum direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Global Varibles -------------------------------------------------------
// Indexing and button pressing related variables
uint8_t cursor_index;
uint8_t desired_cursor_index;
uint8_t mode = MODE_TEXT_ENTRY;
uint8_t should_display_7seg = 0;
uint8_t should_scroll_text = 0;
uint8_t at_predef = 0, at_custom = 0;
uint8_t pressing_re0 = 0,
        pressing_re1 = 0,
        pressing_re2 = 0,
        pressing_re3 = 0,
        pressing_re4 = 0,
        pressing_re5 = 0;
// Custom char variable that 
uint8_t user_defined_charmap[8];
// LCD lines that are going to be used to write data on LCD screen in the main loop
uint8_t first_line[16] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
uint8_t second_line[16] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
// The text that's going to be shown in scroll mode at the first line
uint8_t finished_text[16] = {0x20, 0x20, 0x20, 
                            0x66, 0x69, 0x6e, 0x69, 0x73, 0x68, 0x65, 0x64, 
                            0x20, 0x20, 0x20, 0x20, 0x20};
// Timer0 related variables
uint8_t timer0counter = 0, timer0_interval = 0x49;
// Cursor pointers to display data on LCD
uint8_t custom_p = 0, predefined_p = 0, adc_cursor = 0;
// ---------------------------------------------------------------------

// Interrupt Service Routine function
void __interrupt(high_priority) isr(void) {
    // Check ADC interruption
    if(ADIF) {
        ad_isr_adif();
    }
    // Check timer2 interruption
    if(TMR2IF) {
        ad_isr_tmr2if();
    }
    // Check timer0 interruption
    if (INTCONbits.TMR0IF) {
        // Reset interrupt flag
        INTCONbits.TMR0IF = 0x00;
        timer0counter++;
        should_display_7seg = 1;
        // if 400 ms elapsed, shift second line array to get the scroll effect
        if (timer0counter >= timer0_interval) {
            should_scroll_text = 1;
            timer0counter = 0;
        }
        // Reset timer0 bits to make it properly
        TMR0 = 6;
        T0CON |= 0b111;
        T0PS3 = 0;
    }
}

// The function that generates binary number from given port values
uint8_t charMapRowGenerator(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
    uint8_t result = 0;
    result = a * 16 + b * 8 + c * 4 + d * 2;
    return result;
}

// The function that generates custom character from PORT[A/B/C/D] values
void charMapArrayGenerator(){
    user_defined_charmap[7] = charMapRowGenerator(PORTAbits.RA7, PORTBbits.RB7, PORTCbits.RC7, PORTDbits.RD7);
    user_defined_charmap[6] = charMapRowGenerator(PORTAbits.RA6, PORTBbits.RB6, PORTCbits.RC6, PORTDbits.RD6);
    user_defined_charmap[5] = charMapRowGenerator(PORTAbits.RA5, PORTBbits.RB5, PORTCbits.RC5, PORTDbits.RD5);
    user_defined_charmap[4] = charMapRowGenerator(PORTAbits.RA4, PORTBbits.RB4, PORTCbits.RC4, PORTDbits.RD4);
    user_defined_charmap[3] = charMapRowGenerator(PORTAbits.RA3, PORTBbits.RB3, PORTCbits.RC3, PORTDbits.RD3);
    user_defined_charmap[2] = charMapRowGenerator(PORTAbits.RA2, PORTBbits.RB2, PORTCbits.RC2, PORTDbits.RD2);
    user_defined_charmap[1] = charMapRowGenerator(PORTAbits.RA1, PORTBbits.RB1, PORTCbits.RC1, PORTDbits.RD1);
    user_defined_charmap[0] = charMapRowGenerator(PORTAbits.RA0, PORTBbits.RB0, PORTCbits.RC0, PORTDbits.RD0);
}

// The function that changes a specific port bit at cursor position
void toggle(uint8_t x, uint8_t y){
    // Specify the cursor position on y-axis
    uint8_t temp = 0b1;
    temp = temp << y;
    // Select port and then toggle the bit that is pointed with temp variable
    switch(x){
        // PORTA
        case 0:
            PORTA ^= temp;
            break;
        // PORTB
        case 1: 
            PORTB ^= temp;
            break;
        // PORTC
        case 2: 
            PORTC ^= temp;
            break;
        // PORTA
        case 3: 
            PORTD ^= temp;
            break;
    }
}

// The function that moves cursor on PORT[A/B/C/D] with the given direction
void move_cursor(enum direction d) {
    // Move cursor --up--
    if (d == UP) {
        cursor_y = cursor_y - 1 >= 0 ? cursor_y - 1 : cursor_y;
    }
    // Move cursor --down--
    else if (d == DOWN) {
        cursor_y = cursor_y + 1 <= 7 ? cursor_y + 1 : cursor_y;
    }
    // Move cursor --left--
    else if (d == LEFT) {
        cursor_x = cursor_x - 1 >= 0 ? cursor_x - 1 : cursor_x;
    }
    // Move cursor --right--
    else {
        cursor_x = cursor_x + 1 <= 3 ? cursor_x + 1 : cursor_x;
    }
}

// Timer0 initializer
void init_timer0() {
    T0CON = 0b01000011;
    INTCONbits.TMR0IF = 0x00;
    INTCONbits.TMR0IE = 0x01;
    TMR0 = 6; // 100
    T0CONbits.TMR0ON = 0x01;
}

void init(void) {
    // For LCD and LEDs
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    // For buttons
    TRISE = 0b00111111;
    // For seven-segment display
    TRISJ = 0x00;
    TRISH = 0b00010000;
    // Reset ports
    PORTA= PORTB = PORTC = PORTD = PORTE = PORTH = PORTJ = 0X00;
    // Initialize adc 
    ad_init();
    // Initialize timer0 
    init_timer0();
    // Initialize LCD 
    InitLCD();
    // Set global interrupt enable as 1
    GIE = 1;
}

/*
 * The overall description of the program:
 * 1- The program starts by initializing ports, adc, lcd, and timer0
 * 2- After initialization completed, program goes into round-robin loop
 * 3- In round robin loop:
 *    3.1- It refreshes the seven segment display
 *    3.2- After refreshing seven segment display, it goes into switch statement
 *    where it will act according to its mode (MODE_TEXT_ENTRY, MODE_CHARACTER_DEFINE, or MODE_SCROLL)
 *    3.3- MODE_TEXT_ENTRY:
 *        3.3.1- Every time program goes in MODE_TEXT_ENTRY, it places the cursor at the desired place
 *        3.3.2- Checks if user pressed re4. If that's the case mode changes to MODE_CHARACTER_DEFINE, if not
 *        stay in the same mode
 *        3.3.3- Checks if user pressed re5. If that's the case, mode changes to MODE_SCROLL, if not stay
 *        in the same mode. When changing mode to MODE_SCROLL, program passes and the finished_text into
 *        first_line, and the content of first_line into second_line.
 *        3.3.4- If there is no action with re4 and re5 buttons, program needs to check r0, r1, r2, and r3
 *        buttons to cycle through custom and predefined characters at the cursor's place on lcd. r0 and r3
 *        are the buttons to cycle through the defined custom characters. r1 and r2 are the buttons to cycle
 *        through the predefined characters.
 *    3.4- MODE_CHARACTER_DEFINE:
 *        3.4.1- At the very beginning programs checks if re5 buttons is pressed. If it's pressed defined
 *        custom characters on leds are stored in CGRAM at a proper place, 
 *        3.4.2- If there is no interaction with re5 button, program checks re[0-4] press/release states. If re4
 *        is pressed then the led should be toggled where invisible cursor points to. The other button cases are
 *        used to move invisible cursor on leds.
 *    3.5- MODE_SCROLL:
 *        3.5.1- Scrolling event occurs after some constant time. With the help of Timer0Interrupted function should_scroll_text
 *        is being set as 1 and shifting occurs on second_line.
 *        3.5.2- To show changes on lcd, program rewrites second line into lcd to commit changes.
*/

void main(void) {
    // Initialize everything related with ports, adc, timers, and lcd
    init();
    // Main round-robin loop
    while(1){
        // Make seven segment change its state to show 3 different numbers 
        if (should_display_7seg) {
            should_display_7seg = 0;
            display_seven_segment();
        }
        // Switch case between MODE_TEXT_ENTRY - MODE_CHARACTER_DEFINE - MODE_SCROLL
        switch(mode) {
            // Enter Text Entry Mode (default)
            case MODE_TEXT_ENTRY:
                // Place the cursor at the desired place
                if(cursor_index != desired_cursor_index){
                    cursor_index = desired_cursor_index;
                    LcdCmd(0x80 + cursor_index);
                }
                // Check if re4 is pressed and released --------
                if (pressing_re4) {
                    // Check if the button is released
                    if (PORTE != 0b00010000) {
                        pressing_re4 = 0x00;
                        // Set the mode as Character Define Mode
                        mode = MODE_CHARACTER_DEFINE;
                        LATD = 0;
                    } else {
                        continue;
                    }
                } else {
                    // Check if the button is pressed
                    if (PORTE == 0b00010000) {
                        pressing_re4 = 1;
                    } else;
                }
                // ---------------------------------------------

                // Check if re5 is pressed and released --------
                if (pressing_re5) {
                    // Check if the button is released
                    if (PORTE != 0b00100000) {
                        pressing_re5 = 0x00;
                        // Set the mode as Text Scroll Mode
                        mode = MODE_SCROLL;
                        timer0counter = 0;
                        LcdCmd(0x0C);
                        // Update the lcd just incase there are changes but no timer0 interruption
                        for(int i = 0; i < 16; i++){
                            LcdCmd(0x80 + i);
                            LcdData(first_line[i]);
                        }
                        for(int i = 0; i < 16; i++){
                            LcdCmd(0xc0 + i);
                            LcdData(second_line[i]);
                        }
                    } else {
                        continue;
                    }
                } else {
                    // Check if the button is pressed
                    if (PORTE == 0b00100000) {
                        pressing_re5 = 1;
                        // Before changing mode to 2, copy first_line content into second_line
                        for (int i = 0; i < 16; i++) {
                            second_line[i] = first_line[i];
                        }
                        // Then write "finished" on the first_line
                        for (int i = 0; i < 16; i++) {
                            first_line[i] = finished_text[i];
                        }
                    } else;
                }
                // ---------------------------------------------
                // Continue if there is no operation related re4 or re5 buttons
                if (!pressing_re4 && !pressing_re5) {
                    // RE0 (forward) and RE3 buttons will cycle through the defined custom characters.
                    // Check if there is any custom characters. If not skip checking re0 and re3 buttons
                    if (num_defined_char) {
                        // Check if re0 is pressed and released --------
                        if (pressing_re0) {
                            if (PORTE != 0b00000001) {
                                pressing_re0 = 0x00;
                            } else {
                                continue;
                            }
                        } else {
                            // Check if the button is pressed
                            if (PORTE == 0b00000001) {
                                pressing_re0 = 1;
                                // Cycle custom_p forward and change the content where lcd cursor points to
                                at_custom = 1;
                                // Check if there is a change in cycle method (cycle through predefined chars or cycle through user defined chars)
                                if (at_predef) {
                                    at_predef = 0;
                                    custom_p = 0;
                                } else {
                                    custom_p = custom_p + 1 <= num_defined_char-1 ? custom_p + 1 : 0;
                                }
                                // Update lcd at the position of cursor
                                first_line[cursor_index] = custom_p;
                                LcdData(first_line[cursor_index]);
                                LcdCmd(0x80 + cursor_index);
                                // Reset predefined_p incase it's different than 0
                                predefined_p = -1;
                            } else;
                        }
                        // ---------------------------------------------
                        // Check if re3 is pressed and released --------
                        if (pressing_re3) {
                            if (PORTE != 0b00001000) {
                                // Reset RE3 flag
                                pressing_re3 = 0x00;
                            } else {
                                continue;
                            }
                        } else {
                            // Check if the button is pressed
                            if (PORTE == 0b00001000) {
                                pressing_re3 = 1;
                                // Cycle custom_p backward and change the content where lcd cursor points to
                                at_custom = 1;
                                // Check if there is a change in cycle method (cycle through predefined chars or cycle through user defined chars)
                                if (at_predef) {
                                    at_predef = 0;
                                    custom_p = 0;
                                } else {
                                    custom_p = custom_p - 1 >= 0 ? custom_p - 1 : num_defined_char-1;
                                }
                                // Update lcd at the position of cursor
                                first_line[cursor_index] = custom_p;
                                LcdData(first_line[cursor_index]);
                                LcdCmd(0x80 + cursor_index);
                                // Reset predefined_p incase it's different than 0
                                predefined_p = 1;
                            } else;
                        }
                        // ---------------------------------------------
                    } else;
                    // RE2 (forward) and RE1 buttons will cycle the displayed character through the set of predefined characters
                    // Check if re2 is pressed and released --------
                    if (pressing_re2) {
                        if (PORTE != 0b00000100) {
                            // Reset RE2 flag
                            pressing_re2 = 0x00;
                        } else {
                            continue;
                        }
                    } else {
                        // Check if the button is pressed
                        if (PORTE == 0b00000100) {
                            pressing_re2 = 1;
                            // Cycle predefined_p forward and change the content where lcd cursor points to
                            at_predef = 1;
                            // Check if there is a change in cycle method (cycle through predefined chars or cycle through user defined chars)
                            if (at_custom) {
                                at_custom = 0;
                                predefined_p = 0;
                            } else {
                                predefined_p = predefined_p + 1 <= 36 ? predefined_p + 1 : 0;
                            }
                            // Update lcd at the position of cursor
                            first_line[cursor_index] = PREDEFINED[predefined_p];
                            LcdData(first_line[cursor_index]);
                            LcdCmd(0x80 + cursor_index);
                            // Reset custom_p incase it's different than 0
                            custom_p = -1;
                        } else;
                    }
                    // ---------------------------------------------
                    // Check if re1 is pressed and released --------
                    if (pressing_re1) {
                        if (PORTE != 0b00000010) {
                            // Reset RE1 flag
                            pressing_re1 = 0x00;
                        } else {
                            continue;
                        }
                    } else {
                        // Check if the button is pressed
                        if (PORTE == 0b00000010) {
                            pressing_re1 = 1;
                            // Cycle predefined_p backward and change the content where lcd cursor points to
                            at_predef = 1;
                            // Check if there is a change in cycle method (cycle through predefined chars or cycle through user defined chars)
                            if (at_custom) {
                                at_custom = 0;
                                predefined_p = 0;
                            } else {
                                predefined_p = predefined_p - 1 >= 0 ? predefined_p - 1 : 36;
                            }
                            // Update lcd at the position of cursor
                            first_line[cursor_index] = PREDEFINED[predefined_p];
                            LcdData(first_line[cursor_index]);
                            LcdCmd(0x80 + cursor_index);
                            // Reset custom_p incase it's different than 0
                            custom_p = 1;
                        } else;
                    }
                    // ---------------------------------------------
                }
                break;
            case MODE_CHARACTER_DEFINE:
                // Check if re5 is pressed and released --------
                if (pressing_re5) {
                    if (PORTE != 0b00100000) {
                        pressing_re5 = 0x00;
                        // Go back to Text Entry Mode
                        mode = MODE_TEXT_ENTRY;
                    } else {
                        continue;
                    }
                } else {
                    // Check if the button is pressed
                    if (PORTE == 0b00100000) {
                        pressing_re5 = 1;
                        // Generate charmap for custom character
                        charMapArrayGenerator();
                        CreateCustomCharacter(user_defined_charmap, num_defined_char);
                        // Put custom created character into lcd immediately after pressing re5
                        first_line[cursor_index] = num_defined_char;
                        LcdCmd(0x80 + cursor_index);
                        LcdData(first_line[cursor_index]);
                        LcdCmd(0x80 + cursor_index);
                        // Increase the number of custom characters to show it properly on seveng segment display
                        ++num_defined_char;
                        // Reset used PORTS and cursor pointers
                        cursor_x = cursor_y = 0;
                        PORTA = PORTB = PORTC = PORTD = 0x00;
                    } else;
                }
                // ---------------------------------------------
                // Check if re4 is pressed ---------------------
                if (pressing_re4) {
                    if (PORTE != 0b00010000) {
                        // Reset RE4 flag
                        pressing_re4 = 0x00;
                        // Call toggle function to light on/off that led at (cursor_x, cursor_y)
                        toggle(cursor_x, cursor_y);
                    } else {
                        continue;
                    }
                } else {
                    // Check if the button is pressed
                    if (PORTE == 0b00010000) {
                        pressing_re4 = 1;
                    }
                    // If not check other buttons that manipulates cursor pointer variables
                    // This block basicaly changes invisible cursor position on leds
                    else {
                        // Move cursor --RIGHT--
                        if (pressing_re0) {
                            if (PORTE != 0b00000001) {
                                // Reset RE0 flag
                                pressing_re0 = 0x00;
                            } else {
                                continue;
                            }
                        } else {
                            if (PORTE == 0b00000001) {
                                pressing_re0 = 1;
                                move_cursor(RIGHT);
                            } else;
                        }
                        // Move cursor --LEFT--
                        if (pressing_re3) {
                            if (PORTE != 0b00001000) {
                                // Reset RE3 flag
                                pressing_re3 = 0x00;
                            } else {
                                continue;
                            }
                        } else {
                            if (PORTE == 0b00001000) {
                                pressing_re3 = 1;
                                move_cursor(LEFT);
                            } else;
                        }
                        // Move cursor --UP--
                        if (pressing_re2) {
                            if (PORTE != 0b00000100) {
                                // Reset RE2 flag
                                pressing_re2 = 0x00;
                            } else {
                                continue;
                            }
                        } else {
                            if (PORTE == 0b00000100) {
                                pressing_re2 = 1;
                                move_cursor(UP);
                            } else;
                        }
                        // Move cursor --DOWN--
                        if (pressing_re1) {
                            if (PORTE != 0b00000010) {
                                // Reset RE1 flag
                                pressing_re1 = 0x00;
                            } else {
                                continue;
                            }
                        } else {
                            if (PORTE == 0b00000010) {
                                pressing_re1 = 1;
                                move_cursor(DOWN);
                            } else;
                        }
                        // -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    }
                }
                // ---------------------------------------------
                break;
            // The program's state that shows "finished" text at the first line
            // and shifts second line repeatedly
            case MODE_SCROLL:
                // Check if there is timer0 interruption
                if(should_scroll_text){
                    // Reset flag
                    should_scroll_text = 0;
                    // Shift while second line left for once
                    uint8_t first_lcd_char = second_line[0];
                    for(int i = 0; i < 15; i++) {
                        second_line[i] = second_line[i+1];
                    }
                    second_line[15] = first_lcd_char;
                    // Rewrite second line into lcd to commit changes
                    for(int i = 0; i < 16; i++){
                        LcdCmd(0xc0 + i);
                        LcdData(second_line[i]);
                    }
                }
                break;
        }
    }
}
