/* Lab 6 Part 1 by G09 */

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
#define TIMER_PERIOD 3000000

/* Function prototype definitions. */
unsigned int getSwitchState (int port, int pin);
unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin);

uint32_t status, count1, count2;
uint8_t s1, s2;

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

unsigned int getSwitchState (int port, int pin) {
    if  (GPIO_getInputPinValue(port, pin) == GPIO_INPUT_PIN_LOW)
        return SWITCH_PRESSED;
    else
        return SWITCH_RELEASED;
}

unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin) {
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

int main(void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    /* Setup code for 32 bit timer from 7.8 */
    PCM_setPowerState(PCM_AM_LDO_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);

    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);

    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD);

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    Timer32_enableInterrupt(TIMER32_0_BASE);

    Timer32_startTimer(TIMER32_0_BASE,false);
    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

    /* Set switch pins as inputs with pullup resistors */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN4); /* Register P1.1 and P1.4 as pulled up inputs */
    /* Make interrupt handler for P1.4 and P1.1 */
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN4);
    /* Enable interrupting */
    Interrupt_enableInterrupt(INT_PORT1); /* Port 1 */
    Interrupt_enableMaster();

    while(1) {
        /* TODO: Handle flag not being clear */
    }
    /* TODO: Compute time between presses */
}

void INT_PORT1_IRQHandler(void) {
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    s1 = getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1);
    if (s1 == SWITCH_PRESSED) {
        /* S1 pressed */
        while (getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1) != SWITCH_RELEASED);
        /* Save count 1 from the timer */
        count1 = Timer32_getValue(TIMER32_0_BASE);
        s1 = SWITCH_RELEASED;
    }

    s2 = getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4);
    if (s2 == SWITCH_PRESSED) {
        /* S2 pressed */
        while (getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4) != SWITCH_RELEASED);
        /* Save count 2 from the timer */
        count2 = Timer32_getValue(TIMER32_0_BASE);
        /* TODO: Clear FLAG mentioned in description */
        s2 = SWITCH_RELEASED;
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
}

void T32_INT1_IRQHandler(void) {
    /* TODO: Decide if this example code from 7.8 works */
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
}
