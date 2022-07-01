//PARTIAL CODE FOR LAB WEEK2 Part (i)  Fill in parts of code in RED
/* DriverLib Includes */
// Get includes from Empty Project ; do not type or copy-paste -won't work
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define NUMBER (1000000)
#define RED_LED2 BIT0


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
    GPIO_setAsOutputPin (GPIO_PORT_P2, GPIO_PIN0);
    P2->DIR = RED_LED2; /* Enable bit0 of Port 2 as an output */
    P2->REN = RED_LED2; /* Set P2.0 as pull up */
    P2->OUT = 0x00; /* Turn off Red LED */


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
        }
        delaysecs(oldinterval - 0x30); /* Delay for oldinterval seconds */
        GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN0); /* Toggle P2.0 */
    }
}
