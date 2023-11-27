#ifndef _DEFINE_H
#define _DEFINE_H

#include "device.h"

#define _XTAL_FREQ   40000000

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
#define _10MHZ	63320
#define _16MHZ	61768
#define _20MHZ	60768
#define _32MHZ	57768
#define _40MHZ 	55768

/***********************************************************************
 * ----------------------------- Events --------------------------------
 **********************************************************************/
#define ALARM_EVENT       0x80
#define LCD_EVENT         0x01
#define HASH_EVENT        0x10

/***********************************************************************
 * ----------------------------- Alarms --------------------------------
 **********************************************************************/
#define ALARM_TSK0          0       /* Alarm ID (index) in tascdesc.c */
#define LCD_ALARM_ID        1       /* Alarm ID (index) in tascdesc.c */
#define HASH_ALARM_ID       2       /* Alarm ID (index) in tascdesc.c */

/***********************************************************************
 * ---------------------- Task ID & Priorities -------------------------
 **********************************************************************/
#define TASK0_ID            1
#define LCD_ID              5
#define HASH_ID            11

// We didn't select different priority values for different tasks since
// tasks mostly depends on events to resume.
#define TASK0_PRIO          10
#define LCD_PRIO            10
#define HASH_PRIO           10

#endif /* _DEFINE_H */
