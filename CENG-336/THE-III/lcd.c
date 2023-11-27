#include "Includes.h"
#include "lcd.h"

void display_seven_segment() {
    static uint8_t index = 0;
    switch(index) {
        case 0:
            // First number on seven segment
            PORTJ = SSEGMENT_NUMBERS_GLYPHS[num_defined_char];
            PORTHbits.RH0 = 0x01;
            break;
        case 1:
            PORTHbits.RH0 = 0x00;
            
            // Third number on seven segment
            PORTJ = SSEGMENT_NUMBERS_GLYPHS[cursor_x];
            PORTHbits.RH2 = 0x01;
            break;
        case 2:
            PORTHbits.RH2 = 0x00;
            
            // Fourth number on seven segment
            PORTJ = SSEGMENT_NUMBERS_GLYPHS[cursor_y];
            PORTHbits.RH3 = 0x01;
            break;
        case 3:
            PORTHbits.RH3 = 0x00;
            break;
    }
    index = (index + 1) % 4;
}

void Pulse(void){
    LCD_EN = 1;
    __delay_us(LCD_PULSE);
    LCD_EN = 0;
    __delay_us(LCD_PULSE);
}

void InitLCD(void) {
    __delay_ms(20);
    PORTD = 0x30;
    Pulse();
    
    __delay_ms(6);
    PORTD = 0x30;
    Pulse();
    
    __delay_us(300);
    PORTD = 0x30;
    Pulse();
    
    __delay_ms(2);
    PORTD = 0x20;
    Pulse();
    
    LcdCmd(0x02);
    LcdCmd(0x28);       // 4-bit mode - 2 line - 5x7 font.
    LcdCmd(0x0E);       // Display no cursor - no blink.
    LcdCmd(0x06);       // Automatic Increment - No Display shift.
    LcdCmd(0x80);       // Address DDRAM with 0 offset 80h.
}

void LcdCmd(uint8_t cmd){
    // Send 'cmd' as a command to the LCD.
    LCD_RS = 0;                         // It is a command
    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD |(cmd&0xF0);          // Write Upper nibble of data
    Pulse();                            // Give pulse on E pin
    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD | ((cmd<<4)&0xF0);    // Write Lower nibble of data
    Pulse();                            // Give pulse on E pin
}
void LcdData(uint8_t data){
    // Send 'data' as a data to the LCD.
    LCD_RS = 1;                         // It is data
    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD | (data&0xF0);        // Write Upper nibble of data
    Pulse();                            // Give pulse on E pin
    PORTD = PORTD & 0x0F;               // Make Data pins zero
    PORTD = PORTD | ((data<<4)& 0xF0);  // Write Lower nibble of data
    Pulse();                            // Give pulse on E pin
}

void CreateCustomCharacter (unsigned char *Pattern, const char Location){ 
    int i=0; 
    LcdCmd (0x40+(Location*8));
    for (i=0; i<8; i++) {
        LcdData(Pattern[i]);
    }
}