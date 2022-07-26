/* Description: When pushbutton S1 is pressed, LED1 toggles.
 * The switch state is sensed using GPIO interrupts.
 *
 * The lack of switch debouncing (making the program think that the switch has been
 * pressed multiple times) can lead to incorrect behavior.
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy, July 12, 2019
 *
 * Notes: Source from listing 6.7 (page 97) of the textbook.
*******************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Configure GPIO pins. */
    GPIO_setAsOutputPin (GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

    /* Set up the interrupt processing system on the microprocessor to handle
     * an edge-triggered GPIO IRQ from P1.1.
     * */
    GPIO_interruptEdgeSelect (GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1);
    Interrupt_enableInterrupt (INT_PORT1);
    Interrupt_enableMaster ();

    while (1);

 }

void
PORT1_IRQHandler (void)
{
    uint32_t status;

    /* Read the status of the GPIO pins in Port 1. */
    status = GPIO_getEnabledInterruptStatus (GPIO_PORT_P1);
    /* Clear the interrupt source. */
    GPIO_clearInterruptFlag (GPIO_PORT_P1, status);
    /* Isolate the IRQ from P1.1 for processing. */
    if (status & GPIO_PIN1)
        GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);

    return;
}

