/* Description: Thread that toggles LED1 at specified rate.
 */

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <mqueue.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"

void *blinkyThread(void *arg0)
{
    mqd_t *mqdes = (mqd_t *) arg0; /* Message queue descriptor */
    int msg, i;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pin and turn on LED1  */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);

    while (1) {
        /* If mqrecieve fails, EXIT */
        if (mq_receive(*mqdes, (char *)&msg, sizeof(msg), NULL) == -1){
            break;
        }
        for (i=0; i<msg; i++){
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
            usleep(500000);
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
            usleep(500000);
        }
    }

    return (void *)0;
}
