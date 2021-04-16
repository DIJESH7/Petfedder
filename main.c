

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "uart_input.h"


#define LED (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 2*4)))

#define LED_MASK 4  //PF2 M1PWM6

#define MOTOR_FORWARD 64 //PB6 M0PWM0

#define MOTOR_REVERSE  128 //PB7 M0PWM1


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
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1|SYSCTL_RCGCPWM_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5|SYSCTL_RCGCGPIO_R1;

    _delay_cycles(3);
//    GPIO_PORTB_DIR_R |= MOTOR_FORWARD;
//           GPIO_PORTB_DR2R_R |= MOTOR_FORWARD;
//           GPIO_PORTB_DEN_R |=MOTOR_FORWARD;
//           GPIO_PORTB_AFSEL_R |= MOTOR_FORWARD;
//           GPIO_PORTB_PCTL_R &= GPIO_PCTL_PB6_M;
//           GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB6_M0PWM0;

    GPIO_PORTB_DIR_R |= MOTOR_FORWARD|MOTOR_REVERSE;
        GPIO_PORTB_DR2R_R |= MOTOR_FORWARD|MOTOR_REVERSE;
        GPIO_PORTB_DEN_R |=MOTOR_FORWARD|MOTOR_REVERSE ;
        GPIO_PORTB_AFSEL_R |= MOTOR_FORWARD|MOTOR_REVERSE;
        GPIO_PORTB_PCTL_R &= (GPIO_PCTL_PB6_M|GPIO_PCTL_PB7_M);
        GPIO_PORTB_PCTL_R |= (GPIO_PCTL_PB6_M0PWM0|GPIO_PCTL_PB7_M0PWM1);

    GPIO_PORTF_DIR_R |= LED_MASK;
    GPIO_PORTF_DR2R_R |= LED_MASK;
    GPIO_PORTF_DEN_R |= LED_MASK;
    GPIO_PORTF_AFSEL_R |= LED_MASK;
    GPIO_PORTF_PCTL_R &= GPIO_PCTL_PF2_M;
    GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF2_M1PWM6;

    SYSCTL_SRPWM_R=(SYSCTL_SRPWM_R1|SYSCTL_SRPWM_R0);
    SYSCTL_SRPWM_R=0;

    PWM0_0_CTL_R=0;
    PWM0_0_GENA_R|= PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE;
    PWM0_0_GENB_R|= PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTLOAD_ONE;
    PWM1_3_CTL_R=0;
    PWM1_3_GENA_R|=PWM_1_GENA_ACTCMPAD_ZERO | PWM_1_GENA_ACTLOAD_ONE;

    PWM1_3_LOAD_R=1000;
    PWM1_INVERT_R=PWM_INVERT_PWM6INV;
    PWM0_0_LOAD_R=1048;

   PWM0_INVERT_R|=(PWM_INVERT_PWM0INV|PWM_INVERT_PWM1INV);
    //PWM0_INVERT_R|=PWM_INVERT_PWM0INV;

    PWM1_3_CMPA_R=0;
    PWM0_0_CMPA_R=0;
    PWM0_0_CMPB_R=0;
    PWM0_0_CTL_R=PWM_0_CTL_ENABLE;
    PWM0_1_CTL_R=PWM_0_CTL_ENABLE;

    PWM1_3_CTL_R=PWM_1_CTL_ENABLE;
    PWM1_ENABLE_R=PWM_ENABLE_PWM6EN;

 PWM0_ENABLE_R=PWM_ENABLE_PWM0EN|PWM_ENABLE_PWM1EN;
    //PWM0_ENABLE_R=PWM_ENABLE_PWM0EN;

}

int main(void)
      {
    USER_DATA data;
    initHw();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initPWM();
//    PWM1_3_CMPA_R=20;
//PWM0_0_CMPA_R=200;
//         waitMicrosecond(5000000);
//         PWM0_0_CMPA_R=0;
//         waitMicrosecond(1000000);
 //   PWM0_0_CMPB_R=19;
  //       PWM0_0_CMPA_R=0;
//
//    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1|SYSCTL_RCGCPWM_R0;
//       SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5|SYSCTL_RCGCGPIO_R1;
//
//       _delay_cycles(3);
//       GPIO_PORTB_DIR_R |= MOTOR_FORWARD|MOTOR_REVERSE;
//                 GPIO_PORTB_DR2R_R |= MOTOR_FORWARD|MOTOR_REVERSE;
//                 GPIO_PORTB_DEN_R |=MOTOR_FORWARD|MOTOR_REVERSE ;
//
//       GPIO_PORTF_DIR_R |= LED_MASK;
//       GPIO_PORTF_DR2R_R |= LED_MASK;
//       GPIO_PORTF_DEN_R |= LED_MASK;
//    GPIO_PORTF_DATA_R |= LED_MASK;
//    GPIO_PORTB_DATA_R |= MOTOR_FORWARD;
//    waitMicrosecond(50000000);
             PWM0_0_CMPA_R=80;
            waitMicrosecond(5000000);


//    while (1)
//        {
////        putsUart0("Hello");
//            // Get the string from the user
//            getsUart0(&data);
//            // Echo back to the user of the TTY interface for testing
//                   putsUart0(data.buffer);
//
//                   // Parse fields
//                   parseFields(&data);
//
//
//                   // Echo back the parsed field information (type and fields)
//
//                   putcUart0('\n');
//                   putcUart0('\r');
//        }
//	return 0;
}
