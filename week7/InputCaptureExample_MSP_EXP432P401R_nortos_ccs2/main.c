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
 * Input Capture Example
 *
 * Description: Pin 2,5 is set up in capture mode for Rising Edges using Timer A0
 *  and CCR2. Input clock frequency to Timer A0 is set at 2 KHz to avoid early
 *   overflows. Elapsed time between 2 successive rising edges on Pin 2.5 is
 *   calculated based on 2 successive readings of CCR2.
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


    const Timer_A_ContinuousModeConfig continuousModeConfig=
    {
     TIMER_A_CLOCKSOURCE_ACLK,
     TIMER_A_CLOCKSOURCE_DIVIDER_1,
     TIMER_A_TAIE_INTERRUPT_DISABLE,
     TIMER_A_DO_CLEAR

    };

    const Timer_A_CaptureModeConfig captureModeConfig=
    {
     TIMER_A_CAPTURECOMPARE_REGISTER_2,
     TIMER_A_CAPTUREMODE_RISING_EDGE,
     TIMER_A_CAPTURE_INPUTSELECT_CCIxA,
     TIMER_A_CAPTURE_SYNCHRONOUS,
     TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
     TIMER_A_OUTPUTMODE_OUTBITVALUE
    };

     uint8_t i=0;
     uint32_t j=0;
     float Timeint;

     volatile uint16_t CaptureValues[10]={0,0,0,0,0,0,0,0,0,0};
     //volatile float CaptureFreq[10];

     int main(void)

     { MAP_WDT_A_holdTimer();

       CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
       CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_64);

       GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);

       Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
       Timer_A_initCapture(TIMER_A0_BASE, &captureModeConfig);


       Timer_A_enableCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
       Interrupt_enableInterrupt(INT_TA0_N);
       Interrupt_enableMaster();
       Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
       Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_CONTINUOUS_MODE);

       GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
       GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN1);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);



        while(1)
       {
           j++;
           j--;

       }
}

     void TA0_N_IRQHandler(void)
        {
               GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);
           CaptureValues[i]=Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);

           {   if(i>0)
               Timeint=(float)(CaptureValues[i]-CaptureValues[i-1])/2000.0f;
           i++;
           }
              Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);

         }
