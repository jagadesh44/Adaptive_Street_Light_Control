
/************************************************************
 * File Name    : rtc.c
 * Description  : Initializes the internal RTC of LPC2148
 *                and continuously displays time and date
 *                on a 16x2 LCD.
 *
 * Controller   : LPC2148
 * Peripheral   : Internal RTC
 * Display      : 16x2 LCD
 *
 * Author       : Embedded Systems Project
 ************************************************************/

#include <LPC21xx.h>
#include "lcd.h"
#include "kpm.h"
#include "defines.h"
#include "lcd_defines.h"
#include "delay.h"


/************************************************************
 * Function Name : RTC
 * Description   : Initializes the internal RTC, sets the
 *                 initial time and date, and continuously
 *                 displays the updated values on the LCD.
 *
 * Parameters    : None
 * Return        : None
 ************************************************************/
void RTC(void)
{
    // Reset the RTC counter and prescaler
    CCR = (1 << 1);

    // Enable RTC and select RTC clock source
    CCR = (1 << 0) | (1 << 4);

    // Set initial time
    HOUR = 13;
    MIN  = 45;
    SEC  = 0;

    // Set initial date
    YEAR  = 2026;
    MONTH = 11;
    DOM   = 22;

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("HH:MM:SS");                 // Display time format

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("DD/MM/YYYY");               // Display date format

    while (1)
    {
        CmdLCD(CLEAR_LCD);              // Clear previous display

        CmdLCD(GOTO_LINE1_POS0);

        // Display hours in HH format
        CharLCD(HOUR / 10 + '0');       // Display tens digit
        CharLCD(HOUR % 10 + '0');       // Display units digit

        CharLCD(':');

        // Display minutes in MM format
        CharLCD(MIN / 10 + '0');        // Display tens digit
        CharLCD(MIN % 10 + '0');        // Display units digit

        CharLCD(':');

        // Display seconds in SS format
        CharLCD(SEC / 10 + '0');        // Display tens digit
        CharLCD(SEC % 10 + '0');        // Display units digit

        CmdLCD(GOTO_LINE2_POS0);

        // Display date in DD/MM/YYYY format
        CharLCD(DOM / 10 + '0');        // Display date tens digit
        CharLCD(DOM % 10 + '0');        // Display date units digit

        CharLCD('/');

        // Display month in MM format
        CharLCD(MONTH / 10 + '0');      // Display month tens digit
        CharLCD(MONTH % 10 + '0');      // Display month units digit

        CharLCD('/');

        U32LCD(YEAR);                   // Display four-digit year
    }
}
