/* Description: Adjust the blinking rate of the LED dynamically based on
 * input from BUTTON1.
 *
 * Author: Naga Kandasamy
 * Date created: July 5, 2019
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

    while (1) {
        
    }
}
