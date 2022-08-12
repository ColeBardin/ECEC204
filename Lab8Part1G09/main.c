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
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1);
    GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_2);
    M=CS_getMCLK();

    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);// REFOCLK is 32 KHz
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_2);// ACLK is 8192 Hz
    SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
    WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,WDT_A_CLOCKITERATIONS_32K);//Watchdog reset at 1 sec
    WDT_A_startTimer();

    acquireImage (&in, MIN_VALUE, MAX_VALUE, SIZE); // Acquire image
    blurFilter (&in, &out); // Process image

    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);

    }
