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
 * MSP432 Watchdog Timer Example
 *
 * Description: Watchdog timer reset is set at 1 sec; upon reset, RED LED2 lights up, GREEN LED2 is OFF
 *              Matrix multiplication with different matrix sizes is performed
 *              Upon completion of matrix multiply, GREEN LED2 lights up, RED LED2 switched OFF
 *              If matrix size is greater than 40, the multiply is not completed within 1 sec and
 *              LED2 stays RED. If matrix size is less than 40 multiply is completed within 1 sec
 *              and LED2 becomes GREEN until watchdog reset and again becomes RED
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
#define MATRIX_SIZE 45

int main(void)
{
    const int A[MATRIX_SIZE][MATRIX_SIZE];
        const int B[MATRIX_SIZE][MATRIX_SIZE];
        int C[MATRIX_SIZE][MATRIX_SIZE];


    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1);
    GPIO_setDriveStrengthHigh(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);

    CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ);// REFOCLK is 32 KHz
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_4);// ACLK is 8192 Hz
    SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
    WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,WDT_A_CLOCKITERATIONS_8192);//Watchdog reset at 1 sec
    WDT_A_startTimer();

    int sum;
           unsigned int i, j, k;
           for (i = 0; i < MATRIX_SIZE; i++) {
               for (j = 0; j < MATRIX_SIZE; j++) {
                   sum = 0;
                   for (k = 0; k < MATRIX_SIZE; k++) {
                       sum += A[i][k] * B[k][j];
                       //delaymsecs(10);
                   }
                   C[i][j] = sum;
               }
           }
           WDT_A_holdTimer();
           GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
           GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
}
