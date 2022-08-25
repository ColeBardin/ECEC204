/* Description: Function executed by the thread to toggle LED1 on the board.
 *
 * Author: Naga Kandasamy
 * Date created: AUgust 20, 2019
 * Date modified: March 8, 2020
 */

#include <unistd.h> /* For usleep() */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header file */
#include "Board.h"


void *
mainThread (void *arg0)
{
    /* 1 second delay */
    int time = (int) arg0;

    /* Call driver init function for GPIO. */
    GPIO_init ();

    /* Configure the LED pin. */
    GPIO_setConfig (Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED. */
    GPIO_write (Board_GPIO_LED0, Board_GPIO_LED_ON);

    while (1) {
        sleep (time);
        GPIO_toggle (Board_GPIO_LED0);
    }
}
