/* Lab4Part1G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define NUMBER  750000

int count = 0;

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
    char message[16];
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

    while(1)
    {
       sprintf(message, "%d", count);
       putString(message);
       delaysecs(1);
    }
}


void PORT1_IRQHandler(void) {
    /* TODO: Debounce here */
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN1) {
        count++;
    } else if (status & GPIO_PIN4) {
        count--;
    }
}
