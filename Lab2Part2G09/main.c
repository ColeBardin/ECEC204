/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Defines */
#define LED BIT4
#define B1 BIT0
#define NUMBER (750000)


uint8_t oldinterval=0x31; // default value for 1 sec
uint16_t newinterval;
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
//Interrupt handler receives and echoes character typed in serial terminal even if it is out of the valid range 1-5 secs
void EUSCIA0_IRQHandler(void)
{
    uint32_t status=UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    UART_clearInterruptFlag(EUSCI_A0_BASE,status);
    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
    newinterval=UART_receiveData(EUSCI_A0_BASE);
    UART_transmitData(EUSCI_A0_BASE, newinterval);
    UART_transmitData(EUSCI_A0_BASE, '\r');
    UART_transmitData(EUSCI_A0_BASE, '\n');

    }

}
char message[128];
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
void delaysecs(uint8_t secs)
{
    uint32_t i;
    for (i=0;i<NUMBER*secs;i++);
    return;
}
int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    P6->DIR = LED; /* Set P6.4 as output. */
    P6->REN = B1;   /* Enable the pull-up/down register for P6.0. */
    P6->OUT = B1;   /* The pull-up/down register is set as pull-up. */


    // P1.2 and P1.3 are UART TXD and RXD. These pins must be put in special function mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
    //Sets DCO frequency at 12 MHz to get correct baud rate of 9600
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableMaster();


    while(1)
    {
        /* Make sure interval is between 1 and 5 */
        if ( (newinterval > 0x30) & (newinterval < 0x36)) {
            oldinterval = newinterval; /* If so save it as old interval */
            sprintf(message, "LED FLASHING WITH %d SECS INTERVAL", oldinterval - 0x30);
            putString(message);
            newinterval = 0x00;
        }
        if (!(P6->IN & B1) == 0) { /* If P6.0 is not grounded (button not pressed) */
            GPIO_toggleOutputOnPin (GPIO_PORT_P6, GPIO_PIN4); /* Toggle P6.4 */
            delaysecs(oldinterval - 0x30); /* Delay for oldinterval seconds */
        }
    }
}
