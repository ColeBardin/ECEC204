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
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
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

void delaysecs(uint8_t T)
{
    uint32_t i;
    for (i=0;i<200000*T;i++);
}


int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);//RED LED1 ON
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);//GREEN LED2 OFF
    CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ);
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_4);//Set ACLK at 8 KHz
    while(1)
    {
    SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
    WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,WDT_A_CLOCKITERATIONS_32K);//32K ticks and hence 4 secs timeout for WDT
    WDT_A_startTimer();
    delaysecs(3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);//if delay<4secs RED LED1 OFF and GREEN LED2 ON
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
    // WDT resets to 4 secs in the loop
    }
}
