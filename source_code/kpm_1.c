
/************************************************************
 * File Name    : kpm.c
 * Description  : Contains functions to initialize, scan and
 *                interface a 4x4 matrix keypad with LPC2148.
 *
 * Controller   : LPC2148
 * Peripheral   : 4x4 Matrix Keypad
 * Interface    : GPIO
 *
 * Author       : Embedded Systems Project
 ************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "defines.h"
#include "kpm_defines.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "delay.h"


/************************************************************
 * Keypad Lookup Table
 * Description : Stores the character corresponding to each
 *               row and column position of the keypad.
 ************************************************************/
u8 kpmLUT[4][4] =
{
    {'1', '2', '3', 'a'},
    {'4', '5', '6', 'b'},
    {'7', '8', '9', 'c'},
    {'*', '0', '#', 'd'}
};


/************************************************************
 * Function Name : Initkpm
 * Description   : Configures the keypad row pins as outputs.
 *                 Column pins are configured as inputs.
 *
 * Parameters    : None
 * Return        : None
 ************************************************************/
void Initkpm(void)
{
    // Configure all four keypad row pins as GPIO outputs
    SETBIT(IODIR1, ROW0);
    SETBIT(IODIR1, ROW1);
    SETBIT(IODIR1, ROW2);
    SETBIT(IODIR1, ROW3);

    // Column pins remain configured as inputs
}


/************************************************************
 * Function Name : colscan
 * Description   : Checks whether any key is pressed by
 *                 monitoring all four column input pins.
 *
 * Parameters    : None
 * Return        : 1 - No key pressed
 *                 0 - Key pressed
 ************************************************************/
u8 colscan(void)
{
    u8 t;

    // Read all four column pins
    // Returns 1 when all columns are HIGH
    // Returns 0 when any column is LOW
    t = (READBIT(IOPIN1, COL0) &
         READBIT(IOPIN1, COL1) &
         READBIT(IOPIN1, COL2) &
         READBIT(IOPIN1, COL3));

    return t;
}


/************************************************************
 * Function Name : rowcheck
 * Description   : Identifies the row in which a key is
 *                 pressed by grounding one row at a time.
 *
 * Parameters    : None
 * Return        : Row number (0 to 3)
 ************************************************************/
u8 rowcheck(void)
{
    u8 r;

    // Ground row 0 and check whether a column becomes LOW
    CLRBIT(IOPIN1, ROW0);
    SETBIT(IOPIN1, ROW1);
    SETBIT(IOPIN1, ROW2);
    SETBIT(IOPIN1, ROW3);

    if (colscan() == 0)
    {
        r = 0;
        goto last;
    }

    // Ground row 1 and check for key press
    SETBIT(IOPIN1, ROW0);
    CLRBIT(IOPIN1, ROW1);
    SETBIT(IOPIN1, ROW2);
    SETBIT(IOPIN1, ROW3);

    if (colscan() == 0)
    {
        r = 1;
        goto last;
    }

    // Ground row 2 and check for key press
    SETBIT(IOPIN1, ROW0);
    SETBIT(IOPIN1, ROW1);
    CLRBIT(IOPIN1, ROW2);
    SETBIT(IOPIN1, ROW3);

    if (colscan() == 0)
    {
        r = 2;
        goto last;
    }

    // Ground row 3 and check for key press
    SETBIT(IOPIN1, ROW0);
    SETBIT(IOPIN1, ROW1);
    SETBIT(IOPIN1, ROW2);
    CLRBIT(IOPIN1, ROW3);

    if (colscan() == 0)
    {
        r = 3;
        goto last;
    }

last:
    // Restore all row pins to LOW
    CLRBIT(IOPIN1, ROW0);
    CLRBIT(IOPIN1, ROW1);
    CLRBIT(IOPIN1, ROW2);
    CLRBIT(IOPIN1, ROW3);

    return r;
}


/************************************************************
 * Function Name : colcheck
 * Description   : Identifies the column in which a key is
 *                 pressed by checking the column inputs.
 *
 * Parameters    : None
 * Return        : Column number (0 to 3)
 ************************************************************/
u8 colcheck(void)
{
    u8 c;

    if (READBIT(IOPIN1, COL0) == 0)
        c = 0;

    else if (READBIT(IOPIN1, COL1) == 0)
        c = 1;

    else if (READBIT(IOPIN1, COL2) == 0)
        c = 2;

    else if (READBIT(IOPIN1, COL3) == 0)
        c = 3;

    return c;
}


/************************************************************
 * Function Name : keyscan
 * Description   : Scans the keypad and identifies the
 *                 character corresponding to the pressed key.
 *
 * Parameters    : None
 * Return        : ASCII value of the pressed key
 ************************************************************/
u8 keyscan(void)
{
    u8 r, c, keyV;

    // Wait until a key is pressed
    while (colscan());

    // Identify the pressed key row and column
    r = rowcheck();
    c = colcheck();

    // Retrieve the corresponding character from lookup table
    keyV = kpmLUT[r][c];

    return keyV;
}


/************************************************************
 * Function Name : ReadNum
 * Description   : Reads a numeric value from the keypad
 *                 and displays the entered digits on LCD.
 *
 * Parameters    : None
 * Return        : Entered unsigned integer value
 ************************************************************/
u32 ReadNum(void)
{
    u32 keyV, sum = 0;

    while (1)
    {
        keyV = keyscan();

        // Check whether the pressed key is a numeric digit
        if ((keyV >= '0') && (keyV <= '9'))
        {
            // Build the number digit by digit
            sum = (sum * 10) + (keyV - '0');

            CmdLCD(GOTO_LINE2_POS0);
            U32LCD(sum);

            // Wait until the pressed key is released
            while (colscan() == 0);

            if (sum == 'd')
                return 0;

            return sum;
        }
        else
        {
            // Clear LCD when the 'c' key is pressed
            if (keyV == 'c')
            {
                CmdLCD(CLEAR_LCD);
            }

            // Wait until the pressed key is released
            while (colscan() == 0);

            break;
        }
    }

    return sum;
}


/************************************************************
 * Function Name : ReadNum2
 * Description   : Reads a maximum of two numeric digits
 *                 from the keypad and displays the result.
 *
 * Parameters    : num     - Pointer to store entered number
 *                 lastkey - Pointer to store last pressed key
 * Return        : None
 ************************************************************/
void ReadNum2(u32 *num, u8 *lastkey)
{
    u32 count = 0;

    *num = 0;

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("            ");              // Clear second LCD line

    while (count < 2)
    {
        *lastkey = keyscan();

        // Accept only numeric keys
        if ((*lastkey >= '0') && (*lastkey <= '9'))
        {
            count++;

            // Build the number digit by digit
            *num = (*num * 10) + (*lastkey - '0');

            CmdLCD(GOTO_LINE2_POS0);

            if (count == 1)
            {
                CharLCD(*lastkey);       // Display first digit
            }
            else
            {
                // Display both entered digits
                CharLCD((*num / 10) + '0');
                CharLCD((*num % 10) + '0');
            }

            // Wait until the key is released
            while (colscan() == 0);
        }
        else
        {
            // Wait until the key is released
            while (colscan() == 0);

            break;
        }
    }
}


/************************************************************
 * Function Name : ReadNum4
 * Description   : Reads a maximum of four numeric digits
 *                 from the keypad and displays the result.
 *
 * Parameters    : num     - Pointer to store entered number
 *                 lastkey - Pointer to store last pressed key
 * Return        : None
 ************************************************************/
void ReadNum4(u32 *num, u8 *lastkey)
{
    u32 count = 0;

    *num = 0;

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("             ");             // Clear second LCD line

    while (count < 4)
    {
        *lastkey = keyscan();

        // Accept only numeric keys
        if ((*lastkey >= '0') && (*lastkey <= '9'))
        {
            count++;

            // Build the number digit by digit
            *num = (*num * 10) + (*lastkey - '0');

            CharLCD(*lastkey);           // Display entered digit

            // Wait until the key is released
            while (colscan() == 0);
        }
        else
        {
            // Wait until the key is released
            while (colscan() == 0);

            break;
        }
    }
}
