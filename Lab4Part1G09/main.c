/* Lab4Part1G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3
#define NUMBER  750000
#define DELAY   0.2

/* Function prototype definitions. */
unsigned int getSwitchState (int port, int pin);
unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin);

int count = 0;
int s1, s2;
char message[16];

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

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    // P1.2 and P1.3 are UART TXD and RXD. These pins must be put in special function mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
    //Sets DCO frequency at 12 MHz to get correct baud rate of 9600
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION); /* Enables P1.1 for interrupt */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION); /* Enables P1.4 for interrupt */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableMaster();

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); /* Register P2.0 as output (red) 6.4 */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); /* Register P2.0 as output (green) 6.5 */

    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);

    while(1)
    {
       delaysecs(DELAY);
    }
}


void PORT1_IRQHandler(void) {
    s1 = getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1);
    if (s1 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1) != SWITCH_RELEASED);
        s1 = SWITCH_RELEASED;
    }
    s2 = getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN4);
    if (s2 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN4) != SWITCH_RELEASED);
        s2 = SWITCH_RELEASED;
    }

    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN1) {
        if (count == 9) {
            count = 0;
        } else {
            count++;
        }
    } else if (status & GPIO_PIN4) {
        if (count == -9) {
            count = 0;
        } else {
        count--;
        }
    }

    if ( count > 0 ) {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
        sprintf(message, "+%d", count);
    }
    else if ( count < 0 ) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        sprintf(message, "%d", count);
    }
    else {
        /* Equal to 0 */
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        sprintf(message, "%d", count);
    }
    putString(message);
}
