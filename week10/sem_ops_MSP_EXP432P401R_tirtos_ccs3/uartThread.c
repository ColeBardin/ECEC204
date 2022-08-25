/* Description: Thread to interact with the terminal via the UART. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <semaphore.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "Board.h"

extern sem_t hold;
extern int msg;

void*
uartThread (void *arg0)
{
    const char  welcome[] = "Connected to board\r\n";
    UART_Handle uart;
    UART_Params uartParams;

    /* Call driver init functions */
    UART_init ();

    /* Create a UART with data processing off */
    UART_Params_init (&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.readTimeout = UART_WAIT_FOREVER;
    uartParams.writeTimeout = UART_WAIT_FOREVER;

    uart = UART_open (Board_UART0, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

    UART_write (uart, welcome, sizeof(welcome));

    char input;
    while (1) {
        UART_read (uart, &input, 1); /* Read character from UART */
        if (input == 'F') {
            msg = 0; /* Message to turn off LED1 */
        }
        else if (input == 'O') {
            msg = 1; /* Message to turn on LED1 */
        }

        UART_write (uart, &input, 1); /* Echo typed character back to the terminal */

        /* Unlock the semaphore pointed to by hold. If the onOff thread is blocked on this
         * semaphore, it will be unblocked.
         *
         * The sem_post() function increments the semaphore by 1.  If the semaphore's
         * value consequently becomes greater than zero, then another thread blocked
         * in a sem_wait() call will be woken up.
         */
        sem_post (&hold);
    }
}



