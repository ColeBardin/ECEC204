/* Lab 6 Part 1 by G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define TIMER_PERIOD 3000000

uint32_t count1, count2;
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

    while (*(message+i)!='\0') {
        UART_transmitData(EUSCI_A0_BASE, *(message+i));
        i++;
    }
    UART_transmitData(EUSCI_A0_BASE, 0x0D);
    UART_transmitData(EUSCI_A0_BASE, 0x0A);
}

int main(void) {
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

    /* Setup code for 32 bit timer from 7.8 */
    PCM_setPowerState(PCM_AM_LDO_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);
    /* Enable LED1 on P1.1 as an output, set to low */
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
    /* Initialize 32 bit timer module */
    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD);
    /* Enable interrupt for 32 bit timer */
    Interrupt_enableSleepOnIsrExit();
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    /* Start timer */
    Timer32_startTimer(TIMER32_0_BASE,false);
    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

    /* Configure input GPIO pins, P1.1 for BUTTON1 and BUTTON2 P1.4. */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_interruptEdgeSelect (GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    Interrupt_enableInterrupt (INT_PORT1);

    Interrupt_enableMaster();

    while(1) {
        /* TODO: Handle flag not being clear */
    }
    /* Disable Port1 Interrupt */
    //Interrupt_disableInterrupt(INT_PORT1);
    /* TODO: Compute time between presses */
}

void INT_PORT1_IRQHandler(void) {
    char message[32];
    sprintf(message, "Int port %d", 1);
    putString(message);
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status == GPIO_PIN1) {
        /* S1 pressed */
        /* Save count 1 from the timer */
        count1 = Timer32_getValue(TIMER32_0_BASE);
    }

    if (status == GPIO_PIN4) {
        /* S2 pressed */
        /* Save count 2 from the timer */
        count2 = Timer32_getValue(TIMER32_0_BASE);
        /* Disables interrupt for P1.1 and P1.4 */
        /* TODO: Clear FLAG mentioned in description */
    }
}

void T32_INT1_IRQHandler(void) {
    /* TODO: Decide if this example code from 7.8 works */
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
    putString("CLK");
}
