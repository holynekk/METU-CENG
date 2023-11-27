#include "define.h"

#include <string.h>

/**********************************************************************
 * ----------------------- Local Variables ----------------------------
 **********************************************************************/


/**********************************************************************
 * ------------------------------ Task Hash ---------------------------
 * 
 * Computes the hash value when event gets fired 
 * 
 **********************************************************************/
static unsigned char inp[9];
static unsigned char out[17];

extern void compute_hash(unsigned char *inp, unsigned char *out);

TASK(HASH)
{
    while(1) {
        WaitEvent(HASH_EVENT);
        ClearEvent(HASH_EVENT);
        
        compute_hash(inp, out);
        
        request_transmit(out);
        
    }
    TerminateTask();
}

void alert(const char* str) {
    strcpy(inp, str);
    SetEvent(HASH_ID, HASH_EVENT);
}

/* End of File : tsk_task0.c */
