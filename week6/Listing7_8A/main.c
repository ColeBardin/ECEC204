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
 * MSP432 Listing 7.8
 *
 * Description: Using Timer32 causes LED1 to flash 1 sec ON 1 sec OFF.
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

#define TIMER_PERIOD 3000000

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    PCM_setPowerState(PCM_AM_LDO_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);

    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);

    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD);

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableMaster();

    Timer32_startTimer(TIMER32_0_BASE,false);
    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);
}

void T32_INT1_IRQHandler(void){
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
}

