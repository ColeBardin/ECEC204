/*
 * MSP432 UART - PC Echo with 12MHz BRCLK
 *
 * Description: This demo echoes back characters received via a PC serial port.
 *
 * Pin diagram:
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *            |                 |
 *            |                 |
 *       RST -|     P1.3/UCA0TXD|----> PC (echo)
 *            |                 |
 *            |                 |
 *            |     P1.2/UCA0RXD|<---- PC
 *            |                 |
 *
 * Author: Naga Kandasamy
 * Date: June 22, 2019
 * Note: Original source if from the Simplelink SDK
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "uart_functions.h"

int
main (void)
{
    // char buf[BUFFER_SIZE];
    float value;

    /* Halt watchdog timer.  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the UART module. */
    initUART ();

    /* Write to UART. */
    writeString ("Established communication with the board");

    while (1) {
        // MAP_PCM_gotoLPM0();
        readFloat (&value);
        value = value + 10.0;
        writeFloat (value);
    }
}

