/* Description: Thread to interact with the terminal via the UART.
 */

#include <stdint.h>
#include <stddef.h>
#include <mqueue.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "Board.h"

void *uartThread(void *arg0)
{
    char        input;
    int         msg;
    const char  echoPrompt[] = "Echoing characters:\r\n";
    UART_Handle uart;
    UART_Params uartParams;
    mqd_t *mqdes = (mqd_t *) arg0; /* Message queue descriptor */

    /* Call driver init functions */
    UART_init();

    /* Create a UART with data processing off */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }
    /* Write confirmation prompt to terminal */
    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    /* Loop forever echoing input received from the UART */
    while (1) {
        /* Read in character */
        UART_read(uart, &input, 1);
        /* If input is 2-5 inclusive, write number to msg que */
        switch(input){
        case '2':
            msg=2;
            mq_send(*mqdes, (char *)&msg, sizeof(msg), 0);
            break;
        case '3':
            msg=3;
            mq_send(*mqdes, (char *)&msg, sizeof(msg), 0);
            break;
        case '4':
            msg=4;
            mq_send(*mqdes, (char *)&msg, sizeof(msg), 0);
            break;
        case '5':
            msg=5;
            mq_send(*mqdes, (char *)&msg, sizeof(msg), 0);
            break;
        default: /* If not, do nothing */
            break;
        }
        /* Echo input character in terminal */
        UART_write(uart, &input, 1);
    }
}



