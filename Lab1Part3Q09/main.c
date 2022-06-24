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
#define LED2_RED BIT0
#define LED2_BLUE BIT2
#define S1 BIT1
#define S2 BIT4


void
main (void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     /* Stop watchdog timer. */

    P1->DIR &= ~(S1 | S2);
    P1->REN = S1 | S2;
    P1->OUT = S1 | S2;
    P2->DIR = LED2_RED | LED2_BLUE;
    P2->OUT = 0x00;

    while (1) {
        if ((P1->IN & S1) == 0) /* Active-low input indicates the switch is pressed. */
            P2->OUT ^= LED2_BLUE;    /* Turn on LED 1. */
        else
            P2->OUT &= ~LED2_BLUE;   /* Turn off LED 1. */

        if ((P1->IN & S2) == 0) /* Active-low input indicates the switch is pressed. */
            P2->OUT ^= LED2_RED;    /* Turn on LED 1. */
        else
            P2->OUT &= ~LED2_RED;   /* Turn off LED 1. */
    }
}


