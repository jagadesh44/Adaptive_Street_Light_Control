/**************************
 * File Name    : interrupt.c
 * Description  : Implements external interrupt initialization, RTC
 *                configuration, ADC conversion, RTC display,
 *                keypad-based RTC editing and adaptive streetlight
 *                control based on time and environmental illumination.
 *
 * Controller   : LPC2148
 * Peripherals  : External Interrupt, RTC, ADC, LCD, Keypad, LDR, LED
 * Application  : Adaptive Street Light Control
 *
 * Author       : Embedded Systems Project
 **************************/
#include <LPC214x.h>
#include "lcd.h"
#include "lcd_defines.h"
#include  "kpm.h"
#include "delay.h"

/**************************
 * Function Name : init_eint
 * Description   : Configures external interrupt EINT0 and assigns
 *                 its interrupt service routine to VIC vector slot 0.
 * Parameters    : None
 * Return        : None
 **************************/
void init_eint(void)
	{

		PINSEL0&=~(3<<(1*2));		// Clear pin function bits for P0.1
	
		PINSEL0|=(3<<2);				// Configure P0.1 pin function
	
		VICIntSelect&=~(1<<14);	// Configure EINT0 as an IRQ interrupt
	
		VICVectCntl0=(1<<5)|14;	  // Enable VIC vector slot 0 for EINT0
	
		VICIntEnable=(1<<14);		// Enable EINT0 interrupt in VIC
	
		EXTINT=1<<0;						// Clear pending EINT0 interrupt flag
	
		EXTMODE=1<<0;						 // Configure EINT0 as edge-sensitive
	
		EXTPOLAR=0;							// Configure EINT0 for falling-edge detection
	
	}

/**************************
 * Function Name : init_rtc
 * Description   : Enables the internal RTC peripheral and selects
 *                 the RTC clock source.
 * Parameters    : None
 * Return        : None
 **************************/
void init_rtc(void)
	{
		CCR|=1<<0;		// Enable RTC operation

		CCR|=1<<4;	  // Enable RTC clock source

	}
/**************************
 * Function Name : init_adc
 * Description   : Configures the ADC peripheral for the selected
 *                 analog input channel and enables ADC operation.
 * Parameters    : ch - ADC channel number
 * Return        : None
 **************************/
void init_adc(unsigned int ch)
	{

		PINSEL1&=~(3<<24);	// Clear pin function bits for ADC pin P0.28
	
		PINSEL1|=(1<<24);			// Configure P0.28 as ADC input AD0.1

		AD0CR|=1<<ch;				  // Select the required ADC channel

		AD0CR|=4<<8;		  // Configure ADC clock divider

		AD0CR|=1<<21;			// Enable ADC peripheral
	
	}
/**************************
 * Function Name : adc
 * Description   : Performs ADC conversion of the LDR analog signal,
 *                 calculates the corresponding input voltage and
 *                 displays the voltage value on the LCD.
 * Parameters    : None
 * Return        : float - Measured analog input voltage.
 **************************/
float adc(void)
{
    unsigned int res;
    unsigned int voltage10;
    float v;

    AD0CR |= (1<<24);		 // Start ADC conversion

    while(((AD0GDR>>31)&1)==0);	 // Wait until ADC conversion is complete

    res=(AD0GDR>>6)&0x3FF;	 // Extract 10-bit ADC conversion result

    v=res*3.3/1023;			 // Convert ADC result into input voltage

    // Convert voltage into one decimal digit
    voltage10=(res*33+511)/1023;

    CmdLCD(GOTO_LINE1_POS0+9);	// Position LCD cursor for LDR label
	
    StrLCD("LDR:");		  // Display LDR identification

    CmdLCD(GOTO_LINE1_POS0+13);		// Position cursor for voltage display

    CharLCD((voltage10/10)+'0');		// Display integer part of voltage
		
    CharLCD('.');					 // Display decimal point
		
    CharLCD((voltage10%10)+'0');		  // Display fractional part of voltage

    return v;		 // Return measured voltage
}	
/**************************
 * Function Name : rtc_lcd
 * Description   : Reads the current RTC time and displays hours,
 *                 minutes and seconds on the first LCD line.
 * Parameters    : None
 * Return        : None
 **************************/
void rtc_lcd()
{

		CmdLCD(GOTO_LINE1_POS0);// Move cursor to first line, position 0
	

		CharLCD((HOUR/10)+48);	//Display tens digit of hours
	

		CharLCD((HOUR%10)+48);	//Display units digit of hours
	
		CharLCD(':');	//Display colon

		CharLCD((MIN/10)+48);		//Display tens digit of minutes

		CharLCD((MIN%10)+48);	//Display units digit of minutes
	
		CharLCD(':');	//Display colon

		CharLCD((SEC/10)+48);	//Display tens digit of seconds

		CharLCD((SEC%10)+48);	//Display units digit of seconds

}

/**************************
 * Function Name : edits
 * Description   : Provides keypad-based RTC configuration through
 *                 an interrupt-driven menu. Allows the user to
 *                 modify hours, minutes, seconds, date, month
 *                 and year with input range validation.
 * Parameters    : flag - External interrupt status flag
 *                 num  - Pointer to keypad numerical input
 *                 last - Pointer to last detected keypad key
 * Return        : u32 - Updated interrupt status flag.
 **************************/

u32 edits(u32 flag,u32 *num,u8 *last)
{
		u32 temp,op;
		// Check whether an external interrupt request has occurred.
		if(flag==1)
{

		CmdLCD(CLEAR_LCD);		// Clear LCD display

		StrLCD("interrupt occured");	// Display interrupt message

		delay_ms(500);	//Wait for 500 milliseconds
	
		CmdLCD(CLEAR_LCD);	   // Clear LCD display
	
		StrLCD("RTC DISPLAY");	// Display RTC configuration message
	
		delay_ms(500);	// Provide a short display delay

		IOPIN0^=(1U<<7);	// Toggle LED connected to P0.7
	
	while(1)
	{
		CmdLCD(CLEAR_LCD);	 // Clear LCD before displaying menu

		StrLCD("1.HH2.MM3.SS");	// Display time editing options
	
		CmdLCD(GOTO_LINE2_POS0);	  // Move cursor to second LCD line
	
		StrLCD("4.DOM5.MON6.YEAR");	// Display date editing options

		op = ReadNum();			 // Read selected menu option
	
		// Check whether the user has requested to exit the menu.
		if(op==0)
		{
			flag = 0;	// Clear interrupt status flag
	
			*num = 0;		// Reset keypad numerical input
	
			break;		 // Exit RTC editing menu
		}
			CmdLCD(CLEAR_LCD);		// Clear LCD before accepting new input

			// Configure RTC hour value.	
		if(op == 1)
			{
				temp = HOUR;	// Store previous hour value
	
				StrLCD("Enter Hours:");	// Request hour input
	
				ReadNum2(num, last);	// Read two-digit hour value
	
			if(*num <= 23)
			HOUR = *num;	 // Update RTC hour with valid input
			else
			{
				HOUR = temp;		// Restore previous hour value
				CmdLCD(CLEAR_LCD);
				StrLCD("Invalid input");		 // Display input validation error
			}
			*num = 0;			// Reset numerical input
			}
			// Configure RTC minute value.
			else if(op==2)
			{

				temp=MIN;		//Store old minute value
	
				StrLCD("enter minutes");	// Request minute input

				ReadNum2(num,last);		//Read new minute value
				if(*num<=59)

				MIN=*num;		// Update RTC minute with valid input
			else
			{
				MIN = temp;	 // Restore previous minute value
	
				CmdLCD(CLEAR_LCD);
				StrLCD("Invalid input");		 // Display input validation error
			}
				*num=0;		// Reset numerical input
			}

					// Configure RTC second value.    
		else if(op==3)
		{

			temp=SEC;	//Store old second value
	
			StrLCD("enter seconds");	// Request second input

			ReadNum2(num,last);	// Read two-digit second value
			if(*num<=59)

			SEC=*num;	// Update RTC seconds with valid input
			else	
			{
				SEC = temp;	 // Restore previous second value
	
				CmdLCD(CLEAR_LCD);
	
				StrLCD("Invalid input");	 // Display input validation error
	
			}

			*num=0;		// Reset numerical input
			}
				// Configure RTC date value.
			else if(op==4)
			{
				temp=DOM;		 // Store previous date value
	
				StrLCD("enter date");	// Request date input
	
				ReadNum2(num,last);	// Read two-digit date value
	
				if(*num>=1 && *num<=31)
				DOM=*num;		 // Update date with valid input
				else
				{
					DOM = temp;	// Restore previous date value
	
					CmdLCD(CLEAR_LCD);
	
					StrLCD("Invalid input");	  // Display input validation error
				}
				*num=0;	// Reset numerical input
				}
				// Configure RTC month value.
				else if(op==5)
				{
					temp=MONTH;		// Store previous month value
	
					StrLCD("enter Month");	// Request month input
	
					ReadNum2(num,last);		// Read two-digit month value
	
					if(*num>=1 && *num<=12)	
	
					MONTH=*num;	 // Update month with valid input

					else
					{
						MONTH= temp;	// Restore previous month value
						CmdLCD(CLEAR_LCD);
	
						StrLCD("Invalid input");	    // Display input validation error
	
					}
					*num=0;		 // Reset numerical input
				}
			// Configure RTC year value.
			else if(op==6)
			{
				temp=YEAR;	 // Store previous year value
	
				StrLCD("enter year");	// Request year input
	
				ReadNum4(num,last);	// Read four-digit year value
	
				if(*num>=2000 && *num<=2099)
	
				YEAR=*num;	// Update year with valid input

				else
				{
				YEAR = temp;	 // Restore previous year value
	
				CmdLCD(CLEAR_LCD);
	
				StrLCD("Invalid input");	 // Display input validation error
				}
					*num=0;	 // Reset numerical input
			}

				// Handle invalid menu selections.
			else
			{
				StrLCD("invlid input");		// Display invalid option message
	
				delay_ms(500);		// Allow message to remain visible
	
			}
			delay_ms(500);	// Provide delay between menu operations
			}
}
return flag;	// Return updated interrupt status
}
/**************************
 * Function Name : led_status
 * Description   : Monitors the environmental illumination using the
 *                 LDR sensor and controls the streetlight LED based
 *                 on the RTC operating schedule and measured voltage.
 * Parameters    : None
 * Return        : None
 **************************/
void led_status(void)
{

	float adc_val=adc();	 // Read LDR sensor voltage
	
			// Check nighttime operating hours and insufficient illumination.
		if(((HOUR>=18)||(HOUR<6)) && (adc_val>1.9))
		{

			IOCLR0=(1<<0);	// Turn ON active-low streetlight LED

			CmdLCD(GOTO_LINE2_POS0);	//Move cursor to second LCD line

			StrLCD("L:ON ");	// Display streetlight ON status	
		}
	
		else
			{
	
				IOSET0=(1<<0);	// Turn OFF active-low streetlight LED
	
				CmdLCD(GOTO_LINE2_POS0);	// Position cursor at second LCD line

				StrLCD("L:OFF");	// Display streetlight OFF status
	
				CharLCD(' ');	// Insert space after LED status
			}
			// Display current RTC date in DD/MM/YYYY format.
				CharLCD((DOM/10)+'0');	 // Display tens digit of date

				CharLCD((DOM%10)+'0');	// Display units digit of date

				CharLCD('/');	// Display date separator

				CharLCD((MONTH/10)+'0');	// Display tens digit of month

				CharLCD((MONTH%10)+'0');	// Display units digit of month

				CharLCD('/');		 // Display date separator

				CharLCD((YEAR/1000)+'0');		// Display thousands digit of year

				CharLCD(((YEAR/100)%10)+'0');	// Display hundreds digit of year

				CharLCD(((YEAR/10)%10)+'0');	 // Display tens digit of year

				CharLCD((YEAR%10)+'0'); // Display units digit of year
}


/**************************
 * End of File : interrupt.c
 **************************/
