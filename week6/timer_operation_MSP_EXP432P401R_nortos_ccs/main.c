/* Description: Program shows the use of A_timers to control LEDS.
 *
 * Note: Code adapted from listing 7_12 in the textbook (p. 142)
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy, July 25, 2019
*******************************************************************************/

/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>

/* User-defined includes. */
#include "uart_functions.h"

/* Define desired counter values. */
#define TIMER_A1_COUNTER 31999
#define TIMER_A2_COUNTER 16000

/* Structures used to configure the Timer_A peripheral module on the MSP432 in the following
 * operating modes: continuous, up, and up/down.
 * Timer_A has 16-bit precision.
 * */

/* Continuous timer operation. */
const Timer_A_ContinuousModeConfig continuousModeConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,             // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_64,       // Clock divider
 TIMER_A_TAIE_INTERRUPT_ENABLE,         // TAIE interrupt enabled
 TIMER_A_DO_CLEAR
};

/* Up timer operation. */
const Timer_A_UpModeConfig upConfig=
{
 TIMER_A_CLOCKSOURCE_ACLK,              // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_1,         // Clock divider
 TIMER_A1_COUNTER,                       // Counter value needed to achieve desired timer period
 TIMER_A_TAIE_INTERRUPT_DISABLE,        // TAIE interrupt disabled
 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // CCIE interrupt enabled
 TIMER_A_DO_CLEAR
};


/* Up/down timer operation. */
const Timer_A_UpDownModeConfig updownConfig=
{
 TIMER_A_CLOCKSOURCE_ACLK,      // ACLK: Low frequency auxiliary clock
 TIMER_A_CLOCKSOURCE_DIVIDER_1, // Clock divider = 1
 TIMER_A2_COUNTER,              // Compare value
 TIMER_A_TAIE_INTERRUPT_ENABLE,
 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
 TIMER_A_DO_CLEAR
};

int
main (void)
{
    /* Stop watchdog. */
    MAP_WDT_A_holdTimer ();

    /* Initialize the high-speed clock system. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Main clock derives from DCO
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // High-speed sub-main clock derives from DCO
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Sub-main clock derives from DCO, to control peripherals

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    /* Configure output LEDs. */
    GPIO_setAsOutputPin (GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin (GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin (GPIO_PORT_P2, GPIO_PIN1);

    GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN1);

    /* Configure Timer_A0 in continuous-mode. */
    Timer_A_configureContinuousMode (TIMER_A0_BASE, &continuousModeConfig);

    /* Configure Timer_A1 in up mode. */
    Timer_A_configureUpMode (TIMER_A1_BASE, &upConfig);

    /* COnfigure Timer_A2 in up/down mode. */
    Timer_A_configureUpDownMode (TIMER_A2_BASE, &updownConfig);

    /* Enable the interrupt processing system for the timers. */
    Interrupt_enableInterrupt (INT_TA0_N); // Invoke ISR when Timer_A0 resets to zero
    Interrupt_enableInterrupt (INT_TA1_0); // Invoke ISR when Timer_A1 reaches the compare value when counting up
    Interrupt_enableInterrupt (INT_TA2_0); // Invoke ISR when Timer_A2 reaches the compare value when counting up
    Interrupt_enableInterrupt (INT_TA2_N); // Invoke ISR when Timer_A2 reaches zero when counting down
    Interrupt_enableMaster ();

    /* Start the timers. */
    Timer_A_startCounter (TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
    Timer_A_startCounter (TIMER_A1_BASE, TIMER_A_UP_MODE);
    Timer_A_startCounter (TIMER_A2_BASE, TIMER_A_UPDOWN_MODE);

    while(1);

}

/* ISR for Timer_A0 to catch the TAIFG bit. Toggle RED LED connected to P1.0.
 * In continuous mode, the 16-bit timer counts up until it reaches its maximum value of
 * 0xFFFF or 65535 in decimal, then restarts again from zero. The Timer\_A Interrupt FlaG
 * or TAIFG bit is set when the counter value changes from 0xFFFF to zero, which will
 * trigger the ISR below. */
void
TA0_N_IRQHandler (void)
{
    Timer_A_clearInterruptFlag (TIMER_A0_BASE);
    GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);
    writeInt (Timer_A_getCounterValue (TIMER_A0_BASE));
    return;
}


/* ISR for Timer_A1 to catch the CCIFG bit. Toggle the RED LED connected to P2.0.
 * In up mode, timer counts up until it reaches the value stored in the compare register,
 * TACCR0. Then it restarts from zero again. The Capture/Compare Interrupt FlaG or CCIFG is set
 * when the counter value changes from TACCR0 - 1 to TACCR0, triggering this ISR.
 */

void
TA1_0_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN0);
    writeInt (Timer_A_getCounterValue (TIMER_A1_BASE));
    return;
}

/* ISRs for the up/down counter, Timer A2, toggles the GREEN LED connected to P2.1.
 * In the up/down mode, the timer counts up until it reaches the value in
 * the compare register TAxCCR0. Then, counting is inverted and the timer
 * counts down from TAxCCR0 to zero. The Timer A Interrupt Flag (TAIFG) is
 * set within the Timer A control register when the counter value changes from
 * 1 to 0 when counting down.
 *
 * TA2_0_IRQHandler is invoked when CCIFG bit is set.
 * TA2_N_IRQHandler is invoked when TAIFG bit is set.
 * */

void
TA2_0_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN1);
    writeInt (Timer_A_getCounterValue (TIMER_A2_BASE));
    return;
}

void
TA2_N_IRQHandler (void)
{
    Timer_A_clearInterruptFlag (TIMER_A2_BASE);
    GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN1);
    writeInt (Timer_A_getCounterValue (TIMER_A2_BASE));
    return;
}

