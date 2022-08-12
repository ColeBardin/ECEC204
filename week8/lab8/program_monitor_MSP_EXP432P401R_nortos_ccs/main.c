/* Description: Skeleton code for the program monitor
 *
 * Author: Naga Kandasamy
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"
#include "blur_filter.h"

/* Global variables containing the input and output images. */
image_t in, out;

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    while (1) {
        writeString ("\n\rAcquiring and processing image");
        acquireImage (&in, MIN_VALUE, MAX_VALUE, SIZE); // Acquire image
        blurFilter (&in, &out); // Process image
    }
}
