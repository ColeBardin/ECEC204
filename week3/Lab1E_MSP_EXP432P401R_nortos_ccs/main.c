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
 * MSP432 Lab 1E
 *
 * Description: Incorporates debounce on Switch S1
 *              If S1 is pressed (and may be released also) green LED2 comes ON
 *              If S1 is pressed again (and may be released also) green LED2 goes OFF
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

void
debounce (void)
{
    unsigned int i, j;

    j = 0;
    while (j != 0x001E) {
        if (GPIO_getInputPinValue (GPIO_PORT_P1,GPIO_PIN1) == GPIO_INPUT_PIN_LOW) {
            j |= 0x0001;
            j = j << 1;
        }

        // Delay loop
        for (i = 1000; i > 0; i--);
    }

    return;
}

int
main (void)
{
    /* Stop Watchdog  */
    WDT_A_hold (WDT_A_BASE);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,GPIO_PIN1);

    while(1)
    {

        debounce();
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
        while(GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN1)==GPIO_INPUT_PIN_LOW);

        debounce();
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        while(GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN1)==GPIO_INPUT_PIN_LOW);

    }
}

