/* Lab 7 Part 3 G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "uart_functions.h"

const Timer_A_ContinuousModeConfig continuousModeConfig=
{
    TIMER_A_CLOCKSOURCE_ACLK,
    TIMER_A_CLOCKSOURCE_DIVIDER_1,
    TIMER_A_TAIE_INTERRUPT_DISABLE,
    TIMER_A_DO_CLEAR
};

const Timer_A_CaptureModeConfig captureModeConfig=
{
    TIMER_A_CAPTURECOMPARE_REGISTER_3,
    TIMER_A_CAPTUREMODE_RISING_EDGE,
    TIMER_A_CAPTURE_INPUTSELECT_CCIxA,
    TIMER_A_CAPTURE_SYNCHRONOUS,
    TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
    TIMER_A_OUTPUTMODE_OUTBITVALUE
};

uint8_t i=0;
uint32_t j=0;
float Timeint;
uint32_t overflows=0;

volatile uint16_t CaptureValues[10]={0,0,0,0,0,0,0,0,0,0};
//volatile float CaptureFreq[10];

int main(void)
{
    MAP_WDT_A_holdTimer();

    initUART();

    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_64); /* ACLK at 2 KHz */

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION); /* P6.6 on TA2.3 */

    Timer_A_configureContinuousMode(TIMER_A2_BASE, &continuousModeConfig);
    Timer_A_initCapture(TIMER_A2_BASE, &captureModeConfig);


    Timer_A_enableCaptureCompareInterrupt (TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
    Interrupt_enableInterrupt(INT_TA2_N);
    Interrupt_enableInterrupt(INT_TA2_0);
    Interrupt_enableMaster();
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
    Timer_A_startCounter(TIMER_A2_BASE,TIMER_A_CONTINUOUS_MODE);

    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);



    while(1)
    {
       j++;
       j--;

    }
}

void TA2_N_IRQHandler(void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);
    CaptureValues[i]=Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);

    if(i>0) {
        Timeint=(float)(CaptureValues[i]-CaptureValues[i-1])/2000.0f;
        i++;
    }
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);

}


void TA2_0_IRQHandler(void){
    return;
}
