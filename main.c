
#include <msp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcdLib_432.h"
#define Num_of_Results 8
#define THRESH 2000
volatile uint16_t rightSensor;
volatile uint16_t midSensor;
volatile uint16_t leftSensor;
//volatile uint16_t backSensor;

// Pin configuration
#define ENA_PIN BIT5 //P2.5
#define ENB_PIN BIT4 //P2.4
#define IN1_PIN BIT0 //P3.0
#define IN2_PIN BIT6 //P3.6
#define IN3_PIN BIT7 //P3.7
#define IN4_PIN BIT5 //P3.5

// Function prototypes
void setupGPIO();
void setupPWM();
void setMotorSpeed(int motor, int speed);
void setMotorDirection(int motor, int direction);
void stop();
void goForward();
void turnLeft();
void turnRight();
void goReverse();

int max_ticks = 10000;
int speed = 5000;

int randBit(void){
    srand(time(NULL)); // seed the random number generator with current time
    int random_num = rand(); // generate a random integer number
    int random_bit = random_num % 2;
    return random_bit;
}

int main(void)
    {
    WDT_A->CTL = WDT_A_CTL_PW | // Stop watchdog timer
    WDT_A_CTL_HOLD;

    // Configure GPIO
    P5->SEL1 |= BIT0 | BIT1 | BIT2; // Enable A/D channel A0-A3
    P5->SEL0 |= BIT0 | BIT1 | BIT2;

    // Enable global interrupt
    //__enable_irq();
    //NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);// Enable ADC interrupt in NVIC module

    // Turn on ADC14, extend sampling time to avoid overflow of results
    //ADC14->CTL0 = ADC14_CTL0_ON | ADC14_CTL0_MSC | ADC14_CTL0_SHT0 192 | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_3;
    ADC14->CTL0 = ADC14_CTL0_ON | ADC14_CTL0_MSC | ADC14_CTL0_SHT0_7 | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_3;


    ADC14->MCTL[0] = ADC14_MCTLN_INCH_5; // ref+=AVcc, channel = A5 5.0
    ADC14->MCTL[1] = ADC14_MCTLN_INCH_4; // ref+=AVcc, channel = A4 5.1
    ADC14->MCTL[2] = ADC14_MCTLN_INCH_3 | ADC14_MCTLN_EOS; // ref+=AVcc, channel = A3 5.2
    //ADC14->MCTL[3] = ADC14_MCTLN_INCH_0| ADC14_MCTLN_EOS;// ref+=AVcc, channel = A1, end seq. 5.5


    //ADC14->IER0 = ADC14_IER0_IE3; // Enable ADC14IFG.3
    //SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk; // Wake up on exit from ISR

    // Ensures SLEEPONEXIT takes effect immediately
     //__DSB();
    lcdInit();
    lcdClear();
    setupGPIO();
    setupPWM();

    int bit;

     while(1)
    {    // Start conversion-software trigger
         //while (ADC14->CTL0&0x00010000){}; // wait for ADC
         ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
         rightSensor = ADC14->MEM[0]; // Move A0 results, IFG is cleared
         midSensor = ADC14->MEM[1]; // Move A1 results, IFG is cleared
         leftSensor = ADC14->MEM[2]; // Move A2 results, IFG is cleared
         //backSensor = ADC14->MEM[3]; // Move A3 results, IFG is cleared

         //__sleep();
         //__no_operation(); // For debugger
         //__delay_cycles(200000);
         //printf("ADC Values: %d : %d : %d\n", rightSensor, midSensor, leftSensor);

         if (rightSensor < THRESH){
            lcdSetText("turn left",0,0);
            turnLeft();
            __delay_cycles(1000000);
            stop();
            __delay_cycles(1000000);
         }


         else if (leftSensor < THRESH){
             lcdSetText("turn right",0,0);
             turnRight();
             __delay_cycles(1000000);
             stop();
             __delay_cycles(1000000);
         }

         else if (midSensor < THRESH){
             lcdSetText("backup",0,0);
             goReverse();
             __delay_cycles(2000000);
             stop();
             __delay_cycles(1000000);

             bit = randBit();
             if (bit==0){
                 turnLeft();
                 __delay_cycles(1000000);
                 stop();
                 __delay_cycles(1000000);
             }

             else{
                 turnRight();
                 __delay_cycles(1000000);
                 stop();
                 __delay_cycles(1000000);
             }
         }


         else{
            lcdClear();
            goForward();
         }


    }
}


// ADC14 interrupt service routine
//void ADC14_IRQHandler(void)
//{
//    if (ADC14->IFGR0 & ADC14_IFGR0_IFG3)
//    {
//        rightSensor = ADC14->MEM[0]; // Move A0 results, IFG is cleared
//        midSensor = ADC14->MEM[1]; // Move A1 results, IFG is cleared
//        leftSensor = ADC14->MEM[2]; // Move A2 results, IFG is cleared
//        backSensor = ADC14->MEM[3]; // Move A3 results, IFG is cleared
//        //__no_operation(); // Set Breakpoint1 here
//    }
//}


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


void setMotorSpeed(int motor, int speedParam)
{
    // Clamp speed to 0-100 range
    if (speedParam < 0) speedParam = 0;
    if (speedParam > max_ticks) speedParam = max_ticks;

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
    setMotorSpeed(1, speed);
    setMotorSpeed(2, speed);
}

void turnLeft(void){
    setMotorDirection(1,1);
    setMotorDirection(2,0);
    setMotorSpeed(1, speed);
    setMotorSpeed(2, speed);
}

void turnRight(void){
    setMotorDirection(1,0);
    setMotorDirection(2,1);
    setMotorSpeed(1, speed);
    setMotorSpeed(2, speed);
}

void goReverse(void){
    setMotorDirection(1,0);
    setMotorDirection(2,0);
    setMotorSpeed(1, speed);
    setMotorSpeed(2, speed);
}
