# include "buzzer.h"

void buzzerInit(void){
    P2->DIR |= BIT7; // P2.7 set TA0.1
    P2->SEL0 |= BIT7;
    P2->SEL1 &= ~(BIT7);
    //TIMER_A0->CCR[0] = period - 1; // PWM Period
    TIMER_A1->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set
    // CCR1 PWM duty cycle
    TIMER_A1->CCR[0] = 0; // initialize period to 0
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK
    TIMER_A_CTL_MC__UP | // Up mode
    TIMER_A_CTL_CLR; // Clear TAR
     // Enter LPM0
}

void playNote(int frequency){

    float periodS = 1.0/frequency;
    float periodT = periodS*3000000.0;
    int period = periodT;

    TIMER_A1->CCR[0] = period - 1;
    TIMER_A1->CCR[1] = period/2;
    //SysTick_Wait100ms();
}

void mute(void){
    TIMER_A1->CCR[1] = 0;
}
