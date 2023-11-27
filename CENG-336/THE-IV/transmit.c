#include "transmit.h"

#include "define.h"

static char out_buffer[32];
static char *head, *tail;

static void transmit_reset(void) {
    head = out_buffer;
    tail = head;
}

void transmit_init() {
    transmit_reset();
}

void request_transmit(const char *s) {
    while(*s)
        *tail++ = *s++;
}

void transmit() {
    while(head != tail) {
        if(TXSTAbits.TXEN == 0) {
            TXREG = *head++;
            TXSTAbits.TXEN = 1;
        }
    }
    transmit_reset();
}

