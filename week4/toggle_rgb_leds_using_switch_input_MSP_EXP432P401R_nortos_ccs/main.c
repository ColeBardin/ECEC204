/* Description: Program toggles the RGB LEDs between Red and Green each time
 * BUTTON1 is pressed. Implemented using GPIO interrupts.
 *
 * Notes: modified version of code listing 6.9 (page 99) in text book.
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy, July 12, 2019
*******************************************************************************/

/* DriverLib includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"

/* Global variable. */
unsigned int state = 0;

int
main (void)
{
    /* Stop Watchdog.  */
    MAP_WDT_A_holdTimer ();

    /* Establish communication with the host. */
    initUART ();
    writeString ("Established communication with the board");

    /* Configure the output GPIO pins for the RGB LEDs.
     * P2.0 controls the RED component and P2.1 controls the GREEN component.
     */
    GPIO_setAsOutputPin (GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);

    /* Configure the input GPIO pin for BUTTON1 on P1.1. */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

    /* Configure the interrupt system on the processor to respond to IRQ from P1. */
    GPIO_interruptEdgeSelect (GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1);
    Interrupt_enableInterrupt (INT_PORT1);
    Interrupt_enableMaster ();

    state = 0;
    while (1)
    {
        /* Set both colors to off. */
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);

        switch(state) {
            case 0:
                /* Set RED component of RGB LED to high on P2.0. */
                GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN0);
                writeInt (state);
                break;

            case 1:
                /* Set GREEN component of RGB LED to high on P2.1. */
                GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN1);
                writeInt (state);
                break;

            default:
                /* Should never get here unless there is a fault. */
                writeString ("Unexpected error.");
                break;
        }
    }
}

/* The ISR for IRQs originating from PORT 1 to the microprocessor. */
void
PORT1_IRQHandler (void)
{
    uint16_t status;

    status = GPIO_getEnabledInterruptStatus (GPIO_PORT_P1);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, status);
    /* Isolate status of PIN1 from the status bits. */
    if(status & GPIO_PIN1)
        state =  1- state; /* Toggle state. */

    return;
}

