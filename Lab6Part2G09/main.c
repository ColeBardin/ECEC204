/* Lab 6 Part 2 by G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* User-defined includes. */
#include "uart_functions.h"

#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3
#define DELAY 0.1

uint16_t count1, count2;
float time;
uint8_t s1, s2;
uint8_t flag=0;
uint16_t n=0;

/* Configuration for timer in continuous mode. */
const Timer_A_ContinuousModeConfig continuousModeConfig = {
    TIMER_A_CLOCKSOURCE_ACLK, // Clock source
    TIMER_A_CLOCKSOURCE_DIVIDER_1, // Divider ratio
    TIMER_A_TAIE_INTERRUPT_ENABLE, // Enable TAIE interrupt
    TIMER_A_DO_CLEAR
};

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

void delay (uint8_t msecs) {
    uint32_t i;
    for (i = 0;i < 275 * msecs; i++);
    return;
}

int main(void) {
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Initialize the clock system. Do not change the below configuration. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // DCO = 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");

    /* Configure BUTTON1 (P1.1) and BUTTON2 (P1.4). */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN4);

    /* Enable LED1 (P1.0) to display after S1 is pressed */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configure Timer_A0 in continuous-mode. */
    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    /* Enable the interrupt processing system for the timer. */
    Interrupt_enableInterrupt(INT_TA0_N); // Invoke ISR when Timer_A0 resets to zero
    /* Start the timers. */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    while(1) {
        /* If S1 has been pressed */
        if (flag){
            /* Debounce S2 */
            s2 = getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4);
            if (s2 == SWITCH_PRESSED) {
                while (getDebouncedSwitchState(s2, GPIO_PORT_P1, GPIO_PIN4) != SWITCH_RELEASED);
                s2 = SWITCH_RELEASED;
                /* Reset flag */
                flag=0;
                /* Get current timer value for count2 */
                count2=Timer_A_getCounterValue(TIMER_A0_BASE);
                /* Calculate time between presses and write to UART terminal */
                /* TODO: Reevaluate equation */
                time = (float)(n*0xFFFF+count2-count1) / 32000.0; /* Divider and prescaler = 1, so frequency is 32 KHz */
                writeFloat(time);
                /* Turn off Red LED */
                GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            }
        }
        /* Debounce S1 */
        s1 = getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1);
        if (s1 == SWITCH_PRESSED) {
            while (getDebouncedSwitchState(s1, GPIO_PORT_P1, GPIO_PIN1) != SWITCH_RELEASED);
            s1 = SWITCH_RELEASED;
            /* Get current timer value for count1 */
            count1=Timer_A_getCounterValue(TIMER_A0_BASE);
            /* Reset flag and overflow counter */
            flag=1;
            n=0;
            /* Turn on Red LED */
            GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
        }
    }
}


/* ISR for Timer_A0 to catch the TAIFG bit. Toggle RED LED connected to P1.0.
 * In continuous mode, the 16-bit timer counts up until it reaches its maximum value of
 * 0xFFFF or 65535 in decimal, then restarts again from zero. The Timer\_A Interrupt FlaG
 * or TAIFG bit is set when the counter value changes from 0xFFFF to zero, which will
 * trigger the ISR below. */
void TA0_N_IRQHandler (void) {
    Timer_A_clearInterruptFlag (TIMER_A0_BASE);
    /* Increment overflow counter */
    n++;
    return;
}
