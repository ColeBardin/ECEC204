/* Description: Incorporates debounce on Switch S1
 * If S1 is pressed and then released, green LED2 comes ON
 * If S1 is pressed again and then released, green LED2 goes OFF
 *
 *
 * Author: Giri Prabhu and Naga Kandasamy
 * Date created: July 2, 2019
 * Date modified:
*******************************************************************************/

/* DriverLib Includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes. */
#include <stdint.h>
#include <stdbool.h>
#include "uart_functions.h"

#define FALSE 0
#define TRUE 1
#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3
#define DELAY 1000

/* Function prototype definitions. */
unsigned int getSwitchState (void);
unsigned int getDebouncedSwitchState (unsigned int);

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

int
main (void)
{
    /* Stop Watchdog timer. */
    WDT_A_hold (WDT_A_BASE);

    /* Set P2.1 which is connected to the green LED as output pin. */
    GPIO_setAsOutputPin (GPIO_PORT_P2, GPIO_PIN1);

    /* Configure P1.1 as input pin. Switch S1 is connected to that pin.
     * The input is active low: 0V is true and 5V is false.
     * So, we must connect a pull-up resistor to the pin to make sure
     * that correct default value is supplied in the absence of input.
     */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    unsigned int switchState = SWITCH_RELEASED;
    unsigned int led_on = FALSE;
    while (1) {
        /* Obtain the debounced state of switch. */
        switchState = getDebouncedSwitchState (switchState);
        if (switchState == SWITCH_PRESSED) {
            writeString("\n \r PRESSED");

            /* Wait for switch to be released. */
            while (getDebouncedSwitchState (switchState) != SWITCH_RELEASED);
            switchState = SWITCH_RELEASED;
            writeString ("\n \r RELEASED");

            /* Operate green LED. */
            if (led_on == FALSE) {
                GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN1); /* Turn LED on. */
                led_on = TRUE;
            }
            else {
                GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN1); /* Turn LED off. */
                led_on = FALSE;
            }
        }
    }
}
