/* Simple program to illustrate basic concepts: building and loading the executable; and setting breakpoints and inspecting
 * variables and expressions for debugging purposes.
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy
 * Date: June 17, 2019
 *
 * Source: Code listing 3.6 (page 38) from Unslan, Gurhan, and yucel, Programmable Microcontrollers, McGraw Hill, 2018.
 */

#include "msp.h"

void
main (void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		/* stop watchdog timer. */
	char a = 0x02;
	char b = 0xAF;
	char c,d,e,f;

	c = a | b; /* Perform a bit-wise OR. */
	d = a & b; /* Perform a bit-wise AND. */
	e = a ^ b; /* Perform a bit-wise XOR. */
	f = ~a; /* Perform a complement operation. */

	/* Loop forever. */
	while (1);
}
