/* Description: Program illustrating the use of semaphores to synchronize thread
 * execution.
 *
 * The uartThread accepts user input from the keyboard and uses a semaphore to
 * signal that event to the onOff thread that controls LED1.
 *
 * If the user enters 'F' in the terminal, LED1 is turned off.
 * If the user enters 'O' in the terminal, LED1 is turned on.
 *
 * Author: Naga Kandasamy
 * Date created: August 27, 2019
 * Date modified: March 8, 2020
 */

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/Board.h>

extern void *onOffThread (void *);
extern void *uartThread (void *);

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

/* The semaphore used for signaling purposes. */
sem_t hold;
int msg; // The message variable shared between the two threads

int
main (void)
{
    pthread_t onOffThreadID, uartThreadID;
    pthread_attr_t threadAttribute;
    struct sched_param threadPriority;
    int status;

    /* Initialize board functions */
    Board_init ();

    /* Initialize semaphore for signaling between threads.
     * The semaphore is shared between threads and is initialized
     * with a value of 0. */
    if (sem_init (&hold, 0, 0) != 0){
        /* sem_init() failed */
        while (1);
    }

    /* Create thread to read user input from the terminal */
    pthread_attr_init (&threadAttribute);
    threadPriority.sched_priority = 2;
    pthread_attr_setschedparam (&threadAttribute, &threadPriority);
    pthread_attr_setdetachstate (&threadAttribute, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize (&threadAttribute, THREADSTACKSIZE);
    status = pthread_create (&uartThreadID, &threadAttribute, uartThread, NULL);
    if (status != 0) {
        /* pthread_create() failed */
        while (1) {}
    }

    /* Create thread to control LED1 */
    threadPriority.sched_priority = 1;
    pthread_attr_setschedparam (&threadAttribute, &threadPriority);
    status = pthread_create (&onOffThreadID, &threadAttribute, onOffThread, NULL);
    if (status != 0) {
        /* pthread_create() failed */
        while (1) {}
    }

    /* Start RTOS scheduler */
    BIOS_start ();

    return (0);
}
