#include "usart_h.h"
/*
 * usart.c
 *
 *  Created on: July 1, 2019
 *      Author: i_lov
 */

/* USART0_SendStr(uint8_t *pstr)
 * send one string start from pointer pstr
 *
 * */
void USART0_SendStr(uint8_t *pstr)
{
    /* Disable USART RX before Transfer */
    UART0RX_Disable;
    while(*pstr != '\0')
    {
        /* check if TX0 buffer busy */
        while (!(IFG2&UCA0TXIFG));
        /* send one char */
        UCA0TXBUF = *pstr++;
    }
    while(!(IFG2&UCA0TXIFG));
    UCA0TXBUF = '\n';
    /* Enable USART RX after Transfer */
    UART0RX_Enable;
}
void USART1_SendStr(uint8_t *pstr)
{
    /* Disable USART RX before Transfer */
    UART1RX_Disable;
    while(*pstr != '\0')
    {
        /* check if TX1 buffer busy */
        while (!(UC1IFG&UCA1TXIFG));
        /* send one char */
        UCA1TXBUF = *pstr++;
    }
    while(!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = '\n';
    /* Enable USART RX after Transfer */
    UART1RX_Enable;
}
void SendUCA0Data(uint8_t data)
{
    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = data;
}
void SendUCA1Data(uint8_t data)
{
    while (!(UC1IFG&UCA1TXIFG));                // USCI_A0 TX buffer ready?
    UCA1TXBUF = data;
}
void initUART()
{
#if UART_MODE == SMCLK_115200
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 138;                            // 16MHz 115200
    UCA0BR1 = 0;                              // 16MHz 115200
    UCA0MCTL = UCBRS_7;                       // Modulation UCBRSx = 7
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

// uart1
    UCA1CTL1 |= UCSSEL_2;                     // SMCLK
    UCA1BR0 = 138;                            // 16MHz 115200
    UCA1BR1 = 0;                              // 16MHz 115200
    UCA1MCTL = UCBRS_7;                       // Modulation UCBRSx = 7
    UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UC1IFG &= ~(UCA1RXIFG);
    UC1IE |= UCA1RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == SMCLK_19200
    /* USART0 */
    /* CLK source selection */
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    /* with 16000000/19200 = 833.333 = 0x0341 + 0.33 */
#ifdef SMCLK_16MHz_BR19200
    UCA0BR0 = 0x41;                            // 16MHz 19200
    UCA0BR1 = 0x03;                              // 16MHz 19200
    /* 0.33*8 = 2.4 = 0d2 */
    UCA0MCTL = UCBRS_2;                       // Modulation UCBRSx = 2
#endif
#ifdef SMCLK_8MHz_BR19200
    /* with 8000000/19200 = 416.667 = 0x01A0 + 0.667 */
    UCA0BR0 = 0xA0;                            // 8 MHz 19200
    UCA0BR1 = 0x01;                              // 8 MHz 19200
    /* 0.667*8 = 5.336 = 0x05 */
    UCA0MCTL = UCBRS_6;                       // UCBRS_2 Modulation UCBRSx = 2
#endif
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    /* USART1 */
    /* CLK source selection */
    UCA1CTL1 |= UCSSEL_2;                     // SMCLK
#ifdef SMCLK_16MHz_BR19200
    /* with 16000000/19200 = 833.333 = 0x0341 + 0.33 */
    UCA1BR0 = 0x41;                            // 16MHz 19200
    UCA1BR1 = 0x03;                              // 16MHz 19200
    /* 0.33*8 = 2.4 = 0d2 */
    UCA1MCTL = UCBRS_2;                       // Modulation UCBRSx = 2
#endif
#ifdef SMCLK_8MHz_BR19200
    /* with 8000000/19200 = 416.667 = 0x01A0 + 0.667 */
    UCA1BR0 = 0xA0;                            // 8 MHz 19200
    UCA1BR1 = 0x01;                              // 8 MHz 19200
    /* 0.667*8 = 5.336 = 0x05 */
    UCA1MCTL = UCBRS_6;                       // UCBRS_2 Modulation UCBRSx = 2
#endif
    UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UC1IFG &= ~(UCA1RXIFG);
    UC1IE |= UCA1RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == SMCLK_9600
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 16MHz 9600
    UCA0BR1 = 0;                              // 16MHz 9600
    UCA0MCTL = UCBRS_0 + UCOS16 + UCBRF_3;    // Modulation UCBRSx = 0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == ACLK_9600
    UCA0CTL1 |= UCSSEL_1;                     // ACLK
    UCA0BR0 = 3;                              // 32768Hz 9600
    UCA0BR1 = 0;                              // 32768Hz 9600
    UCA0MCTL = UCBRS_3;                       // Modulation UCBRSx = 3
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#else
    #error "Select UART Baud Rate Right! "
#endif
}



