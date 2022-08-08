/* Lab 7 Part 1A G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define TIMER_PERIOD_0 600 /* 3 MHz of SMCLK / 5000 Hz for frequency of LED */
#define TIMER_PERIOD_1 390 /* 35% duty cycle with SET RESET mode: OFF for 0-390 (600*0.65) and ON for 390-600 (600-600*0.35) */

const Timer_A_CompareModeConfig compareModeConfig=
    {
     TIMER_A_CAPTURECOMPARE_REGISTER_2, /* For CCR2 */
     TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
     TIMER_A_OUTPUTMODE_SET_RESET,
     TIMER_PERIOD_1

    };



const Timer_A_UpModeConfig upConfig=
{
 TIMER_A_CLOCKSOURCE_SMCLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 TIMER_PERIOD_0,
 TIMER_A_TAIE_INTERRUPT_DISABLE,// interrupt when counter reaches 0, ISR is TAx_N
 TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,// interrupt when counter reaches max, ISR is TAx_0
 TIMER_A_DO_CLEAR
};

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    PCM_setPowerState(PCM_AM_LF_VCORE0);
    //J.0 is Pin 41 LFXIN
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION);
    //P5.7 is the compare pin for Timer A2,CCR2 (TA2.2)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    CS_setExternalClockSourceFrequency(32768,48000000);
    CS_startHFXT(false); /* Start HFXTCLK without bypass */
    CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_16); /* Set SMCLK to be 3 MHz = 48 MHz / 16 */

    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_initCompare(TIMER_A2_BASE, &compareModeConfig);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

}
