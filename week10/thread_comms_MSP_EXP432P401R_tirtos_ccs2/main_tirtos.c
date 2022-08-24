/* Description: Program illustrating the creation of multiple POSIX threads.
 *
 * The uartThread accepts user input from the keyboard and passes it along as
 * a message to the onOff thread that control LED1.
 *
 * If the user enters 'F' in the terminal, LED1 is turned off.
 * If the user enters 'O' in the terminal, LED1 is turned on.
 *
 * Author: Naga Kandasamy
 * Date created: August 25, 2019
 * Date modified: August 16, 2021
 */

#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/Board.h>

extern void *onOffThread(void *);
extern void *uartThread(void *);

/* Stack size in bytes */
#define THREADSTACKSIZE    1024
#define MSG_SIZE sizeof(int)
#define MSG_NUM 4

/* Message queue variable */
mqd_t mqdes;

int main(void)
{
    pthread_t onOffThreadID, uartThreadID;
    pthread_attr_t threadAttribute;
    struct sched_param threadPriority;
    int status;

    /* Initialize board functions */
    Board_init();

    /* Create the message queue for inter-thread communication */
    struct mq_attr mqAttribute;
    mqAttribute.mq_maxmsg = MSG_NUM;
    mqAttribute.mq_msgsize = MSG_SIZE;
    mqAttribute.mq_flags = 0;
    mqdes = mq_open("mailbox", O_RDWR | O_CREAT, 0664, &mqAttribute);
    if (mqdes == (mqd_t)-1) {
      /* mq_open() failed */
      while (1);
    }

    /* Create thread to read user input from the terminal */
    pthread_attr_init(&threadAttribute);
    threadPriority.sched_priority = 2;
    pthread_attr_setschedparam(&threadAttribute, &threadPriority);
    pthread_attr_setdetachstate(&threadAttribute, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&threadAttribute, THREADSTACKSIZE);
    status = pthread_create(&uartThreadID, &threadAttribute, uartThread, (void *)&mqdes);
    if (status != 0) {
        /* pthread_create() failed */
        while (1);
    }

    /* Create thread to control LED1 */
    threadPriority.sched_priority = 1;
    pthread_attr_setschedparam(&threadAttribute, &threadPriority);
    status = pthread_create(&onOffThreadID, &threadAttribute, onOffThread, (void *)&mqdes);
    if (status != 0) {
        /* pthread_create() failed */
        while (1);
    }

    /*Start RTOS scheduler */
    BIOS_start();

    return(0);
}
