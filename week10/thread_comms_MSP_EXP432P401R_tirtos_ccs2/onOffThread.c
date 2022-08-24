/* Dsecription: Thread turns LED1 on and off as dictated by user input. */

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <mqueue.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"

void *onOffThread(void *arg0)
{
    /* Message queue descriptor */
    mqd_t *mqdes = (mqd_t *)arg0;
    int msg;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pin  */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Wait to receive a message from uartThread. If the queue is empty,
     * by default mq_receive blocks until a message becomes available.
     */
    while (mq_receive(*mqdes, (char *)&msg, sizeof(msg), NULL) != -1) {
        if (msg == 0)
            /* Turn off user LED */
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        else if (msg == 1)
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }

    return (void *)0;
}
