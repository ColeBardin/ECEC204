/* Description: Program illustrating the creation of a simple thread that
 * controls the blinking of LED1.
 *
 * Author: Naga Kandasamy
 * Date created: August 19, 2019
 * Date modified: March 8, 2020
 *
 * Notes: Original source is the empty project under TI Drivers > empty > TI-RTOS > CCS Compiler > empty
 */

#include <stdint.h>
/* POSIX Header files */
#include <pthread.h>
/* RTOS header files */
#include <ti/sysbios/BIOS.h>
/* Example/Board Header files */
#include <ti/drivers/Board.h>

extern void *mainThread (void *);

/* Stack size in bytes for threads. */
#define THREADSTACKSIZE    1024

int
main (void)
{
    pthread_t           tid;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 status;

    /* Call driver init functions */
    Board_init ();

    /* Initialize the attributes structure with default values */
    pthread_attr_init (&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    status = pthread_attr_setschedparam (&attrs, &priParam);
    status |= pthread_attr_setdetachstate (&attrs, PTHREAD_CREATE_DETACHED);
    status |= pthread_attr_setstacksize (&attrs, THREADSTACKSIZE);
    if (status != 0) {
        /* failed to set attributes */
        while (1);
    }

    int delay = 2;
    status = pthread_create (&tid, &attrs, mainThread, (void *)delay);
    if (status != 0) {
        /* pthread_create() failed */
        while (1);
    }

    /* Start scheduler. Function does not return. */
    BIOS_start ();

    return (0);
}
