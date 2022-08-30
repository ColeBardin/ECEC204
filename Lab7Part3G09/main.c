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
    TIMER_A_TAIE_INTERRUPT_ENABLE,
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
float Timeint;
uint32_t overflows=0;

volatile uint16_t CaptureValues[2]={0,0};

int main(void)
{
    MAP_WDT_A_holdTimer();

    initUART();

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // SMCLK at 12 MHz for UART

    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_4); /* ACLK at 32 KHz */

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION); /* P6.6 on TA2.3 */

    writeString("Established Connection with board");

    Timer_A_configureContinuousMode(TIMER_A2_BASE, &continuousModeConfig);
    Timer_A_initCapture(TIMER_A2_BASE, &captureModeConfig);

    Timer_A_enableCaptureCompareInterrupt (TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3);
    Interrupt_enableInterrupt(INT_TA2_N);
    Interrupt_enableMaster();
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3);
    Timer_A_startCounter(TIMER_A2_BASE,TIMER_A_CONTINUOUS_MODE);

    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);

    while(1); //bp
}

void TA2_N_IRQHandler(void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);
    uint32_t status = Timer_A_getInterruptStatus(TIMER_A2_BASE);

    /* status=1 -> overflow, stats=0 -> capture */
    if (status == 1) {
        Timer_A_clearInterruptFlag(TIMER_A2_BASE);
        overflows++; //bp
    }
    else if (status == 0) { /* On Rising edge to P6.6 */
        if (i==0){ /* First Rising Edge */
            CaptureValues[i]=Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3); // Capture current timer count
            overflows=0; // Reset overflow counter
            i++; //bp
            writeString("S1");
        } else if (i>0) { /* Second Rising Edge */
            CaptureValues[i]=Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3); // Captuer current timer count
            Timeint=(float)(CaptureValues[i]-CaptureValues[i-1] + overflows*0xFFFF)/32000.0f; // Calculate elapsed time
            i=0; //bp
            writeString("S2");
            writeFloat(Timeint);
        }
    }
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3);
}
