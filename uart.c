#include "uart.h"


void UARTInit(void){

    // Bit 2 is receiving
    P3->SEL0 |= BIT2 | BIT3; // set pins to UART mode (secondary function)

    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Reset eUSCI
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Keep eUSCI in reset
    EUSCI_A_CTLW0_SSEL__SMCLK; // Use SMCLK as the eUSCI clock source
    EUSCI_A2->BRW = 19; // 3000000/16/9600
    EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
}

void sendString(char *message) {
    while (*message) {
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)); // Wait until the TX buffer is ready
        EUSCI_A0->TXBUF = *message; // Send the character
        message++; // Move to the next character in the string
    }
}


