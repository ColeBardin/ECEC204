/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * Lab Week 7 Part 1B Starter Project
 *
 * Description: Generates PWM waveform with frequency of 1000 Hz and
 * Duty Cycle 30% on Blue LED2 (Pin 2.2)
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>


#define TIMER_PERIOD_0 1500
#define TIMER_PERIOD_1 1050

const Timer_A_CompareModeConfig compareModeConfig=
    {
     TIMER_A_CAPTURECOMPARE_REGISTER_1,
     TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
     TIMER_A_OUTPUTMODE_SET_RESET,
     TIMER_PERIOD_1

    };



const Timer_A_UpModeConfig upConfig=
{
 TIMER_A_CLOCKSOURCE_SMCLK,
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
    //GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    CS_setExternalClockSourceFrequency(32768,48000000);
    CS_startHFXT(false);
    CS_initClockSignal(CS_SMCLK,CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_32);

    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_initCompare(TIMER_A0_BASE, &compareModeConfig);


    Interrupt_enableSleepOnIsrExit();
    Timer_A_enableCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Interrupt_enableInterrupt(INT_TA0_N);
    Interrupt_enableInterrupt(INT_TA0_0);
    Interrupt_enableMaster();;

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);


    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

}

    void TA0_0_IRQHandler(void)
    {
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);

     }

    void TA0_N_IRQHandler(void)
        {
            Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }
