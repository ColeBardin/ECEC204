/* Simple program to illustrate a register-level program.
 * If the switch S1 is presses, the program lights up LED 1.
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy
 * Date: June 17, 2019
 *
 * Source: Code listing 5.1 (page 76) from Unslan, Gurhan, and yucel, Programmable Microcontrollers, McGraw Hill, 2018.
 */
#include "msp.h"

/* Defines */
#define LED1 BIT0
#define S1 BIT1

void
main (void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		/* Stop watchdog timer. */

	P1->DIR = LED1; /* Set P1.0 as output and P1.1 as input. */
	P1->REN = S1;   /* Enable the pull-up/down register for P1.1. */
	P1->OUT = S1;   /* The pull-up/down register is set as pull-up. */

	while (1) {
	    if ((P1->IN & S1) == 0) /* Active-low input indicates the switch is pressed. */
	        P1->OUT |= LED1;    /* Turn on LED 1. */
	    else
	        P1->OUT &= ~LED1;   /* Turn off LED 1. */
	}
}


