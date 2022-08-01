/* Lab 6 Part 3 by G09 */
/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* User-specific includes. */
#include "uart_functions.h"

/* Size of the N x N matrix and the min and max values in the matrix. */
#define MATRIX_SIZE 40
#define MIN_VALUE 5
#define MAX_VALUE 10

void matrixMult (int *, int *, int *, unsigned int);
void populateMatrix (int *, int, int, unsigned int);
void tic (void);
void toc (void);
void delay (unsigned int);

/* Continuous timer operation. */
const Timer_A_ContinuousModeConfig continuousModeConfig = {
    TIMER_A_CLOCKSOURCE_ACLK,             // Clock source ACLK
    TIMER_A_CLOCKSOURCE_DIVIDER_1,       // Clock divider 1
    TIMER_A_TAIE_INTERRUPT_ENABLE,         // TAIE interrupt enabled
    TIMER_A_DO_CLEAR
};

/* Data structures for the matrices. */
int A[MATRIX_SIZE * MATRIX_SIZE];
int B[MATRIX_SIZE * MATRIX_SIZE];
int C[MATRIX_SIZE * MATRIX_SIZE];

float elapsedTime;
uint16_t overflow=0;
uint16_t count1, count2;

int main (void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the high-speed clock system. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); /* MCLK, freq = 12 MHz / divider */
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Set SMCLK to 12MHz for UART comms.

    /* Set REFO to 128 KHz */
    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    /* Initialize ACLK of REFO with Divider of 1, so freq ACLK = 128 KHz */
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Configure Timer_A0 in continuous-mode. */
    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    /* Enable the interrupt processing system for the timer. */
    Interrupt_enableInterrupt(INT_TA0_N); // Invoke ISR when Timer_A0 resets to zero
    /* Start the timers. */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    srand (rand()); /* Seed the random number generator. */

    while (1) {
        populateMatrix (A, MIN_VALUE, MAX_VALUE, MATRIX_SIZE); /* Populate matrices with random numbers. */
        populateMatrix (B, MIN_VALUE, MAX_VALUE, MATRIX_SIZE);

        writeString ("\n\rMultiplying matrices");

        tic (); /* Start timer */
        matrixMult(A, B, C, MATRIX_SIZE);
        toc(); /* Stop timer */

        writeString ("\n\rDone multiplying");

        /* Calculate elapsted time with overflow, frequency of Timer A = ALCK = 128 KHz. Scale by 1000 to convert to ms */
        elapsedTime = (float)( count2 - count1 + 0xFFFF*overflow ) / (128.0f);
        /* Display elapsed time on the terminal. */
        writeFloat(elapsedTime);

        delay (1000); /* Delay 1s before starting again. */
   }
}

/* Function populates the input matrices with random FP values. */
void populateMatrix (int *M, int min, int max, unsigned int dim) {
   unsigned int i, j;
   for (i = 0; i < dim; i++) {
      for (j = 0; j < dim; j++) {
         M[i * dim + j] = ceil (min + (max - min) * rand ()/(float) RAND_MAX);
      }
   }
   return;
}

/* Function performs the matrix multiplication C = A x B */
void matrixMult (int *A, int *B, int *C, unsigned int dim) {
    unsigned int i, j, k;
    int temp;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            temp = 0.0;
            for (k = 0; k < dim; k++)
                temp += A[i * dim + k] * B[k * dim + j];

            C[i * dim + j] = temp;
        }
    }

    return;
}

/* The tic function. */
void tic (void) {
    /* Get starting time and set overflow counter to 0 */
    count1 = Timer_A_getCounterValue(TIMER_A0_BASE);
    overflow=0;
    return;
}

/* The toc function. */
void toc (void) {
    /* Get ending time */
    count2 = Timer_A_getCounterValue(TIMER_A0_BASE);
    return;
}


/* Function implements a delay for the specified number of millseconds. */
void delay (unsigned int msecs) {
    unsigned int i;
    for (i = 0; i < 275 * msecs; i++);
    return;
}

/* ISR for Timer_A0 to catch the TAIFG bit. Toggle RED LED connected to P1.0.
 * In continuous mode, the 16-bit timer counts up until it reaches its maximum value of
 * 0xFFFF or 65535 in decimal, then restarts again from zero. The Timer\_A Interrupt FlaG
 * or TAIFG bit is set when the counter value changes from 0xFFFF to zero, which will
 * trigger the ISR below. */
void TA0_N_IRQHandler (void) {
    Timer_A_clearInterruptFlag (TIMER_A0_BASE);
    /* Increment overflow counter */
    overflow++;
    return;
}
