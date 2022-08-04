/* Lab 7 Part 2 G09 */
/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"

/* Defines. */
#define PWM_PERIOD 32000 // Timer period
#define DUTY_CYCLE_LENGTH 0.1 * PWM_PERIOD // Initial duty cycle length

/* Configuration parameters for Timer_A module in PWM configuration */
Timer_A_PWMConfig pwmConfig =
{
 TIMER_A_CLOCKSOURCE_ACLK,                  // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_1,             // Divider ratio, 1:1
 PWM_PERIOD,                                // Timer period which will stored in CCR0
 TIMER_A_CAPTURECOMPARE_REGISTER_1,         // Compare register to use
 TIMER_A_OUTPUTMODE_RESET_SET,              // Output mode for PWM signal
 DUTY_CYCLE_LENGTH                          // Duty cycle length which will be stored in CCR1
};

int
main (void)
{
    /* Stop watchdog timer. */
    MAP_WDT_A_holdTimer ();

    /* Initialize the high-speed clock system. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    /* Configure output LED. */
    GPIO_setAsOutputPin (GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN0);

    /* Configure switch S1. */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    /* Configure P2.4 as the pin that outputs the PWM signal. */
    GPIO_setAsPeripheralModuleFunctionOutputPin (GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configure PWM. */
    Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);

    /* Configure and enable interrupt system. */
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1);
    Interrupt_enableInterrupt (INT_PORT1); // ISR for when button S1 is pressed

    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Interrupt_enableInterrupt (INT_TA0_0);
    Interrupt_enableInterrupt (INT_TA0_N);
    Interrupt_enableMaster ();

    while (1);

}

/* ISR to handle switch S1. */
void
PORT1_IRQHandler (void)
{
    uint32_t status;
    static float duty_cycle = 0.1;

    status = GPIO_getEnabledInterruptStatus (GPIO_PORT_P1);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, status);

    /* Isolate P1.1 */
    if (status & GPIO_PIN1) {
        if (duty_cycle >= 1.0)
            duty_cycle = 0.1;
        else
            duty_cycle += 0.1;

        /* Update the configuration with the length of the new duty cycle. */
        int duty_cycle_length = (int) PWM_PERIOD * duty_cycle;
        pwmConfig.dutyCycle = duty_cycle_length;
        Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);
    }

    return;
}

/* ISR to handle end of timer period. */
void
TA0_0_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    writeString ("\n\rEnd of timer period");
    /* User-defined code. */

    return;
}

/* ISR to handle end of PWM pulse. */
void
TA0_N_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    writeString ("\n\rEnd of PWM pulse");
    /* User-defined code. */

    return;
}
