/*
 * usart_h.h
 *
 *  Created on: Jul 1, 2019
 *      Author: i_lov
 */

#ifndef APP_USART_H_H_
#define APP_USART_H_H_

#include <msp430.h>
#include <stdint.h>

/* you have to select only one of the following choice */
#define SMCLK_8MHz_BR19200
//#define SMCLK_16MHz_BR19200
/*****************************************************************/
//******************************************************************************
// UART TX logic control Initialization *********************************************************
//******************************************************************************
//******************************************************************************
//   The UART can operate using ACLK at 9600, SMCLK at 115200 or SMCLK at 19200 or SMCLK at 9600.
//   To configure the UART mode, change the following line:
//
//      #define UART_MODE       SMCLK_115200
//      to any of:
//      #define UART_MODE       SMCLK_115200
//      #define UART_MODE       SMCLK_19200
//      #define UART_MODE       SMCLK_9600
//      #define UART_MODE       ACLK_9600
//
//   UART RX ISR is used to handle communication.
//   ACLK = 32.768kHz, MCLK = SMCLK = DCO 16MHz.
//******************************************************************************
#define UART0RX_Enable {P1OUT |= BIT4;}
#define UART0RX_Disable {P1OUT &= ~BIT4;}
#define UART1RX_Enable {P1OUT |= BIT5;}
#define UART1RX_Disable {P1OUT &= ~BIT5;}

/* UART CLK source selection */
#define SMCLK_115200     0
#define SMCLK_9600      1
#define ACLK_9600       2
#define SMCLK_19200    3

#define UART_MODE       SMCLK_19200

void USART0_SendStr(uint8_t *pstr);
void USART1_SendStr(uint8_t *pstr);
void SendUCA0Data(uint8_t data);
void SendUCA1Data(uint8_t data);
void initUART();

#endif /* APP_USART_H_H_ */
