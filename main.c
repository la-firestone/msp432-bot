
#include <msp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcdLib_432.h"
#include "motors.h"
#include "buzzer.h"
#define Num_of_Results 8
#define THRESH 2000
volatile uint16_t rightSensor;
volatile uint16_t midSensor;
volatile uint16_t leftSensor;


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

    // ######################################

    P2->DIR |= 0x07;  // P2.0-_2.1 set as outputs 00000111 (pins 2.0, 2.1, 2.2, use | to set)
    P2->OUT &= ~0x07; // &= 11111000, turns off all the LEDS at init

    SysTick_Init();
    P2->OUT |= BIT1;
    delayms(1000);
    P2->OUT &= ~BIT1;


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
            lcdClear();
            lcdSetText("turn left",0,0);
            turnLeft();
            __delay_cycles(1000000);
            stop();
            __delay_cycles(1000000);
         }


         else if (leftSensor < THRESH){
             lcdClear();
             lcdSetText("turn right",0,0);
             turnRight();
             __delay_cycles(1000000);
             stop();
             __delay_cycles(1000000);
         }

         else if (midSensor < THRESH){
             lcdClear();
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
