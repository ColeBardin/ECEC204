/* Description: Toggling LEDs via the use of nested interrupts.
 *
 * Use an external wire to connect P3.6 to OV (ground) to simulate pressing a
 * switch. P3.6 is configured to be active low; so a button press should
 * supply 0V to the pin.
 *
 * Adapted from code listing 6.11 (p. 101) from the text book.
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy, July 16, 2019
*******************************************************************************/

/* DriverLib includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

#define DELAY 15000

/* Function prototypes. */
void delay_ms (uint16_t);

void
main (void)
{
    /* Disable the watchdog timer. */
    WDT_A_holdTimer ();

    /* Configure the RGB LED on Port P2.0 and P2.1. */
    GPIO_setAsOutputPin (GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
    GPIO_setDriveStrengthHigh (GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
    GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);

    /* Configure input pins, BUTTON1 on P1.1 and a user-defined pin P3.6.
     * Both pins are configured to be active low, that is 0V is logic 1 and
     * 5V is logic 0.
     */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P3, GPIO_PIN6);

    /* Configure and enable the interrupt handling system. */
    GPIO_interruptEdgeSelect (GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt (GPIO_PORT_P1, GPIO_PIN1);

    GPIO_interruptEdgeSelect (GPIO_PORT_P3, GPIO_PIN6, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag (GPIO_PORT_P3, GPIO_PIN6);
    GPIO_enableInterrupt (GPIO_PORT_P3, GPIO_PIN6);

    Interrupt_setPriority (INT_PORT1, 0); /* High priority ISR for PORT1 */
    Interrupt_setPriority (INT_PORT3, 64); /* Low priority ISR for PORT3 */
    Interrupt_enableInterrupt (INT_PORT1);
    Interrupt_enableInterrupt (INT_PORT3);
    Interrupt_enableMaster ();

    while (1);
}

/* ISR for IRQs from GPIO pins in PORT1. */
void
PORT1_IRQHandler (void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus (GPIO_PORT_P1);
    GPIO_clearInterruptFlag (GPIO_PORT_P1,status);

    /* Isolate the status of P1.1. */
    if (status & GPIO_PIN1) {
        GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN0);
        delay_ms (3000);
        GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN0);
    }

    return;
}

/* ISR for IRQ from GPIO pins in PORT3. */
void
PORT3_IRQHandler (void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus (GPIO_PORT_P3);
    GPIO_clearInterruptFlag (GPIO_PORT_P3, status);

    /* Isolate the status of P3.6. */
    if (status & GPIO_PIN6) {
        GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN1);
        delay_ms (3000);
        GPIO_toggleOutputOnPin (GPIO_PORT_P2, GPIO_PIN1);
    }

    return;
}

/* Function that implements a delay in milliseconds. */
void
delay_ms (uint16_t ms)
{
  uint16_t delay;
  uint16_t i;
  for (delay = ms; delay > 0; delay--)
      for (i = 300; i > 0; i--);

  return;
}


