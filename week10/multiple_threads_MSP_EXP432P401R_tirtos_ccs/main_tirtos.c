/* Description: Program illustrating the creation of multiple POSIX threads
 *
 * Author: Naga Kandasamy
 * Date created: August 25, 2019
 * Date modified: August 16, 2021
 */

#include <stdlib.h>
#include <pthread.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/Board.h>

extern void *blinkyThread(void *arg0);
extern void *uartThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

int main(void)
{
    pthread_t blinkyThreadID, uartThreadID;
    pthread_attr_t attrs;
    struct sched_param threadPriority;
    int status;

    /* Initialize board functions */
    Board_init();

    /* Create thread to toggle LED1 at a rate of once every 2 seconds */
    pthread_attr_init(&attrs);
    threadPriority.sched_priority = 1;
    pthread_attr_setschedparam(&attrs, &threadPriority);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);

    int delay = 1;
    status = pthread_create(&blinkyThreadID, &attrs, blinkyThread, (void *) delay);
    if (status != 0) {
        /* pthread_create() failed */
        while (1);
    }

    /* Create thread to handle terminal functions via the UART.
     * This thread runs at higher priority than the blinky thread.
     * */
    threadPriority.sched_priority = 2;
    pthread_attr_setschedparam(&attrs, &threadPriority);
    status = pthread_create(&uartThreadID, &attrs, uartThread, NULL);
    if (status != 0) {
        /* pthread_create() failed */
        while (1);
    }

    /* Start RTOS scheduler */
    BIOS_start();

    return(0);
}
