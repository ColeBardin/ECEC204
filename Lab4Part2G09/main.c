/* Lab4Part2G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3
#define NUMBER  750000
#define DELAY   0.1

/* Function prototype definitions. */
unsigned int getSwitchState (int port, int pin);
unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin);

uint8_t s1, s2;
uint8_t state = 0;

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

    /* Make interrupt handler for P3.3 */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN3);
    GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN3, GPIO_LOW_TO_HIGH_TRANSITION); /* Enables P1.1 for interrupt */
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN3);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN3);

    /* Make interrupt handler for P4.1 */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION); /* Enables P1.4 for interrupt */
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN1);

    /* Enable interrupting */
    Interrupt_enableInterrupt(INT_PORT3); /* P3.3 */
    Interrupt_enableInterrupt(INT_PORT4); /* P4.1 */
    Interrupt_enableMaster();

    /* Set switch pins as inputs with pullup resistors */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P3, GPIO_PIN3); /* Register P3.3 as input (Switch 1) */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P4, GPIO_PIN1); /* Register P4.1 as input (Switch 2) */

    /* Set output pins for RBG LED as outputs */
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4); /* Register P6.4 as output (red) */
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN5); /* Register P6.5 as output (green) */
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6); /* Register P6.6 as output (blue) */

    /* Start with all LED colors being off */
    GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);

    while(1)
    {
        delaysecs(DELAY);
        /* Enable lighting based on state */
        switch (state) {
        case 0:
            /* Off */
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 1:
            /* Red */
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 2:
            /* Green */
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 3:
            /* Blue */
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 4:
            /* Yellow */
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 5:
            /* Purple */
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 6:
            /* Cyan */
            GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        case 7:
            /* White */
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN6);
            break;
        }
    }
}

void PORT3_IRQHandler(void) {
    char message[32];
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    /* Debounce S1 */
    s1 = getDebouncedSwitchState(s1, GPIO_PORT_P3, GPIO_PIN3);
    if (s1 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s1, GPIO_PORT_P3, GPIO_PIN3) != SWITCH_RELEASED);
        s1 = SWITCH_RELEASED;
    }
    /* Logic for updating state */
    if (status & GPIO_PIN3) {
        if (state%2 == 0) {
            state++;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    sprintf(message, "State: %d", state);
    putString(message);
}

void PORT4_IRQHandler(void) {
    char message[32];
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);
    /* Debounce S2 */
    s2 = getDebouncedSwitchState(s1, GPIO_PORT_P4, GPIO_PIN1);
    if (s2 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s1, GPIO_PORT_P4, GPIO_PIN1) != SWITCH_RELEASED);
        s2 = SWITCH_RELEASED;
    }
    /* Logic for updating state */
    if (status & GPIO_PIN1) {
        if (state%2 == 1) {
            if (state != 7) {
                state++;
            } else {
                state=0;
            }
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
    sprintf(message, "State: %d", state);
    putString(message);
}
