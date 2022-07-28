/* Lab 6 Part 1 by G09 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "uart_functions.h"

#define TIMER_PERIOD 300000000
#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3
#define DELAY 0.1

uint32_t count1, count2;
float time;
uint8_t s1, s2;
uint8_t flag=0;

unsigned int getSwitchState (int port, int pin) {
    if  (GPIO_getInputPinValue(port, pin) == GPIO_INPUT_PIN_LOW)
        return SWITCH_PRESSED;
    else
        return SWITCH_RELEASED;
}

unsigned int getDebouncedSwitchState (unsigned int previousState, int port, int pin) {
    unsigned int currentState = getSwitchState(port, pin); /* Get current state of the switch. */
    if (currentState == previousState) { /* State is unchanged. */
        return previousState;
    }
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

int main(void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Enable UART communication */
    initUART();

    /* Enable LED1 as output */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);

    /* Configure input GPIO pins, P1.1 for BUTTON1 and BUTTON2 P1.4. */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    Interrupt_enableInterrupt(INT_PORT1);

    /* Enable interrupt master switch */
    Interrupt_enableMaster();

    /* Setup code for 32 bit timer from 7.8 */
    PCM_setPowerState(PCM_AM_LDO_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);

    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD);

    /* Start timer */
    Timer32_startTimer(TIMER32_0_BASE,false);

    while(1) {
        if (flag) {
        /* Disable Port1 Interrupt */
        //Interrupt_disableInterrupt(INT_PORT1);
            /* TODO: determine if deivider is correct */
        time = (float)(count1-count2)/3200000.0;
        writeFloat(time);
        flag=0;
        }
    }
}

void PORT1_IRQHandler(void) {
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    /* Debounce S1 */
    s1 = getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1);
    if (s1 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1) != SWITCH_RELEASED);
        s1 = SWITCH_RELEASED;
        count1=Timer32_getValue(TIMER32_0_BASE);
        GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
    }
    /* Debounce S2 */
    s2 = getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4);
    if (s2 == SWITCH_PRESSED) {
        while (getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4) != SWITCH_RELEASED);
        s2 = SWITCH_RELEASED;
        if (!flag) {
            flag=1;
            count2=Timer32_getValue(TIMER32_0_BASE);
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
    }
}
