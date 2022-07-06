/* Program that turns on LED 1 when button S1 is pressed.
 * Program uses DriverLib functions rather than operate at the register level.
 *
 * Author: Naga Kandasamy
 * Date: June 17, 2019
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
    MAP_WDT_A_holdTimer();

    /* Configure P1.0 and P1.1 appropriately. */
    GPIO_setAsOutputPin (GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

    while (1) {
        if (GPIO_getInputPinValue (GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_LOW) /* P1.1 is low, indicating switch pressed. */
            GPIO_setOutputHighOnPin (GPIO_PORT_P1, GPIO_PIN0); /* Turn on LED connected to P1.0. */
        else
            GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN0); /* Turn off LED connected to P1.0. */
    }
}
