/*
 * A/D Conversion Module Helper
 */

#ifndef AD_H
#define	AD_H

#include <stdint.h>

/*
 * Initializes the a/d conversion module.
 * Uses Timer1.
 */
void ad_init(void);

/*
 * Interrupt service routine for ADIF_bit.
 */
void ad_isr_adif(void);

/*
 * Interrupt service routine for TMR2IF_bit.
 */
void ad_isr_tmr2if(void);

#endif	/* AD_H */

