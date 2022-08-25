/* Description: Worker thread code. */

#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "Board.h"

void *
worker (void *arg0)
{
    int tid = (int) arg0;
    int j;

    GPIO_init ();

    /* Configure the LED pins */
    GPIO_setConfig (Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig (Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    GPIO_write (Board_GPIO_LED0, Board_GPIO_LED_ON);
    GPIO_write (Board_GPIO_LED1, Board_GPIO_LED_ON);

    for (j = 0; j < 10; j++) {
        if (tid == 0)
            GPIO_toggle (Board_GPIO_LED0);
        else if (tid == 1)
            GPIO_toggle (Board_GPIO_LED1);

        sleep (1);
    }

    if (tid == 0)
        GPIO_write (Board_GPIO_LED0, Board_GPIO_LED_OFF);
    else if (tid == 1)
        GPIO_write (Board_GPIO_LED1, Board_GPIO_LED_OFF);

    return NULL;
}


