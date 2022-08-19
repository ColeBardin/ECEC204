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

uint32_t M;
/* Global variables containing the input and output images. */
image_t in, out;

int main (void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();
    // Set up LFXT and from that derive ACLK at 32 KHz
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN0|GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_2); /* WDT Frequency: 128 KHz / Divider */

    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_8); /* MCLK Frequency: 12 MHz / Divider */
    M=CS_getMCLK();

    /* Configure WDT */
    MAP_SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
    MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_SMCLK,WDT_A_CLOCKITERATIONS_32K);
    MAP_WDT_A_startTimer(); /* Start the watchdog. */

    /* Process the image blurring */
    acquireImage (&in, MIN_VALUE, MAX_VALUE, SIZE); // Acquire image
    blurFilter (&in, &out); // Process image
    /* Service WDT */
    MAP_WDT_A_clearTimer();
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);

    while(1);
}
