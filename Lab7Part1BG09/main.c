/* Lab 7 Part 1B G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define TIMER_PERIOD_0 128
#define TIMER_PERIOD_1  16

const Timer_A_CompareModeConfig compareModeConfig=
    {
     TIMER_A_CAPTURECOMPARE_REGISTER_1,
     TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
     TIMER_A_OUTPUTMODE_SET_RESET,
     TIMER_PERIOD_1

    };



const Timer_A_UpModeConfig upConfig=
{
 TIMER_A_CLOCKSOURCE_ACLK,
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
    //P2.4 is the compare pin for Timer A0,CCR1 (TA0.1)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);

    CS_setExternalClockSourceFrequency(32768,48000000);
    CS_startLFXT(CS_LFXT_DRIVE3);
    CS_initClockSignal(CS_ACLK,CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_initCompare(TIMER_A0_BASE, &compareModeConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

}
