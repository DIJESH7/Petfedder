#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "uart_input.h"
#include "gpio.h"

#define LED (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 2*4)))

#define SPEAKER (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4)))

#define LED_MASK 4  //PF2 M1PWM6

#define MOTOR_FORWARD 64 //PB6 M0PWM0

#define MOTOR_REVERSE  128 //PB7 M0PWM1
#define WATER_MOTOR 16  //PB4 M0PWM2

#define WATER_REVERSE 32 //PB5 M0PWM3
#define SPEAKER_MASK 8
#define TRIG_MASK 128 //A7 is trig output

#define ECHO_MASK 1 //B0 is echo input
int timer = 0;
int count = 0;

#define MOTOR PORTE,3
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
      SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN
              | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
      // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
      // Note UART on port A must use APB
      SYSCTL_GPIOHBCTL_R = 0;
      SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R1
              | SYSCTL_RCGCGPIO_R5;
      SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1 | SYSCTL_RCGCTIMER_R2;
      GPIO_PORTA_DIR_R |= SPEAKER_MASK;
      GPIO_PORTA_DEN_R |= SPEAKER_MASK;

      ;

      // Configure pin A7:
      GPIO_PORTA_DIR_R |= TRIG_MASK;
      GPIO_PORTA_DEN_R |= TRIG_MASK;

      //Pin B0:
      GPIO_PORTB_DIR_R &= ~ECHO_MASK;
      GPIO_PORTB_DEN_R |= ECHO_MASK;

      //Configure the Interrupts:
      GPIO_PORTB_IS_R &= ~ECHO_MASK;              // Make interrupt edge sensitive
      GPIO_PORTB_IBE_R |= ECHO_MASK;                 // Interrupt on one edge

      GPIO_PORTB_ICR_R |= ECHO_MASK;                  // Clear Interrupt
      NVIC_EN0_R |= 1 << (INT_GPIOB - 16);         // turn-on interrupt 17 (GPIOB)
      GPIO_PORTB_IM_R |= ECHO_MASK;                   //Turn on interrupt
}
//void initPWMwater()
//{
//    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1 | SYSCTL_RCGCPWM_R0;
//    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5 | SYSCTL_RCGCGPIO_R1;
//
//    _delay_cycles(3);
//    GPIO_PORTB_DIR_R |= WATER_MOTOR | WATER_REVERSE;
//    GPIO_PORTB_DR2R_R |= WATER_MOTOR | WATER_REVERSE;
//    GPIO_PORTB_DEN_R |= WATER_MOTOR | WATER_REVERSE;
//    GPIO_PORTB_AFSEL_R |= WATER_MOTOR | WATER_REVERSE;
//    GPIO_PORTB_PCTL_R &= (GPIO_PCTL_PB4_M | GPIO_PCTL_PB5_M);
//    GPIO_PORTB_PCTL_R |= (GPIO_PCTL_PB4_M0PWM2 | GPIO_PCTL_PB5_M0PWM3);
//    SYSCTL_SRPWM_R = (SYSCTL_SRPWM_R1 | SYSCTL_SRPWM_R0);
//    SYSCTL_SRPWM_R = 0;
//
//    PWM0_0_CTL_R = 0;
//    PWM0_1_GENA_R |= PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE;
//    PWM0_1_GENB_R |= PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTLOAD_ONE;
//    PWM0_1_LOAD_R = 1048;
//
//       PWM0_INVERT_R |= (PWM_INVERT_PWM2INV | PWM_INVERT_PWM3INV);
//       PWM0_1_CMPA_R = 0;
//           PWM0_1_CMPB_R = 0;
//           PWM0_1_CTL_R = PWM_0_CTL_ENABLE;
//           PWM0_ENABLE_R = PWM_ENABLE_PWM2EN | PWM_ENABLE_PWM3EN;
//}

void initPWM()
{
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1 | SYSCTL_RCGCPWM_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5 | SYSCTL_RCGCGPIO_R1;

    _delay_cycles(3);
//    GPIO_PORTB_DIR_R |= MOTOR_FORWARD;
//           GPIO_PORTB_DR2R_R |= MOTOR_FORWARD;
//           GPIO_PORTB_DEN_R |=MOTOR_FORWARD;
//           GPIO_PORTB_AFSEL_R |= MOTOR_FORWARD;
//           GPIO_PORTB_PCTL_R &= GPIO_PCTL_PB6_M;
//           GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB6_M0PWM0;

    GPIO_PORTB_DIR_R |= MOTOR_FORWARD | MOTOR_REVERSE;
    GPIO_PORTB_DR2R_R |= MOTOR_FORWARD | MOTOR_REVERSE;
    GPIO_PORTB_DEN_R |= MOTOR_FORWARD | MOTOR_REVERSE;
    GPIO_PORTB_AFSEL_R |= MOTOR_FORWARD | MOTOR_REVERSE;
    GPIO_PORTB_PCTL_R &= (GPIO_PCTL_PB6_M | GPIO_PCTL_PB7_M);
    GPIO_PORTB_PCTL_R |= (GPIO_PCTL_PB6_M0PWM0 | GPIO_PCTL_PB7_M0PWM1);

    GPIO_PORTF_DIR_R |= LED_MASK;
    GPIO_PORTF_DR2R_R |= LED_MASK;
    GPIO_PORTF_DEN_R |= LED_MASK;
    GPIO_PORTF_AFSEL_R |= LED_MASK;
    GPIO_PORTF_PCTL_R &= GPIO_PCTL_PF2_M;
    GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF2_M1PWM6;

    SYSCTL_SRPWM_R = (SYSCTL_SRPWM_R1 | SYSCTL_SRPWM_R0);
    SYSCTL_SRPWM_R = 0;

    PWM0_0_CTL_R = 0;
    PWM0_0_GENA_R |= PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE;
    PWM0_0_GENB_R |= PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTLOAD_ONE;
    PWM1_3_CTL_R = 0;
    PWM1_3_GENA_R |= PWM_1_GENA_ACTCMPAD_ZERO | PWM_1_GENA_ACTLOAD_ONE;

    PWM1_3_LOAD_R = 1000;
    PWM1_INVERT_R = PWM_INVERT_PWM6INV;
    PWM0_0_LOAD_R = 1000;

    PWM0_INVERT_R |= (PWM_INVERT_PWM0INV | PWM_INVERT_PWM1INV);
    //PWM0_INVERT_R|=PWM_INVERT_PWM0INV;

    PWM1_3_CMPA_R = 0;
    PWM0_0_CMPA_R = 0;
    PWM0_0_CMPB_R = 0;
    PWM0_0_CTL_R = PWM_0_CTL_ENABLE;
    PWM0_1_CTL_R = PWM_0_CTL_ENABLE;

    PWM1_3_CTL_R = PWM_1_CTL_ENABLE;
    PWM1_ENABLE_R = PWM_ENABLE_PWM6EN;

    PWM0_ENABLE_R = PWM_ENABLE_PWM0EN | PWM_ENABLE_PWM1EN;
    //PWM0_ENABLE_R=PWM_ENABLE_PWM0EN;

}

void EchoISR()
{
    GPIO_PORTB_ICR_R |= ECHO_MASK; //Clear the interrupt
    putcUart0(timer + 48);
    putsUart0("\r\n");
    if (timer == 0)
    {
        //Confingure Timer:
        TIMER1_CTL_R &= ~TIMER_CTL_TAEN;
        TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;
        TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
        TIMER1_TAILR_R = 200;   // Interrupt every 5 us
        TIMER1_IMR_R = TIMER_IMR_TATOIM;
        NVIC_EN0_R |= 1 << (INT_TIMER1A - 16);
        TIMER1_CTL_R |= TIMER_CTL_TAEN;
        count = 0;
    }

    else
    {
        //Turn off timer here:
        TIMER1_IMR_R = ~TIMER_IMR_TATOIM;
        TIMER1_CTL_R &= ~TIMER_CTL_TAEN;
        if (count == 0)
            putsUart0("0ms \r\n");

        else
        {
            int distance = 343 * count * 5 / 20000;       //distance in cm
            char outstr[4] = 0;
            outstr[3] = '\0';
            int i = 0;
            for (i = 0; i < 3; i++)
            {
                outstr[2 - i] = (distance % 10) + 48;
                distance = distance / 10;
            }
            putsUart0(outstr);
            putsUart0(" cm.\r\n");
            //Error could be up to 0.03 cms.

        }
    }
    timer++;
}

void Timer1ISR()
{
    //counts the no. of 5us interrupts
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;
    count++;
}

void initMotor()
{
//    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN
//            | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
    enablePort (PORTE);

    selectPinPushPullOutput (MOTOR);
}

void motorOn()
{
    setPinValue(MOTOR, 1);
}

void motorOff()
{
    setPinValue(MOTOR, 0);
}
void timer2Isr()
{
    SPEAKER ^= 1;
    TIMER2_ICR_R = TIMER_ICR_TATOCINT;               // clear interrupt flag
}
void initTimer2()
{
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;      // turn-off timer before reconfiguring
    TIMER2_CFG_R = TIMER_CFG_32_BIT_TIMER;    // configure as 32-bit timer (A+B)
    TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // configure for periodic mode (count down)
    TIMER2_TAILR_R = 40000000; // set load value to 40e6 for 1 Hz interrupt rate
    TIMER2_IMR_R = TIMER_IMR_TATOIM;                 // turn-on interrupts
    NVIC_EN0_R |= 1 << (INT_TIMER2A - 16);     // turn-on interrupt 37 (TIMER1A)
}

void playLowContainer()
{
    TIMER2_CTL_R |= TIMER_CTL_TAEN;
    TIMER2_TAILR_R = 19111.3235;
    waitMicrosecond(2000000);
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;

    TIMER2_CTL_R |= TIMER_CTL_TAEN;
    TIMER2_TAILR_R = 102040.816;
    waitMicrosecond(2000000);
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;
}

int main(void)
{
    USER_DATA data;
    initHw();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initTimer2();
    initMotor();
    initPWM();
    putsUart0("gi");

    // initPWMwater();
//
//    PWM0_0_CMPA_R = 999;
//    waitMicrosecond(5000000);
//    PWM0_0_CMPA_R = 0;
//    waitMicrosecond(5000000);
//    PWM0_0_CMPB_R = 999;
//    waitMicrosecond(5000000);
//    PWM0_0_CMPB_R = 0;
//    waitMicrosecond(5000000);
//    PWM0_0_CMPA_R = 700;
//    waitMicrosecond(5000000);
//    PWM0_0_CMPA_R = 0;
//    waitMicrosecond(5000000);
//    PWM0_0_CMPB_R = 900;
//    waitMicrosecond(5000000);
//        PWM0_1_CMPA_R = 900;
//        waitMicrosecond(500000);
//        PWM0_1_CMPA_R =10;

    while (true)
    {
        timer = 0;
        GPIO_PORTA_DATA_R &= ~TRIG_MASK;       //Turn off at first
        waitMicrosecond(5);
        GPIO_PORTA_DATA_R |= TRIG_MASK; //Turn on for 10us
        waitMicrosecond(10);
        GPIO_PORTA_DATA_R &= ~TRIG_MASK; //Turn off
        waitMicrosecond(500000);

        if (kbhitUart0())
        {

        }
//        playLowContainer();
//        motorOn();
    }
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
//  return 0;
}
