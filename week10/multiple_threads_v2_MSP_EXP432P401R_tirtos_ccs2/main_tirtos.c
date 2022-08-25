/* Description: Program illustrating the creation of multiple POSIX threads
 *
 * Author: Naga Kandasamy
 * Date created: August 31, 2019
 * Date modified: March 8, 2020
 */

#include <stdlib.h>
#include <pthread.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/Board.h>

extern void *contractor (void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

int
main (void)
{
    pthread_t tid;
    pthread_attr_t attrs;
    struct sched_param threadPriority;
    int status;

    /* Initialize board functions */
    Board_init ();

    /* Create the contractor thread. */
    pthread_attr_init (&attrs);
    threadPriority.sched_priority = 10;
    pthread_attr_setschedparam (&attrs, &threadPriority);
    pthread_attr_setdetachstate (&attrs, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize (&attrs, THREADSTACKSIZE);

    status = pthread_create (&tid, &attrs, contractor, NULL);
    if (status != 0) {
        /* pthread_create() failed */
        while (1) {}
    }

    /*Start RTOS scheduler */
    BIOS_start ();

    return (0);
}
