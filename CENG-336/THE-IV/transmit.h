#ifndef TRANSMIT_H
#define	TRANSMIT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*
 * Initialize transmitter.
 */
void transmit_init(void);

/*
 * Send null-terminated string.
 */
void request_transmit(const char *s);

/*
 * Do the transmit job.
 */
void transmit(void);


#ifdef	__cplusplus
}
#endif

#endif	/* TRANSMIT_H */
