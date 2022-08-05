/* Lab 7 Part 1B G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define TIMER_PERIOD_0 640 /* CCR0 value for 50 Hz: 640 (32 KHz ACLK / 50 Hz desired LED frequency) */
#define TIMER_PERIOD_1 480 /* CCRN value for 25% duty cycle on 50 Hz in SET RESET mode: 480 (640-0.25*640) */

const Timer_A_CompareModeConfig compareModeConfig=
{
 TIMER_A_CAPTURECOMPARE_REGISTER_1,
 TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
 TIMER_A_OUTPUTMODE_SET_RESET,
 TIMER_PERIOD_1
};

const Timer_A_UpModeConfig upConfig=
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 TIMER_PERIOD_0,
 TIMER_A_TAIE_INTERRUPT_DISABLE,// interrupt when counter reaches 0, ISR is TAx_N
 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,// interrupt when counter reaches max, ISR is TAx_0
 TIMER_A_DO_CLEAR
};

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    PCM_setPowerState(PCM_AM_LF_VCORE0);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION);

    /* TODO: Change output to P1.6 for lab */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

    CS_setExternalClockSourceFrequency(128000,48000000); /* ACLK can only be based from LFXTCLK so set frequency to something divisble by 50 */
    CS_startLFXT(CS_LFXT_DRIVE3); /* Start LFXTCLK in default mode */
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_4); /* Set ACLK to be 32 KHz = 128 KHz / 4 */

    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_initCompare(TIMER_A0_BASE, &compareModeConfig);


    Interrupt_enableSleepOnIsrExit();
    Timer_A_enableCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Interrupt_enableInterrupt(INT_TA0_N);
    Interrupt_enableInterrupt(INT_TA0_0);
    Interrupt_enableMaster();

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);


    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

}

/* TODO: Change to P1.6 for lab */
void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN6);

 }

void TA0_N_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN6);
}
