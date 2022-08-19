/* Lab Exam 02 by Cole Bardin */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

uint8_t purple = 0;
uint8_t flag=1;

#define TIMER_PERIOD 3000000

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    PCM_setPowerState(PCM_AM_LDO_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0,2);
    FlashCtl_setWaitState(FLASH_BANK1,2);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION); /* NEEDED to set MCLK to 48 MHz */
    CS_setExternalClockSourceFrequency(32000,CS_48MHZ);
    CS_startHFXT(false);
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1); /* MCLK at 48 MHz = 48 MHz HFXTCLK / 1 Divider */

    /* Setup all 3 colors of LED2 */
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);

    /* Init both T32 modules at 3 MHz = 48 MHz MCLK / 16 Prescaler */
    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    Timer32_initModule(TIMER32_1_BASE,TIMER32_PRESCALER_16,TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    /* Set T32 counts */
    Timer32_setCount(TIMER32_0_BASE,TIMER_PERIOD); /* 3M macro for 1 second interrupt intervals */
    Timer32_setCount(TIMER32_1_BASE,10*TIMER_PERIOD); /* Using 10x 3M macro for 10 second interrrupt intervals */

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    Interrupt_enableInterrupt(TIMER32_1_INTERRUPT);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Timer32_enableInterrupt(TIMER32_1_BASE);
    Interrupt_enableMaster();

    /* Start T32 modules */
    Timer32_startTimer(TIMER32_0_BASE,false);
    Timer32_startTimer(TIMER32_1_BASE,true); /* Start T32_1 in oneshot mode */
    PCM_setPowerState(PCM_LPM0_LDO_VCORE1);

}

void T32_INT1_IRQHandler(void){
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    /* Flag for toggling one LED at a time */
    if (flag) {
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);
        flag=0;
    }
    /* Check global status of T32_1 */
    if (purple){
        /* Flash Blue and Purple */
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN0);
    } else {
        /* Flash Red and Green */
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);
    }

}

void T32_INT2_IRQHandler(void){
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
    /* Call flag to change flash colors */
    purple=1;
}
