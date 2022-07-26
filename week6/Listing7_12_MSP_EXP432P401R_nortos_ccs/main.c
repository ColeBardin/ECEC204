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
 * MSP432 Listing 7_12 (Page 142)
 *
 * Description: LED1 flashes at 1 sec. LED2 Green flashes at 2 sec
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


    /* Stop Watchdog  */


#define TIMER_PERIOD_0 8192
#define TIMER_PERIOD_1 32000

const Timer_A_UpDownModeConfig updownConfig=
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 TIMER_PERIOD_0,
 TIMER_A_TAIE_INTERRUPT_ENABLE,
 TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
 TIMER_A_DO_CLEAR
};


const Timer_A_UpModeConfig upConfig=
{
 TIMER_A_CLOCKSOURCE_SMCLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 TIMER_PERIOD_1,
 TIMER_A_TAIE_INTERRUPT_DISABLE,
 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
 TIMER_A_DO_CLEAR
};

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    PCM_setPowerState(PCM_AM_LF_VCORE0);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ);
    CS_setExternalClockSourceFrequency(32768,48000000);
    CS_startLFXT(CS_LFXT_DRIVE3);
    CS_initClockSignal(CS_ACLK,CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK,CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);


    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

    Timer_A_configureUpDownMode(TIMER_A0_BASE, &updownConfig);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_TA0_N);
    Interrupt_enableInterrupt(INT_TA1_0);
    Interrupt_enableMaster();;

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UPDOWN_MODE);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

}

    void TA0_N_IRQHandler(void)
    {
        Timer_A_clearInterruptFlag(TIMER_A0_BASE);
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    }

    void TA1_0_IRQHandler(void)
        {
            Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);

        }

