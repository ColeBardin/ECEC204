/* Contains basic functions to read/write integer, floating-point and
 * string data types from/to the UART.
 *
 *  Created on: Jun 22, 2019
 *  Author: Naga Kandasamy
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Other includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uart_functions.h"

/* Configuration parameters to make the eUSCI A UART module to operate with
 * a 9600 baud rate. These values were calculated using the online calculator that TI provides
 * at: software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */

const eUSCI_UART_ConfigV1 uartConfig =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
     78,                                      // BRDIV = 78
     2,                                       // UCxBRF = 2
     0,                                       // UCxBRS = 0
     EUSCI_A_UART_NO_PARITY,                  // No Parity
     EUSCI_A_UART_LSB_FIRST,                  // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
     EUSCI_A_UART_MODE,                       // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

/* Ring buffer to hold the characters received from the UART. */
struct ringBuffer_t {
    char buffer[BUFFER_SIZE];
    uint8_t in;
    uint8_t out;
};
struct ringBuffer_t ringBuffer;

/* EUSCI A0 UART ISR - receives data from the host PC. */
void
EUSCIA0_IRQHandler (void)
{
    char c;

    /* Obtain the current interrupt status of the UART. */
    uint32_t status = MAP_UART_getEnabledInterruptStatus (EUSCI_A0_BASE);

    /* Clear UART interrupt sources. */
    MAP_UART_clearInterruptFlag (EUSCI_A0_BASE, status);

    /* Check if we received anything via the UART. */
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        /* Read the data from the UART module. */
        c = MAP_UART_receiveData (EUSCI_A0_BASE);

        /* Echo it back to the host via the UART. */
        MAP_UART_transmitData (EUSCI_A0_BASE, c);

        /* Store the character in the ring buffer. */
        ringBuffer.buffer[ringBuffer.in++] = c;
        if (ringBuffer.in == BUFFER_SIZE)
            ringBuffer.in = 0;
    }

    return;
}

/* Read a floating-point value from the UART. */
void
readFloat (float *value)
{
    char buf[BUFFER_SIZE];
    readString (buf);
    *value = atof (buf); /* Convert ASCII to floating-point data type. */
    return;
}

/* Write a floating-point value to the UART. */
void
writeFloat (float value)
{
    char buf[BUFFER_SIZE];
    double temp, integer_part, fractional_part;

    /* modf() breaks the FP value into tow parts: the integer part is stored in
     * the pointer location passed as the second argument to the function, and the
     * fractional value is returned by the function.
     */
    temp = modf (value, &integer_part);
    /* Extract the fractional portion of the FP value upto
     * three digits of precision.
     */
    temp = modf (1000 * temp, &fractional_part);
    sprintf (buf, "%d.%d", (int) integer_part, (int) fractional_part);
    writeString (buf);

    return;
}

/* Read an integer from the terminal via the UART. */
void
readInt (int *value)
{
    char buf[BUFFER_SIZE];
    readString (buf);
    *value = atoi (buf); /* Convert ASCII to integer data type. */
    return;
}

/* Write an integer value to the UART. */
void
writeInt (int value)
{
    char buf[BUFFER_SIZE];
    sprintf (buf, "%d", value); /* Convert integer data type to ASCII. */
    writeString (buf);
    return;
}

/* Function reads the characters received by the UART and constructs the
 * resulting string.
 */
void
readString (char *buffer)
{
    char c;
    char *ptr = buffer;

    for (;;) {
        if (ringBuffer.out == ringBuffer.in) /* Ring buffer is empty. */
            continue;
        else {
            c = ringBuffer.buffer[ringBuffer.out++];
            if (ringBuffer.out == BUFFER_SIZE)
                ringBuffer.out = 0;

            if (c == 0x0D) { /* Check for carriage return. */
                *ptr = '\0';
                break;
            }
            else {
                *ptr++ = c;
            }
        }
    }

    return;
}


/* Function writes the provided string to the UART. We print out one
 * character at a time to the UART until the end of string is reached. */
void
writeString (char *buffer)
{
    char *ptr = buffer;
    while (*ptr != '\0') {
        MAP_UART_transmitData (EUSCI_A0_BASE, *ptr);
        ptr++;
    }

    return;
}



void
initUART (void)
{
    /* Initialize the ring buffer. */
    ringBuffer.in = 0;
    ringBuffer.out = 0;

    /* Select P1.2 and P1.3 in UART mode. */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin (GPIO_PORT_P1,
                       GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Set digital clock oscillator (DCO) to 12MHz. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12);

    /* Configure UART Module. */
    MAP_UART_initModule (EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module. */
    MAP_UART_enableModule (EUSCI_A0_BASE);

    /* Enable Interrupts from UART to processor. */
    MAP_UART_enableInterrupt (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt (INT_EUSCIA0);

    MAP_Interrupt_enableMaster ();

    return;
}




