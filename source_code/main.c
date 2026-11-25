/**************************
 * File Name    : eint.c
 * Description  : Implements the main application control sequence and
 *                external interrupt handling for the Adaptive Street
 *                Light Control System.
 *
 * Controller   : LPC2148
 * Peripherals  : LCD, Keypad, ADC, RTC, External Interrupt
 * Application  : Adaptive Street Light Control
 *
 * Author       : Embedded Systems Project
 **************************/

#include <LPC21xx.h>
//#include <LPC214x.h>
#include "interrupt.h"
#include "lcd.h"
#include "lcd_defines.h"                                                  
#include "delay.h"                                                         
#include "kpm.h" 
 
 
void eint0_fiq_isr(void)__irq;  //Declaration of external interrupt service routine   

void rtc_lcd(void);//Declaration of RTC display function


u32 flag=0;		//Declaration of Global Variable 

/**************************
 * Function Name : main
 * Description   : Initializes the required peripherals and executes
 *                 the continuous monitoring and control operations
 *                 of the adaptive streetlight system.
 * Parameters    : None
 * Return        : int
 **************************/
int main()
{
u8 last;		// Stores the last detected keypad key
u32 num=0;	// Stores the numerical value entered through keypad

Initlcd();	// Initialize LCD interface

Initkpm();		 // Initialize matrix keypad

init_adc(1);	 // Initialize ADC for LDR sensor input

init_eint();	//Initialize external interrupt EINT0

VICVectAddr0=(unsigned)eint0_fiq_isr;		//Connect interrupt service routine to VIC vector slot 0

init_rtc();		//initialize rtc

IODIR0|=(1<<7)|(1<<5)|(1<<0);		// Configure P0.7, P0.5 and P0.0 as output pins

while(1)
{

rtc_lcd();		//Display current RTC time on LCD

IOPIN0^=(1<<5);	//Toggle LED connected to P0.5
	
flag=edits(flag,&num,&last);	// Check interrupt flag and edit RTC time using keypad
	
led_status();		//Read LDR and control street-light LED

}

}

/**************************
 * Function Name : eint0_fiq_isr
 * Description   : External interrupt 0 service routine. Sets the
 *                 interrupt flag, clears the pending interrupt
 *                 request and acknowledges interrupt completion
 *                 to the VIC.
 * Parameters    : None
 * Return        : None
 **************************/
void eint0_fiq_isr(void)__irq			//External interrupt service routine
{
	
flag=1;						//Set flag when external interrupt occurs

EXTINT=1<<0;		 // Clear EINT0 interrupt pending flag

VICVectAddr=0;	    // Acknowledge interrupt completion to VIC
	
}

/**************************
 * End of File : eint.c
 **************************/
