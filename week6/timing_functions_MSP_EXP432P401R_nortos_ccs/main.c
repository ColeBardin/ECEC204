/* Description: Skeleton program for timing matrix multiplication operation C = A x B.
 *
 * Author: Naga Kandasamy
 * Date created: July 29, 2019
 * Date modified:
*******************************************************************************/

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

/* Data structures for the matrices. */
int A[MATRIX_SIZE * MATRIX_SIZE];
int B[MATRIX_SIZE * MATRIX_SIZE];
int C[MATRIX_SIZE * MATRIX_SIZE];

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the high-speed clock system. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Set SMCLK to 12MHz for UART comms.

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    srand (rand()); /* Seed the random number generator. */

    while (1) {
        populateMatrix (A, MIN_VALUE, MAX_VALUE, MATRIX_SIZE); /* Populate matrices with random numbers. */
        populateMatrix (B, MIN_VALUE, MAX_VALUE, MATRIX_SIZE);

        writeString ("\n\rMultiplying matrices");

        tic (); /* Time the multiplication operation. */
        matrixMult(A, B, C, MATRIX_SIZE);
        toc();

        writeString ("\n\rDone multiplying");

        /* FIXME: Display elapsed time on the terminal. */

        delay (1000); /* Delay 1s before starting again. */
   }
}

/* Function populates the input matrices with random FP values. */
void
populateMatrix (int *M, int min, int max, unsigned int dim)
{
   unsigned int i, j;
   for (i = 0; i < dim; i++) {
      for (j = 0; j < dim; j++) {
         M[i * dim + j] = ceil (min + (max - min) * rand ()/(float) RAND_MAX);
      }
   }
   return;
}

/* Function performs the matrix multiplication C = A x B */
void
matrixMult (int *A, int *B, int *C, unsigned int dim)
{
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
void
tic (void)
{
    /* FIXME: Complete the function. */
    return;
}

/* The toc function. */
void
toc (void)
{
    /* FIXME: Complete the function. */
    return;
}


/* Function implements a delay for the specified number of millseconds. */
void
delay (unsigned int msecs)
{
    unsigned int i;
    for (i = 0; i < 275 * msecs; i++);
    return;
}
