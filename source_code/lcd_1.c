
/************************************************************
 * File Name    : lcd.c
 * Description  : Contains functions to initialize and control
 *                a 16x2 LCD in 8-bit mode using LPC2148.
 *
 * Controller   : LPC2148
 * Peripheral   : 16x2 LCD
 * Interface    : 8-bit parallel interface
 *
 * Author       : Embedded Systems Project
 ************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "defines.h"
#include "lcd_defines.h"
#include "delay.h"


/************************************************************
 * Function Name : writelcd
 * Description   : Writes an 8-bit command or data value
 *                 to the LCD.
 *
 * Parameters    : dat - 8-bit value to be written
 * Return        : None
 ************************************************************/
void writelcd(u8 dat)
{
    WRITEBYTE(IOPIN0, LCD_DATA, dat);  // Write data to LCD data pins

    IOCLR0 = 1 << LCD_RW;              // Select LCD write operation

    IOSET0 = 1 << LCD_EN;               // Enable LCD

    delay_us(1);                        // Provide enable pulse width

    IOCLR0 = 1 << LCD_EN;               // Disable LCD

    delay_ms(2);                        // Allow LCD to process the data
}


/************************************************************
 * Function Name : CmdLCD
 * Description   : Sends a command instruction to the LCD.
 *
 * Parameters    : cmd - LCD command byte
 * Return        : None
 ************************************************************/
void CmdLCD(u8 cmd)
{
    IOCLR0 = 1 << LCD_RS;               // Select command register

    writelcd(cmd);                      // Send command to LCD
}


/************************************************************
 * Function Name : CharLCD
 * Description   : Displays a single character on the LCD.
 *
 * Parameters    : asciival - ASCII value of the character
 * Return        : None
 ************************************************************/
void CharLCD(u8 asciival)
{
    IOSET0 = 1 << LCD_RS;               // Select data register

    writelcd(asciival);                 // Send character to LCD
}


/************************************************************
 * Function Name : Initlcd
 * Description   : Configures GPIO pins and initializes the
 *                 LCD in 8-bit, 2-line display mode.
 *
 * Parameters    : None
 * Return        : None
 ************************************************************/
void Initlcd(void)
{
    // Configure LCD data, RS, RW and EN pins as GPIO outputs
    IODIR0 = ((0xFF << LCD_DATA) |
              (1 << LCD_RS) |
              (1 << LCD_RW) |
              (1 << LCD_EN));

    delay_ms(15);                       // Wait for LCD power-on initialization

    // Initialize LCD using the required startup sequence
    CmdLCD(0x30);
    delay_ms(4);

    delay_us(100);
    CmdLCD(0x30);

    delay_us(100);
    CmdLCD(0x30);

    CmdLCD(MODE_8BIT_2LINE);            // Select 8-bit, 2-line mode
    CmdLCD(DSP_ON_CUR_OFF);              // Display ON, cursor OFF
    CmdLCD(CLEAR_LCD);                   // Clear LCD display
    CmdLCD(SHIFT_CUR_RIGHT);             // Set cursor movement direction
}


/************************************************************
 * Function Name : StrLCD
 * Description   : Displays a null-terminated string on LCD.
 *
 * Parameters    : s - Pointer to the input string
 * Return        : None
 ************************************************************/
void StrLCD(s8 *s)
{
    // Display characters until the null terminator is reached
    while (*s)
    {
        CharLCD(*s++);
    }
}


/************************************************************
 * Function Name : U32LCD
 * Description   : Converts and displays an unsigned 32-bit
 *                 integer on the LCD.
 *
 * Parameters    : n - Unsigned integer value
 * Return        : None
 ************************************************************/
void U32LCD(u32 n)
{
    u8 a[10];
    s32 i = 0;

    if (n == 0)
    {
        CharLCD('0');                   // Display zero
    }
    else
    {
        while (n > 0)
        {
            a[i++] = (n % 10) + 48;      // Extract digit and convert to ASCII

            n /= 10;                    // Remove the last digit

            i++;                        // Increment array index
        }

        // Display extracted digits in reverse order
        for (--i; i >= 0; i--)
        {
            CharLCD(a[i]);
        }
    }
}


/************************************************************
 * Function Name : S32lcd
 * Description   : Displays a signed 32-bit integer on LCD.
 *
 * Parameters    : sn - Signed integer value
 * Return        : None
 ************************************************************/
void S32lcd(s32 sn)
{
    if (sn < 0)
    {
        CharLCD('-');                   // Display negative sign

        sn = -sn;                       // Convert to positive value
    }

    U32LCD(sn);                         // Display numeric value
}


/************************************************************
 * Function Name : F32lcd
 * Description   : Displays a floating-point number with the
 *                 specified number of decimal places.
 *
 * Parameters    : fN  - Floating-point value
 *                 nDP - Number of decimal places
 * Return        : None
 ************************************************************/
void F32lcd(f32 fN, u8 nDP)
{
    u32 n, i;

    if (fN < 0.0)
    {
        CharLCD('-');                   // Display negative sign

        fN = -fN;                       // Convert to positive value
    }

    n = fN;                             // Extract integer part

    U32LCD(n);                          // Display integer part

    CharLCD('.');                       // Display decimal point

    for (i = 0; i < nDP; i++)
    {
        fN = (fN - n) * 10;              // Extract next decimal digit

        n = fN;                         // Store extracted digit

        CharLCD(n + 48);                // Convert digit to ASCII and display
    }
}


/************************************************************
 * Function Name : BUILDCGRAM
 * Description   : Creates custom characters by writing
 *                 pattern bytes into LCD CGRAM.
 *
 * Parameters    : p      - Pointer to character pattern data
 *                 nBytes - Number of pattern bytes
 * Return        : None
 ************************************************************/
void BUILDCGRAM(u8 *p, u8 nBytes)
{
    u32 i;

    CmdLCD(GOTO_CGRAM_START);           // Select CGRAM starting address

    // Write custom character pattern bytes into CGRAM
    for (i = 0; i < nBytes; i++)
    {
        CharLCD(p[i]);
    }

    CmdLCD(GOTO_LINE1_POS0);            // Return cursor to first LCD line
}
