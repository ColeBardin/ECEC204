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
 * MSP432 Listing 7.13 page 143
 *
 * Description: Calcultes frequency of ACLK generated on Pin 4.2 and applied to Pin 2.4 by input capture
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
     TIMER_A_CLOCKSOURCE_SMCLK,
     TIMER_A_CLOCKSOURCE_DIVIDER_1,
     TIMER_A_TAIE_INTERRUPT_ENABLE,
     TIMER_A_DO_CLEAR

    };

    const Timer_A_CaptureModeConfig captureModeConfig=
    {
     TIMER_A_CAPTURECOMPARE_REGISTER_1,
     TIMER_A_CAPTUREMODE_RISING_EDGE,
     TIMER_A_CAPTURE_INPUTSELECT_CCIxB,
     TIMER_A_CAPTURE_SYNCHRONOUS,
     TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
     TIMER_A_OUTPUTMODE_OUTBITVALUE
    };

     int i=0;
     int j=0;

     volatile uint16_t CaptureValues[2]={0,0};
     volatile float CaptureFreq[10];

     int main(void)

     { MAP_WDT_A_holdTimer();
       CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
       CS_initClockSignal(CS_ACLK,CS_VLOCLK_SELECT,CS_CLOCK_DIVIDER_1);
       CS_setDCOFrequency(CS_12MHZ);

       GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
       GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);

       Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
       Timer_A_initCapture(TIMER_A0_BASE, &captureModeConfig);

       //Interrupt_enableSleepOnIsrExit();
       Interrupt_enableInterrupt(INT_TA0_N);
       Interrupt_enableMaster();

       Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_CONTINUOUS_MODE);
       GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
       GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN1);
       GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);

       //PCM_setPowerState(PCM_LPM0_LDO_VCORE1);
       while(1)
       {

       }


}

     void TA0_N_IRQHandler(void)
             {

               GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
               Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
                  if(i<10)
                {
                   CaptureValues[1]=Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
                   CaptureFreq[i++]= (float)12000000/(CaptureValues[1]-CaptureValues[0]);
                   CaptureValues[0]=CaptureValues[1];
                }
                  else if(i==10)
                      j=i;



               }
