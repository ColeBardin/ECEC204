/* Lab 8 Part 2 by G09 */
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>\

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define CAN         0x18    /* Cancel in ASCII (24) */
#define N_ATTEMPTS  3       /* Number of passcode attempts after device is locked */
#define TIMEOUT     10       /* Timeout duration in seconds */

char charCode;
char pass[6]; /* String where usable password will be stored since it needs to be in main stack frame */

uint8_t state=0; /* State of lock, 0=UNLOCKED, 1=LOCKED */
uint8_t input=0; /* Status used by UART Interrupt to tell getButton to wait until new value is input */
uint8_t fails=0; /* Counter for number of fails */
uint8_t flash=0; /* State of flashing LED when changing passcode */
uint8_t timeout=0; /* State for whether or not to count timeouts */
uint16_t start_time, end_time, elapsed_time;
unsigned int ovf=0;

/* Configuration for timer in continuous mode. */
const Timer_A_ContinuousModeConfig continuousModeConfig = {
    TIMER_A_CLOCKSOURCE_ACLK, // Clock source
    TIMER_A_CLOCKSOURCE_DIVIDER_1, // Divider ratio
    TIMER_A_TAIE_INTERRUPT_ENABLE, // Enable TAIE interrupt
    TIMER_A_DO_CLEAR
};

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

void
writeString (char *buffer)
{
    MAP_UART_transmitData (EUSCI_A0_BASE, '\n');
    MAP_UART_transmitData (EUSCI_A0_BASE, '\r');

    char *ptr = buffer;
    while (*ptr != '\0') {
        MAP_UART_transmitData (EUSCI_A0_BASE, *ptr);
        ptr++;
    }

    MAP_UART_transmitData (EUSCI_A0_BASE, '\n');
    MAP_UART_transmitData (EUSCI_A0_BASE, '\r');
    return;
}

/* Waits for input flag to be set by UART INT upon input. Clears flag and returns character input */
char getButton(){
    while(!input){
        if (timeout){
            end_time =Timer_A_getCounterValue(TIMER_A0_BASE);
            elapsed_time=(float)(ovf*0xFFFF+end_time-start_time)/128000.0f;
            if (elapsed_time > TIMEOUT){
                writeString("Timeout Occured! Start Over");
                writeString("Enter 5 Digit Passcode");
                return CAN; // If Counter exceeds overflow, return CAN (cancel in ASCII)
            }
        }
    }
    input=0;
    return charCode;
}

/* Matches user input to the current passcode, 0=FAIL, 1=SUCCESS */
int getCombination() {
    int index=0;
    char input;
    /* Iterate for each index of the passcode */
    for (index=0; index<5; index++) {
        input = getButton();
        if (input==CAN) {
            index=-1;
            timeout=0; // Disable timeout for first input
            continue;
        }
        if (input != pass[index]){ /* Get button pushed by user via UART. */
            /* If any do not match, FAIL */
            timeout=0; // Disable timeout counting
            return 0;
        }
        if(index==0){
            start_time=Timer_A_getCounterValue(TIMER_A0_BASE);
            timeout=1;
            ovf=0;
        }
    }
    timeout=0; // Disable timeout counting
    /* If all 5 are matched, return SUCCESS */
    return 1;

}

/* Change passcode, must be entered twice. 0=FAIL, 1=SUCCESS */
int setPasscode(){
    char new[6];
    new[5]='\0'; /* Null terminate uninitialized 5-char string */
    char msg[256];
    int index;
    flash=1;
    /* Get new passcode from user */
    writeString("Enter New 5 Digit Passcode");
    for (index=0; index<5; index++){
        new[index] = getButton();
    }
    /* Validate re-entering of passcode */
    writeString("Re-Enter Newpasscode");
    for (index=0; index<5; index++){
        if (new[index] != getButton()){
            writeString("Aborted! Passcodes don't match");
            flash=0; // Stop flashing LED
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0); // Ensure LED is ON since dvice is LOCKED
            return 0;
        }
    }
    /* On successful validation, edit pass variable */
    sprintf(msg, "Success! Passcode changed to: %s", new);
    writeString(msg);
    strcpy(pass, new);
    flash=0; // Stop flashing LED
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // Ensure LED is OFF since device is UNLOCKED
    return 1;
}

int main(void)
{
    char msg[64];
    char ret;
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // SMCLK at 12 MHz for UART

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_128KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference
    /* Configure Timer_A0 in continuous-mode. */
    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    /* Enable the interrupt processing system for the timer. */
    Interrupt_enableInterrupt(INT_TA0_N); // Invoke ISR when Timer_A0 resets to zero
    /* Start the timers. */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    // P1.2 and P1.3 are UART TXD and RXD. These pins must be put in special function mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);;
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);

    /* Enable Red LED1 */
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);

    /* Copy string literal into editable var */
    strcpy(pass, "24153");
    writeString("Device is UNLOCKED");

    while(1) {
        if (state==0) { /* UNLOCKED State */
            writeString("Press '1' to LOCK or '2' to Change Passcode");
            while(1){
                ret = getButton();
                if (ret=='1') {
                    /* LOCK and enable Red LED */
                    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
                    writeString("Locked");
                    state=1; /* Change state to be locked */
                    break;
                } else if (ret=='2'){
                    /* Change passcode */
                    if (setPasscode()) {
                        /* On success, keep UNLOCKED */
                        break;
                    } else {
                        /* On fail, LOCK device */
                        GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
                        writeString("Locked");
                        state=1;
                        break;
                    }
                }
            }
        } else { /* LOCKED State */
            writeString("Enter 5 Digit Passcode");
            /* Get passscode from user */
            while (!getCombination()) {
                /* On fail, increment fail counter */
                fails++;
                /* Check if they used all attempts */
                if (fails==N_ATTEMPTS) {
                    /* If so, hault the code */
                    writeString("Permanently Locked");
                    while(1);
                }
                /* Else output number of remaining attempts */
                sprintf(msg, "Fail. %d tries remaining", N_ATTEMPTS-fails);
                writeString(msg);
            }
            /* On success, UNLOCK the device and turn off Red LED */
            writeString("Unlocked");
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            /* Reset state and fail counter */
            state=0;
            fails=0;
        }
    }
}

void TA0_N_IRQHandler (void) {
    Timer_A_clearInterruptFlag (TIMER_A0_BASE);
    /* Increment overflow counter */
    ovf++;
    if (flash){
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
    return;
}

//Interrupt handler receives and echoes character typed in serial terminal even if it is out of the valid range 1-5 secs
void EUSCIA0_IRQHandler(void)
{
    uint32_t status=UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    UART_clearInterruptFlag(EUSCI_A0_BASE,status);
    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        charCode=UART_receiveData(EUSCI_A0_BASE);
        input=1;
        if (fails!=N_ATTEMPTS){ /* Only mirror to terminal if there are sill attempts left, do not mirror when permanenetly locked */
            UART_transmitData(EUSCI_A0_BASE, charCode);
            UART_transmitData(EUSCI_A0_BASE, '\r');
            UART_transmitData(EUSCI_A0_BASE, '\n');
        }
    }

}
