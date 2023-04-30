// MSP432P401 Demo - ADC14, Repeated Sequence of Conversions
//
// Description: This example shows how to perform a repeated sequence of
// conversions using "repeat sequence-of-channels" mode. AVcc is used for the
// reference and repeated sequence of conversions is performed on Channels A0,
// A1, A2, and A3. Each conversion result is stored in ADC14->MEM[0], ADC14->MEM[1],
// ADC14->MEM[2], and ADC14->MEM[3] respectively. After each sequence, the 4 conversion
// results are moved to A0results[], A1results[], A2results[], and A3results[].
// Test by applying voltages to channels A0 - A3. Open a watch window in
// debugger and view the results. Set Breakpoint1 in the index increment line
// to see the array values change sequentially and Breakpoint2 to see the entire
// array of conversion results in A0results[], A1results[], A2results[], and
// A3results[]for the specified Num_of_Results.
//
// Note that a sequence has no restrictions on which channels are converted.
// For example, a valid sequence could be A0, A3, A2, A4, A2, A1, A0, and A7.
// See the User's Guide for instructions on using the ADC14.
//
// MSP432P401
// -----------------
// /|\| |
// | | |
// --|RST |
// | |
// Vin0 -->|P5.5/A0 |
// Vin1 -->|P5.4/A1 |
// Vin2 -->|P5.3/A2 |
// Vin3 -->|P5.2/A3 |
// | |
//
// William Goh
// Texas Instruments Inc.
// Oct 2016 (updated) | June 2014 (created)
// Built with CCSv6.1, IAR, Keil, GCC
//******************************************************************************
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "motors.h"
#include "lcdLib_432.h"
#define Num_of_Results 8
volatile uint16_t A0results;
volatile uint16_t A1results;
volatile uint16_t A2results;
volatile uint16_t A3results;


int main(void)
    {
    WDT_A->CTL = WDT_A_CTL_PW | // Stop watchdog timer
    WDT_A_CTL_HOLD;

    // Configure GPIO
    P5->SEL1 |= BIT0 | BIT1 | BIT2 |BIT4; // Enable A/D channel A0-A3
    P5->SEL0 |= BIT0 | BIT1 | BIT2 |BIT4;

    // Enable global interrupt
    __enable_irq();
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);// Enable ADC interrupt in NVIC module

    // Turn on ADC14, extend sampling time to avoid overflow of results
    //ADC14->CTL0 = ADC14_CTL0_ON | ADC14_CTL0_MSC | ADC14_CTL0_SHT0 192 | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_3;
    ADC14->CTL0 = ADC14_CTL0_ON | ADC14_CTL0_MSC | ADC14_CTL0_SHT0_7 | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_3;


    ADC14->MCTL[0] = ADC14_MCTLN_INCH_5; // ref+=AVcc, channel = A0
    ADC14->MCTL[1] = ADC14_MCTLN_INCH_4; // ref+=AVcc, channel = A1
    ADC14->MCTL[2] = ADC14_MCTLN_INCH_3; // ref+=AVcc, channel = A2
    ADC14->MCTL[3] = ADC14_MCTLN_INCH_1 | ADC14_MCTLN_EOS;// ref+=AVcc, channel = A3, end seq.


    ADC14->IER0 = ADC14_IER0_IE3; // Enable ADC14IFG.3
    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk; // Wake up on exit from ISR

    // Ensures SLEEPONEXIT takes effect immediately
     __DSB();
     lcdInit();
     lcdClear();
     setupGPIO();
     setupPWM();

     // Control motor1
     setMotorDirection(1, 1); // Forward
     setMotorSpeed(1, 5000); // 50% duty cycle
     __delay_cycles(2000000); // 2s delay

     setMotorDirection(1, 0); // Reverse
     setMotorSpeed(1, 5000); // 50% duty cycle
     __delay_cycles(2000000); // 2s delay

     // Control motor2
     setMotorDirection(2, 1); // Forward
     setMotorSpeed(2, 7500); // 75% duty cycle
     __delay_cycles(2000000); // 2s delay

     setMotorDirection(2, 0); // Reverse
     setMotorSpeed(2, 7500); // 75% duty cycle
     __delay_cycles(2000000); // 2s delay

     motorsOff();

     while(1)
    {    // Start conversion-software trigger
         ADC14->CTL0 |= ADC14_CTL0_ENC |
                 ADC14_CTL0_SC;
         //__sleep();
         //__no_operation(); // For debugger
         __delay_cycles(100000);
         printf("ADC Values: %d : %d : %d : %d\n", A0results, A1results, A2results, A3results);

//         if (A0results < 2000){// | (A1results < 2000) | (A2results < 2000) | (A3results < 2000)) {
//            lcdSetText("detected",0,0);
//         }

//         else{
//             lcdClear();
//         }


    }
}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG3)
    {
        A0results = ADC14->MEM[0]; // Move A0 results, IFG is cleared
        A1results = ADC14->MEM[1]; // Move A1 results, IFG is cleared
        A2results = ADC14->MEM[2]; // Move A2 results, IFG is cleared
        A3results = ADC14->MEM[3]; // Move A3 results, IFG is cleared
        //__no_operation(); // Set Breakpoint1 here
    }
}
