/* Description: Thread turns LED1 on and off as dictated by user input. */

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <semaphore.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"

extern sem_t hold;
extern int msg;

void *
onOffThread (void *arg0)
{
    /* Call driver init functions */
    GPIO_init ();

    /* Configure the LED pin  */
    GPIO_setConfig (Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    while (1) {
        /* Block on the semaphore until signaled by the uartThread.
         *
         * The sem_wait() function decrements the semaphore value by 1.
         * If the semaphore's value is greater than zero, then the decrement proceeds,
         * and the function returns, immediately.  If the semaphore currently has the
         * value zero, then the call blocks until either it becomes possible to perform
         * the decrement (i.e., the semaphore value rises above 0).
         */
        sem_wait (&hold);

        if (msg == 0)
            /* Turn off user LED */
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        else if (msg == 1)
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }
}
