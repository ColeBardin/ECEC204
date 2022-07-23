/* Lab Exam 01 by Cole Bardin */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define NUMBER      750000
#define DELAY       0.1


void delaysecs(float secs) {
    uint32_t i;
    for (i=0;i<NUMBER*secs;i++);
    return;
}

void off() {
    /* Turn off all LED2 */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
}

void fCyan() {
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
    delaysecs(DELAY);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
}

void fPurple() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
    delaysecs(DELAY);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
}

void sWhite() {
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
}

uint8_t state=3;

 void main() {
     /* Stop Watchdog Timer */
     WDT_A_holdTimer();

     /* Set switch pins as inputs with pullup resistors */
     GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1); /* Register P1.1 as input (Switch 1) */

     /* Set output pins for RBG LED2 as outputs */
     GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); /* Register P2.0 as output (red) */
     GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); /* Register P2.1 as output (green) */
     GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); /* Register P2.2 as output (blue) */

     /* Start with all LED colors being off */
     GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
     GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
     GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);

     while(1) {
         if (GPIO_getInputPinValue (GPIO_PORT_P1,GPIO_PIN1) == GPIO_INPUT_PIN_LOW) {
             while(GPIO_getInputPinValue (GPIO_PORT_P1,GPIO_PIN1) == GPIO_INPUT_PIN_LOW);
             if((state==0)||(state==2))
                 state^=0x03;
             else if((state==1)||(state==3))
                 state&=0x02;
         }

     switch (state) {
     case 0: off(); break;
     case 1: fPurple(); break;
     case 2: fCyan(); break;
     case 3: sWhite(); break;
     }
     delaysecs(DELAY);
     }
 }
