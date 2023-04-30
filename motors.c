#include "motors.h"


int max_ticks = 10000;

void setupGPIO()
{
    P2->SEL0 |= (ENA_PIN | ENB_PIN);
    P2->SEL1 &= ~(ENA_PIN | ENB_PIN);
    P2->DIR |= (ENA_PIN | ENB_PIN); //P2.4 and P2.5

    P3->DIR |= (IN1_PIN | IN2_PIN | IN3_PIN | IN4_PIN);
    P3->OUT &= ~(IN1_PIN | IN2_PIN | IN3_PIN | IN4_PIN);
}


void setupPWM()
{
    // Set period
    TIMER_A0->CCR[0] = max_ticks-1;

    // Configure CCR1 and CCR2 for ENA and ENB
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[1] = 0;
    TIMER_A0->CCR[2] = 0;
    // Configure Timer A1
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
    TIMER_A_CTL_MC__UP | // Up mode
    TIMER_A_CTL_CLR; // Clear TAR

}

void setMotorSpeed(int motor, int speed)
{
    // Clamp speed to 0-100 range
    if (speed < 0) speed = 0;
    if (speed > max_ticks) speed = max_ticks;

    // Set duty cycle
    if (motor == 1)
    {
        TIMER_A0->CCR[1] = speed;
    }
    else if (motor == 2)
    {
        TIMER_A0->CCR[2] = speed;
    }
}


void setMotorDirection(int motor, int direction)
{
    if (motor == 1)
    {
        if (direction == 0) // Reverse
        {
            P3->OUT |= IN1_PIN;
            P3->OUT &= ~IN2_PIN;
        }
        else // Forward
        {
            P3->OUT &= ~IN1_PIN;
            P3->OUT |= IN2_PIN;
        }
    }
    else if (motor == 2)
    {
        if (direction == 0) // Reverse
        {
            P3->OUT |= IN3_PIN;
            P3->OUT &= ~IN4_PIN;
        }
        else // Forward
        {
            P3->OUT &= ~IN3_PIN;
            P3->OUT |= IN4_PIN;
        }
    }
}

void motorsOff(void){

    setMotorSpeed(1,0);
    setMotorSpeed(2,0);

}
