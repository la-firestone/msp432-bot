#include "motors.h"


//int max_ticks = 10000;
//int speed = 5000;

void SysTick_Init(void){
    SysTick->CTRL = 0;           // Disable SysTick during setup
    SysTick->LOAD = 0x00FFFFFF;  // Maximum reload value 2^24-1 ~16.77 MILLION
    SysTick->VAL = 0;            // Any write to current value clears it
    SysTick->CTRL = 0x00000005;  // Enable SysTick with core clock
}

void SysTick_Wait(uint32_t delay)
{
    SysTick->LOAD = delay-1;
    SysTick->VAL = 0;
    while ((SysTick->CTRL & 0x00010000) == 0){}
}

void delay1ms(void)
{
    SysTick_Wait(3000); // wait 10 ms
}


void delayms(int ms){
    int i;
        for (i=0;i<ms;i++)
        {
//            SysTick->LOAD = 30000-1;
//            SysTick->VAL = 0;
//            while ((SysTick->CTRL & 0x00010000) == 0){}
            delay1ms();
        }
}

void motorInit()
{
    P2->SEL0 |= (ENA_PIN | ENB_PIN);
    P2->SEL1 &= ~(ENA_PIN | ENB_PIN);
    P2->DIR |= (ENA_PIN | ENB_PIN); //P2.4 and P2.5

    P3->DIR |= (IN1_PIN | IN2_PIN | IN3_PIN | IN4_PIN);
    P3->OUT &= ~(IN1_PIN | IN2_PIN | IN3_PIN | IN4_PIN);
}


void motorPWMInit()
{
    // Set period
    TIMER_A0->CCR[0] = MAX_TICKS-1;

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

void setMotorSpeed(int motor, int speedParam)
{
    // Clamp speed to 0-100 range
    if (speedParam < 0) speedParam = 0;
    if (speedParam > MAX_TICKS) speedParam = MAX_TICKS;

    // Set duty cycle
    if (motor == 1)
    {
        TIMER_A0->CCR[1] = speedParam;
    }
    else if (motor == 2)
    {
        TIMER_A0->CCR[2] = speedParam;
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

void stop(void){
    setMotorSpeed(1, 0);
    setMotorSpeed(2, 0);
}

void goForward(void){
    setMotorDirection(1,1);
    setMotorDirection(2,1);
    setMotorSpeed(1, DRIVE_SPEED);
    setMotorSpeed(2, DRIVE_SPEED);
}

void goForwardDelay(int delay){
//    setMotorDirection(1,1);
//    setMotorDirection(2,1);
//    setMotorSpeed(1, speed);
//    setMotorSpeed(2, speed);
    goForward();
    delayms(delay);
    stop();
}

void turnLeft(void){
    setMotorDirection(1,1);
    setMotorDirection(2,0);
    setMotorSpeed(1, TURN_SPEED);
    setMotorSpeed(2, TURN_SPEED);
}

void turnLeftDelay(int delay){
//
    turnLeft();
    delayms(delay);
    stop();
}

void turnRight(void){
    setMotorDirection(1,0);
    setMotorDirection(2,1);
    setMotorSpeed(1, TURN_SPEED);
    setMotorSpeed(2, TURN_SPEED);
}

void turnRightDelay(int delay){
    turnRight();
    delayms(delay);
    stop();
}

void goReverse(void){
    setMotorDirection(1,0);
    setMotorDirection(2,0);
    setMotorSpeed(1, DRIVE_SPEED);
    setMotorSpeed(2, DRIVE_SPEED);
}

void goReverseDelay(int delay){
    goReverse();
    delayms(delay);
    stop();
}

