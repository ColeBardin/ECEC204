/* Lab4Part3G09 */

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
uint16_t charCode;

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

    /* Make interrupt handler for P1.4 */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION); /* Enables P1.1 for interrupt */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    /* Make interrupt handler for P1.1 */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION); /* Enables P1.4 for interrupt */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    /* Enable interrupting */
    Interrupt_enableInterrupt(INT_PORT1); /* Port 1 */
    Interrupt_enableMaster();

    /* Set switch pins as inputs with pullup resistors */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1); /* Register P1.1 as input (Switch 1) */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN4); /* Register P1.4 as input (Switch 2) */

    /* Set output pins for RBG LED2 as outputs */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); /* Register P2.0 as output (red) */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); /* Register P2.1 as output (green) */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); /* Register P2.2 as output (blue) */

    /* Start with all LED colors being off */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);

    while(1)
    {
        delaysecs(DELAY);
        /* Allow for UART transmission */
        if ( state == 1 ) {
            switch (charCode) {
            case 0x4f:
            case 0x6f:
                /* O for OFF */
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x52:
            case 0x72:
                /* R for Red */
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x47:
            case 0x67:
                /* G for Green */
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x42:
            case 0x62:
                /* B for Blue */
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x59:
            case 0x79:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x50:
            case 0x70:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x43:
            case 0x63:
                GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            case 0x57:
            case 0x77:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                break;
            }
        }

    }
}

void PORT1_IRQHandler(void) {
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    s1 = getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1);
    if (s1 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1) != SWITCH_RELEASED);
        s1 = SWITCH_RELEASED;
    }
    s2 = getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4);
    if (s2 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4) != SWITCH_RELEASED);
        s2 = SWITCH_RELEASED;
    }
    if (status & GPIO_PIN1) {
        state = 1;
        charCode=0;
    } else if (status & GPIO_PIN4) {
        state = 0;
        /* Turn off all LED */
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
}

//Interrupt handler receives and echoes character typed in serial terminal even if it is out of the valid range 1-5 secs
void EUSCIA0_IRQHandler(void)
    {
        uint32_t status=UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
        UART_clearInterruptFlag(EUSCI_A0_BASE,status);
        if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
            charCode=UART_receiveData(EUSCI_A0_BASE);
            if ( state == 1 ) {
                UART_transmitData(EUSCI_A0_BASE, charCode);
                UART_transmitData(EUSCI_A0_BASE, '\r');
                UART_transmitData(EUSCI_A0_BASE, '\n');
            }
        }

    }
