#include "uart.h"


void UARTInit(void){

    P3->SEL0 |= (BIT2 | BIT3); // Set P1.2 and P1.3 to use the UART
    P3->SEL1 &= ~(BIT2 | BIT3);
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put the UART in reset state
    EUSCI_A2->CTLW0 |= (EUSCI_A_CTLW0_MODE_0 | EUSCI_A_CTLW0_SSEL__SMCLK); // UART mode, SMCLK as source
    EUSCI_A2->BRW = 20; // Baud rate divisor for 115200 bps
    EUSCI_A2->MCTLW = (0 << EUSCI_A_MCTLW_BRF_OFS) | (8 << EUSCI_A_MCTLW_BRS_OFS) | EUSCI_A_MCTLW_OS16; // Fine-tuning for the baud rate
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Take the UART out of reset state
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear the RX interrupt flag
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE; // Enable RX interrupt
    NVIC_EnableIRQ(EUSCIA2_IRQn); // Enable the UART interrupt

}

void UARTsendString(char *str) {
    while (*str) {
       while (!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG)); // Wait until the TX buffer is ready
       EUSCI_A2->TXBUF = *str; // Send the character
       str++; // Move to the next character in the string
    }
}


