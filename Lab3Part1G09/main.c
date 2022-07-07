/* Lab3 Part1 G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1
#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3
#define DELAY 1000
#define NUMBER 500000
#define D_INTERVAL 0.2

unsigned int led_period = 5;

/* Function prototype definitions. */
unsigned int getSwitchState (void);
unsigned int getDebouncedSwitchState (unsigned int);

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

/* Get the debounced state of the switch. */
unsigned int
getDebouncedSwitchState (unsigned int previousState)
{
    unsigned int currentState = getSwitchState(); /* Get current state of the switch. */
    if (currentState == previousState) /* State is unchanged. */
        return previousState;

    /* Instantaneous state has changed. Wait for it to stabilize using
     * debouncing algorithm. The state has to remain unchanged for four
     * consecutive sampling periods. */
    unsigned int i = 0, j = 0;
    unsigned int nextState;

    while (j != 0x001E) {
        nextState = getSwitchState ();
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

/* Returns instantaneous state of the switch. */
unsigned int
getSwitchState (void)
{
    if (GPIO_getInputPinValue (GPIO_PORT_P1,GPIO_PIN1) == GPIO_INPUT_PIN_LOW)
        return SWITCH_PRESSED;
    else
        return SWITCH_RELEASED;
}
void delaysecs(float secs)
{
    uint32_t i;
    for (i=0;i<NUMBER*secs;i++);
    return;
}
void putString(char message[])
{
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

    /* Buffer for formatted string */
    char message[128];
    unsigned int switchState = SWITCH_RELEASED;

    // P1.2 and P1.3 are UART TXD and RXD. These pins must be put in special function mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
    //Sets DCO frequency at 12 MHz to get correct baud rate of 9600
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableMaster();

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); /* Register P2.0 as output (LED2 Red) */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1); /* Register P1.1 as input (Switch 1) */

    /* Display initial status of led_period to screen */
    sprintf(message, "led_period is %d", led_period);
    putString(message);

    do {
        delaysecs( (float)(led_period)*D_INTERVAL ); /* Delay the board operation for led_period*0.2 seconds */
        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
        /* Obtain the debounced state of switch. */
        switchState = getDebouncedSwitchState (switchState); /* Debounce the switch */
        if (switchState == SWITCH_PRESSED) { /* If pressed */
            if (led_period == 1) { /* Reset back to period 5 after period 1 */
                led_period = 5;
            } else {
                led_period--; /* Decrement the period if it is not 1 */
            }
            /* When changing the period, display new period to screen */
            sprintf(message, "led_period is %d", led_period);
            putString(message);
            /* Wait for switch to be released. */
            while (getDebouncedSwitchState (switchState) != SWITCH_RELEASED);
            switchState = SWITCH_RELEASED;
        }
    }
    while(1);
}
