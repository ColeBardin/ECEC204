/* Description: The WDT module is used to illustrate how the watchdog can
 * initiate a reset if the system becomes unresponsive. The watchdog timer is set up to
 * initiate a soft reset if it hasn't been serviced in 4 seconds. A simple SysTick is also
 * setup to make it so that the watchdog is serviced every second. When the GPIO button connected
 * to P1.1 is pressed, the SysTick  interrupt will be disabled causing the 
 * watchdog to timeout. Upon reset, the program will detect that the watchdog 
 * timeout triggered a soft reset and  blink the LED on P1.0 to signify the 
 * watchdog timeout.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.1  |<--- Switch
 *            |                  |
 *            |                  |
 *            |            P1.0  |---> LED
 *            |                  |
 *            |                  |
 * Modified by: Naga Kandasamy, August 7, 2019
 * Original source: TI SDK
 ******************************************************************************/
/* DriverLib includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

#define WDT_A_TIMEOUT RESET_SRC_1

int
main (void)
{
    volatile uint32_t ii;
    
    /* Halt the watchdog while we set it up. */
    MAP_WDT_A_holdTimer ();

    /* If the watchdog just reset us, toggle a GPIO to illustrate
        that the watchdog timed out. Period of LED is 1s */
    if (MAP_ResetCtl_getSoftResetSource () & WDT_A_TIMEOUT) {
        MAP_GPIO_setAsOutputPin (GPIO_PORT_P1, GPIO_PIN0);
        while (1) {
            MAP_GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);
            for (ii = 0; ii < 80000; ii++); // Delay
        }
    }

    /* Setting MCLK to REFO at 128Khz for low frequency mode and SMCLK to REFO. */
    MAP_CS_setReferenceOscillatorFrequency (CS_REFO_128KHZ);
    MAP_CS_initClockSignal (CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal (CS_HSMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal (CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Configure P1.1 as an input for button press */
    MAP_GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);

    /* Configure WDT to timeout after 512k iterations of SMCLK, at 128k,
     * this will roughly equal 4 seconds. A soft reset will be performed if the
     * watchdog times out. */
    MAP_SysCtl_setWDTTimeoutResetType (SYSCTL_SOFT_RESET);
    /* See page 381 in the DriverLib user guide for valid values for
     * WDT_A_CLOCKSOURCE and WDT_A_CLOCK_ITERATIONS.
     */
    MAP_WDT_A_initWatchdogTimer (WDT_A_CLOCKSOURCE_SMCLK, WDT_A_CLOCKITERATIONS_512K);
    
    /* Set the SysTick module to wake up every 128000 clock iterations to service
     * the dog.
     */
    MAP_SysTick_enableModule ();
    MAP_SysTick_setPeriod (128000);
    MAP_SysTick_enableInterrupt ();

    /* Enable interrupts and start watchdog timer. */
    MAP_GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1);
    MAP_Interrupt_enableInterrupt (INT_PORT1);
    MAP_Interrupt_enableMaster ();

    MAP_WDT_A_startTimer ();

    while (1);
}

/* SysTick ISR - This ISR will fire every 1s and service the watchdog. */
void
SysTick_Handler (void)
{
    MAP_WDT_A_clearTimer ();
    return;
}

/* ISR to handle the button press on S1. */
void
PORT1_IRQHandler (void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus (GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag (GPIO_PORT_P1, status);

    /* Isolate status of pin 1. */
    if (status & GPIO_PIN1) {
        MAP_SysTick_disableInterrupt (); // Simulate a fault by diabling the interrupt servicing the watchdog.
    }
    return;
}
