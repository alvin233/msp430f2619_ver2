#ifndef _AD9833_H_
#define _AD9833_H_

#include <msp430.h>
#include <stdint.h>

/* Definition of AD9833 */
/* Pin definition */
#define SCK_Pin BIT2
#define DAT_Pin BIT3
#define FSYNC_Pin BIT4
/* wave shape selection */
#define TRI_WAVE 0
#define SQU_WAVE 1
#define SIN_WAVE 2
/* pins output definition */
/* CLK pin output definition */
#define SCK_1 {P2OUT |= SCK_Pin;} /* CLK bit set 1 */
#define SCK_0 {P2OUT &= ~SCK_Pin;} /* CLK bit reset 0 */
/* DATA pin output definition */
#define DAT_1 {P2OUT |= DAT_Pin;} /* DATA bit set 1 */
#define DAT_0 {P2OUT &= ~DAT_Pin;} /* DATA bit reset 0 */
/* FSYNC pin output definition */
#define FSYNC_1 {P2OUT |= FSYNC_Pin;} /* FSYNC bit set 1 */
#define FSYNC_0 {P2OUT &= ~FSYNC_Pin;} /* FSYNC bit reset 0 */
/* function definition */
void delay_us(uint8_t x);
void intiAD9833(uint32_t Freq_kHz, uint32_t Freq_SFR, uint8_t WaveMode, uint16_t  Phase);
#endif
