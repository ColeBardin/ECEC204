/* Description: Program counts the number of times the user presses the
 * pushbutton BUTTON1. It displays the current count in the terminal window.
 *
 * The lack of switch debouncing can lead to a single push counted multiple times by the
 * code.
 *
 * Original source: Listing 6.8 from the textbook
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy, July 12, 2019
*******************************************************************************/

/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include <uart_functions.h>

/* Global variable to keep track of the count. */
int count, flag;

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Establish communication with the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    /* Configure GPIO pin P1.1. */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1,GPIO_PIN1);

    /* Initialize the count variable. */
    count = flag = 0;

    /* Configure IRQ handling on the CPU for P1.1. */
    GPIO_interruptEdgeSelect (GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1);
    Interrupt_enableInterrupt (INT_PORT1); /* ISR to handle IRQ from P1. */
    Interrupt_enableMaster ();

    while (1) {
        if (flag == 1) {
            flag = 0;
            writeInt (count);
        }
    }

}

void
PORT1_IRQHandler (void)
{

    uint32_t status = GPIO_getEnabledInterruptStatus (GPIO_PORT_P1);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, status);
    if (status & GPIO_PIN1) {
        count++; /* Increment pushbutton presses. */
        flag = 1; /* Indicate to the main processing loop of pushbutton event. */
    }

    return;
}

