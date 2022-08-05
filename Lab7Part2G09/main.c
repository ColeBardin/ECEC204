/* Lab 7 Part 2 G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "uart_functions.h"

#define TIMER_PERIOD 300000 // Counter for updating duty cycle. 300,000 = 3 MHz * 4 seconds per period / 40 steps per period

#define PWM_PERIOD  256 // CCR0 at 256 = 128 KHz / 500 Hz desired Hz
#define DUTY_CYCLE_LENGTH 0.0*PWM_PERIOD
#define STEP 0.05f

double duty_cycle=0.0;
uint8_t up = 1;

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

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    initUART();

    PCM_setPowerState(PCM_AM_LDO_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin (GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1); // MCLK 48 MHz
    CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_128KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // ACLK is 128 KHz for Timer A


    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);

    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE); /* Timer32 at 3 MHz */
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD);

    Timer32_startTimer(TIMER32_0_BASE,false);

    /* Configure PWM. */
    Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);

    // Interrupt_enableSleepOnIsrExit();
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Interrupt_enableInterrupt (INT_TA0_0);
    Interrupt_enableInterrupt (INT_TA0_N);

    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    Interrupt_enableMaster();

    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);
}

void T32_INT1_IRQHandler(void){
    Timer32_clearInterruptFlag(TIMER32_0_BASE);

    if ( duty_cycle > 0.99 ) up=0;
    if ( duty_cycle < 0.01 ) up=1;

    if (up == 1) duty_cycle += STEP;
    else duty_cycle -= STEP;

    /* Update the configuration with the length of the new duty cycle. */
    int duty_cycle_length = (int) PWM_PERIOD * duty_cycle;
    pwmConfig.dutyCycle = duty_cycle_length;
    Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);
    writeFloat(duty_cycle);

    return;
}

/* ISR to handle end of timer period. */
void
TA0_0_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    return;
}

/* ISR to handle end of PWM pulse. */
void
TA0_N_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);

    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    return;
}
