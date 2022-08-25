/* Description: Thread to interact with the terminal via the UART.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "Board.h"

#define BUF_SIZE 32
#define NUM_THREADS 2

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

extern void *worker (void *);

void *
contractor (void *arg0)
{
    char buf[BUF_SIZE];
    const char  welcome[] = "Connected to board\r\n";
    UART_Handle uart;
    UART_Params uartParams;

    pthread_t tid[NUM_THREADS];
    pthread_attr_t attrs;
    struct sched_param threadPriority;
    int status;

    /* Call driver init functions */
    UART_init ();

    /* Create a UART with data processing off */
    UART_Params_init (&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open (Board_UART0, &uartParams);
    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

    UART_write (uart, welcome, sizeof(welcome));

    /* Create worker threads. */
    pthread_attr_init (&attrs);
    threadPriority.sched_priority = 5;
    pthread_attr_setschedparam (&attrs, &threadPriority);
    pthread_attr_setdetachstate (&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize (&attrs, THREADSTACKSIZE);

    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        sprintf (buf, "Creating thread %d \r\n", i);
        UART_write (uart, buf, strlen (buf));
        status = pthread_create (&tid[i], &attrs, worker, (void *)i);
        if (status != 0) {
            /* pthread_create() failed */
            while (1);
        }
    }

    /* Wait for threads to join */
    for (i = 0; i < NUM_THREADS; i++)
        pthread_join (tid[i], NULL);

    strcpy (buf, "Threads completed.\r\n");
    UART_write (uart, buf, strlen (buf));

    while (1);
}



