
/************************************************************
 * File Name    : delay.c
 * Description  : Provides software-based delay functions
 *                to generate microsecond, millisecond and
 *                second delays.
 *
 * Controller   : LPC2148
 * Delay Type   : Software Delay
 *
 * Author       : Embedded Systems Project
 ************************************************************/

#include "types.h"


/************************************************************
 * Function Name : delay_us
 * Description   : Generates a software delay in microseconds
 *                 using a decrementing loop.
 *
 * Parameters    : dlyUS - Delay duration in microseconds
 * Return        : None
 ************************************************************/
void delay_us(u32 dlyUS)
{
    // Generate approximate microsecond delay
    for (dlyUS *= 12; dlyUS > 0; dlyUS--);
}


/************************************************************
 * Function Name : delay_ms
 * Description   : Generates a software delay in milliseconds
 *                 using a decrementing loop.
 *
 * Parameters    : dlyMS - Delay duration in milliseconds
 * Return        : None
 ************************************************************/
void delay_ms(u32 dlyMS)
{
    // Generate approximate millisecond delay
    for (dlyMS *= 12000; dlyMS > 0; dlyMS--);
}


/************************************************************
 * Function Name : delay_s
 * Description   : Generates a software delay in seconds
 *                 using a decrementing loop.
 *
 * Parameters    : dlyS - Delay duration in seconds
 * Return        : None
 ************************************************************/
void delay_s(u32 dlyS)
{
    // Generate approximate second delay
    for (dlyS *= 12000000; dlyS > 0; dlyS--);
}
