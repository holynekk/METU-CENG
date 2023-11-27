#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "LCD.h"
#include "transmit.h"
#include "pet.h"

/**********************************************************************
 * ----------------------- Local Variables ----------------------------
 **********************************************************************/

/**********************************************************************
 * ------------------------------ TASK0 -------------------------------
 * 
 * Writes various strings to LCD 
 * 
 **********************************************************************/

static unsigned char all_LCD[4][COLUMN] = {"MONEY:      1000", "HUNGER          ", "HAPPY:          ", "THIRST          "};
static int lcd_mode;

int portBpressed;

void update_lcd(void) {
    int i, k;
    char *money_str;
    int money_str_len;
    pet *p = get_pet();
    // Clear all 3 lines for hunger, thirst, and happiness
    for(i = 6; i < 16; i++) {
        all_LCD[0][i] = ' ';
        all_LCD[1][i] = ' ';
        all_LCD[2][i] = ' ';
        all_LCD[3][i] = ' ';
    }
    // Put squares wrt values into corresponding places
    for(i = 6; i < 6+(p->hunger/10); i++) {
        all_LCD[1][i] = 0xFF;
    }
    for(i = 6; i < 6+(p->happiness/10); i++) {
        all_LCD[2][i] = 0xFF;
    }
    for(i = 6; i < 6+(p->thirst/10); i++) {
        all_LCD[3][i] = 0xFF;
    }
    // Updates money line on LCD
    itoa(p->money, money_str);
    money_str_len = strlen(money_str);
    for(i = 0; i < money_str_len; ++i)
        all_LCD[0][15-i] = money_str[money_str_len-1-i];
    LcdPrintString(all_LCD[0], all_LCD[1+lcd_mode]);
}

TASK(TASK0)
{
    // Trying to send command per 50ms time interval
    SetRelAlarm(ALARM_TSK0, 10, 50);
    portBpressed = 0;
    lcd_mode = 0;
    while(1){
        WaitEvent(ALARM_EVENT);
        ClearEvent(ALARM_EVENT);
        transmit();
        // Waits for RB1 button to be released
        if(portBpressed && !PORTBbits.RB1) {
            portBpressed = 0 ;
            lcd_mode = ++lcd_mode % 3;
            update_lcd();
        }
    }
    TerminateTask();
}

/* End of File : tsk_task0.c */
