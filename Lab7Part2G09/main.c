/* Lab 7 Part 2 G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "uart_functions.h"

#define TIMER_PERIOD 75000 // Counter for updating duty cycle. 75,000 = 750 KHz * 4 seconds per period / 40 steps per period

#define PWM_PERIOD  128 // CCR0 at 256 = 128 KHz / 500 Hz desired Hz
#define DUTY_CYCLE_LENGTH 0 // CCR3 starts at 0
#define STEP 0.05f

double duty_cycle=0.0;
uint8_t up = 1;

/* Configuration parameters for Timer_A module in PWM configuration */
Timer_A_PWMConfig pwmConfig =
{
    TIMER_A_CLOCKSOURCE_ACLK,                  // Clock source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,             // Divider ratio, 1:1
    PWM_PERIOD,                                // Timer period which will stored in CCR0
    TIMER_A_CAPTURECOMPARE_REGISTER_3,         // Compare register to use (TA0.3 goes to CCR3 */
    TIMER_A_OUTPUTMODE_RESET_SET,              // Output mode for PWM signal
    DUTY_CYCLE_LENGTH                          // Duty cycle length which will be stored in CCR3
};

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* For duty cycle debug */
    initUART();

    /* Set power state to Active Mode, LDO annd Core Voltage of 1 */
    PCM_setPowerState(PCM_AM_LDO_VCORE1);

    /* Edit number of wait states for flash memory */
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    /* Enable Capture pin P2.6 = TA0.3 */
    GPIO_setAsPeripheralModuleFunctionOutputPin (GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_4); // MCLK 12 MHz
    CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_128KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // ACLK is 128 KHz for Timer A

    /* Initialize T32 at 750 KHz = 12 MHz of MCLK / 16 Prescaler */
    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE); /* Timer32 at 750 KHz */
    /* Start count height such that 40 counts will take 4 seconds: 750 KHz * 4 seconds per period / 40 counts per period */
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD);
    /* Start the counter */
    Timer32_startTimer(TIMER32_0_BASE,false);

    /* Initial Configure PWM. */
    Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);

    /* Enable Capture and Compare interrupts for Timer A */
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3);
    /* Enable Timer 32 Interrupts */
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    /* Enable Master Interrupt */
    Interrupt_enableMaster();

    /* Set CPU to Low Power Mode, LDO, Core Voltage of 1 */
    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);
}

void T32_INT1_IRQHandler(void){
    Timer32_clearInterruptFlag(TIMER32_0_BASE);

    /* Adjust duty cycle percentage */
    /* Decide if direction change is needed */
    if ( duty_cycle > 0.99 ) up=0;
    if ( duty_cycle < 0.01 ) up=1;
    /* Move STEP in appropriate direction */
    if (up == 1) duty_cycle += STEP;
    else duty_cycle -= STEP;

    /* Update the configuration with the length of the new duty cycle. */
    int duty_cycle_length = (int) PWM_PERIOD * duty_cycle;
    pwmConfig.dutyCycle = duty_cycle_length;
    Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);
    /* Print duty cycle to serial */
    writeFloat(duty_cycle);

    return;
}
