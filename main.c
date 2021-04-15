

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "uart_input.h"


#define MOTOR (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 2*4)))

#define MOTOR_MASK 4  //PF (M1PW6)
/**
 * main.c
 */
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
        // Note UART on port A must use APB
        SYSCTL_GPIOHBCTL_R = 0;
        SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R5;



}

void initPWM()
{
SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1;
SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
    _delay_cycles(3);

    GPIO_PORTF_DIR_R |= MOTOR_MASK;
    GPIO_PORTF_DR2R_R |= MOTOR_MASK;
    GPIO_PORTF_DEN_R |= MOTOR_MASK;
    GPIO_PORTF_AFSEL_R |= MOTOR_MASK;
    GPIO_PORTF_PCTL_R &= GPIO_PCTL_PF2_M;
    GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF2_M1PWM6;

    SYSCTL_SRPWM_R=SYSCTL_SRPWM_R1;
    SYSCTL_SRPWM_R=0;

    PWM1_3_CTL_R=0;
    PWM1_3_GENA_R=PWM_1_GENA_ACTCMPAD_ZERO | PWM_1_GENA_ACTLOAD_ONE;

    PWM1_3_LOAD_R=1048;

    PWM1_INVERT_R=PWM_INVERT_PWM6INV;
    PWM1_3_CMPA_R=0;
    PWM1_3_CTL_R=PWM_1_CTL_ENABLE;
    PWM1_ENABLE_R=PWM_ENABLE_PWM6EN;



}

int main(void)
{
    USER_DATA data;
    initHw();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initPWM();
    while (1)
        {
//        putsUart0("Hello");
            // Get the string from the user
            getsUart0(&data);
            // Echo back to the user of the TTY interface for testing
                   putsUart0(data.buffer);

                   // Parse fields
                   parseFields(&data);

                   PWM1_3_CMPA_R=900;

                   // Echo back the parsed field information (type and fields)

                   putcUart0('\n');
                   putcUart0('\r');
        }
	return 0;
}
