/* Description: Code for finite state machine (FSM) whose behavior is
 * dictated by BUTTON1 and BUTTON2.
 *
 * Author: Naga Kandasamy
 * Date created: July 5, 2019
*******************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define NUMBER  500000
#define DELAY   0.2
#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3

/* Function prototype definitions. */
unsigned int getSwitchState (int port, int pin);
unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin);

const eUSCI_UART_ConfigV1 uartConfig=   // For CCSv9 and CCSv10 have UART_ConfigV1
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     78,//BRDIV
     2, //UCxBRF
     0,//UCxBRS
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

unsigned int getSwitchState (int port, int pin)
{
    if  (GPIO_getInputPinValue(port, pin) == GPIO_INPUT_PIN_LOW)
        return SWITCH_PRESSED;
    else
        return SWITCH_RELEASED;
}

unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin)
{
    unsigned int currentState = getSwitchState(port, pin); /* Get current state of the switch. */
    if (currentState == previousState) /* State is unchanged. */
        return previousState;

    /* Instantaneous state has changed. Wait for it to stabilize using
     * debouncing algorithm. The state has to remain unchanged for four
     * consecutive sampling periods. */
    unsigned int i = 0, j = 0;
    unsigned int nextState;

    while (j != 0x001E) {
        nextState = getSwitchState (port, pin);
        if (currentState == nextState) {
            j |= 0x0001;
            j = j << 1;
        }
        else
            j = 0;

        currentState = nextState;

        /* Delay. Needs to be tuned by programmer for the debounce
         * algorithm to work correctly. Usually switch specific. */
        for (i = DELAY; i > 0; i--);
    }

    return currentState;
}

void delaysecs(float secs) {
    uint32_t i;
    for (i=0;i<NUMBER*secs;i++);
    return;
}

void putString(char message[]) {
    uint16_t i=0;

    while (*(message+i)!='\0')
    {
        UART_transmitData(EUSCI_A0_BASE, *(message+i));
    i++;
    }
    UART_transmitData(EUSCI_A0_BASE, 0x0D);
    UART_transmitData(EUSCI_A0_BASE, 0x0A);

}

int main(void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    unsigned int state = 0;
    int s1, s2;

    // P1.2 and P1.3 are UART TXD and RXD. These pins must be put in special function mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
    //Sets DCO frequency at 12 MHz to get correct baud rate of 9600
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableMaster();

    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4); /* Register P2.0 as output (red) 6.4 */
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN5); /* Register P2.0 as output (green) 6.5 */
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6); /* Register P2.0 as output (blue) 6.6 */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P6, GPIO_PIN0); /* Register P1.1 as input (Switch 1) 6.0 */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P6, GPIO_PIN1); /* Register P1.4 as input (Switch 2) 6.1 */

    GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);

    while(1)
    {
        s1 = getDebouncedSwitchState(s1, GPIO_PORT_P6, GPIO_PIN0);

        if (s1 == SWITCH_PRESSED) {
            switch (state) {
                case 0:
                    state = 1;
                    putString("State: 1");
                    break;
                case 1:
                    state = 0;
                    putString("State: 0");
                    break;
                case 2:
                    state = 3;
                    putString("State: 3");
                    break;
                case 3:
                    state = 2;
                    putString("State: 2");
                    break;
            }
            while (getDebouncedSwitchState(s1, GPIO_PORT_P6, GPIO_PIN0) != SWITCH_RELEASED);
            s1 = SWITCH_RELEASED;
        }

        s2 = getDebouncedSwitchState(s2, GPIO_PORT_P6, GPIO_PIN1);
        if (s2 == SWITCH_PRESSED) {
                    switch (state) {
                        case 0:
                            state = 2;
                            putString("State: 2");
                            break;
                        case 1:
                            state = 3;
                            putString("State: 3");
                            break;
                        case 2:
                            state = 0;
                            putString("State: 0");
                            break;
                        case 3:
                            state = 1;
                            putString("State: 1");
                            break;
                    }
                    while (getDebouncedSwitchState(s1, GPIO_PORT_P6, GPIO_PIN1) != SWITCH_RELEASED);
                    s2 = SWITCH_RELEASED;
                }

        switch (state) {
            case 0:
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);
                delaysecs(DELAY);
                break;
            case 1:
                GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
                delaysecs(DELAY);
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
                break;
            case 2:
                GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
                delaysecs(DELAY);
                GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
                break;
            case 3:
                GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);
                GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);
                break;
        }
        delaysecs(DELAY);
    }
}
