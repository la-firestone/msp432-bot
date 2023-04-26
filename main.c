#include "msp.h"
#include <stdio.h>
#include "lcdLib_432.h"

void sendString(char *message) {
    while (*message) {
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)); // Wait until the TX buffer is ready
        EUSCI_A0->TXBUF = *message; // Send the character
        message++; // Move to the next character in the string
    }
}

void IR_Init(void){
    // set REF voltage to 2.5
    while (REF_A->CTL0&0x0400){};
    REF_A->CTL0 = 0x0039;
    while (REF_A->CTL0&0x1000){};

    // set ADC to single conversion, divide by 1, 14-bit res
    // no interrupts, memory allocated to MEM0
    // 32 clock cycles for SHM
    ADC14->CTL0 &= ~0x00000002;
    while(ADC14->CTL0&0x00010000){};
    ADC14->CTL0 = 0x04203310;
    ADC14->CTL1 = 0x0000F030; // TODO verify this
    ADC14->MCTL[0] = 0x00000185; // port 5.0
    ADC14->MCTL[1] = 0x00000184; // port 5.1
    ADC14->MCTL[2] = 0x00000183; // port 5.2
    ADC14->MCTL[3] = 0x00000181; // port 5.4
    ADC14->IER1 = ADC14->IER0 = 0;
    P5->SEL0 |= 0x17; //p5.0, 5.1, 5.2, 5.4 // TODO: change 5.4
    P5->SEL1 |= 0x17; //p5.0, 5.1, 5.2, 5.4
    ADC14->CTL0 |= 0x00000002;

}


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

//    // initialize UART for Putty
//    P1->SEL0 |= BIT2 | BIT3; // set pins to UART mode (secondary function)
//    P1->SEL1 &= ~(BIT2 | BIT3);
//    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Reset eUSCI
//    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Keep eUSCI in reset
//    EUSCI_A_CTLW0_SSEL__SMCLK; // Use SMCLK as the eUSCI clock source
//    EUSCI_A0->BRW = 19; // 3000000/16/9600
//    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
//    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
//    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag


    IR_Init();
    // initialize variables and constants
    float refV = 2.5;
    float precision = 16384.0;
    float res = refV/precision;
    uint16_t adcResult;
    float volts;
    char uartStr[50];
    float resultsIR[4];

    lcdInit();
    lcdClear();
    int memIndex;

    while (1) {

        for (memIndex=0; memIndex<4; memIndex++){
            while (ADC14->CTL0&0x00010000){}; // wait for ADC
            ADC14->CTL0 |= 0x00000001;        // start a conversion
            while((ADC14->IFGR0&0x00000017)==0){};  // wait until buffer is ready
            adcResult = ADC14->MEM[memIndex];        // return the converted value
            volts = res*adcResult;
            resultsIR[memIndex]=volts;
            _delay_cycles(10000);

        }

        int i;
        printf("\nResults: ");
        for(i=0; i<4; i++) {
            printf("%.2f ", resultsIR[i]);
        }

        _delay_cycles(1000000);
    }

//        // Convert volt float value to a string and display to LCD
//        sprintf(uartStr, "%.2f ", volts, "\n");
//        lcdClear();
//        lcdSetText("Volts",0,0);
//        lcdSetText(uartStr,0,1);
//
//        sendString(uartStr);
//
//        // _delay_cycles(1000000);delay for screen


}
