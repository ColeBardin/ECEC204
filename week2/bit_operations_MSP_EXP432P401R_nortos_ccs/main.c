/* Program that performs basic bit-level operations: counting bits, checking
 * if input is a power of two, and rearranging bits.
 *
 * Author: Naga Kandasamy
 * Date created: June 28, 2019
 * Date modified:
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "bit_operations.h"
#include "uart_functions.h"

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    uint8_t v1 = 0xF7; /* Hex representation of the integer. */
    int count = countBits (v1);
    writeInt (count); /* Display result on terminal. */

    uint8_t v2 = 0x0A; /* Hex representation of the integer. */
    int status = isPowerOfTwo (v2);
    writeInt (status);

    uint8_t v3 = 0x56;
    uint8_t rv3 = rearrangeBits (v3);
    writeHex ((int) rv3);

    while (1) {
        
    }
}
