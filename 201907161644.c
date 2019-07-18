
#include <msp430.h>
#include <stdint.h>
#include "AD9833_H.h"
#include "usart_h.h"
//******************************************************************************
// Device Initialization *******************************************************
/**********************************************************************************/
// #define DCO
//******************************************************************************
/* PWM output port selection */
/* by selection TA, output port is P1.2 and P1.3 */
#define TA
/* by-selection TB, output port is P4.1 and P4.2 */
//#define TB
/*******************************/
/* deletel a  hello */
void initClockTo16MHz()
{
#ifdef DCO
    if (CALBC1_16MHZ==0xFF)                  // If calibration constant erased
    {
        while(1);                            // do not load, trap CPU!!
    }
    DCOCTL = 0;                              // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                  // Set DCO
    DCOCTL = CALDCO_16MHZ;
#else
    if (CALBC1_16MHZ==0xFF)                  // If calibration constant erased
    {
        while(1);                            // do not load, trap CPU!!
    }
    DCOCTL = 0;                              // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                  // Set DCO
    DCOCTL = CALDCO_16MHZ;
    /*  */
    BCSCTL2 |= SELM_0; /* MCLK Source Select 0: DCOCLK */
    BCSCTL2 |= DIVM_0; /* MCLK Divider 0: /1 */
    /* select XT2, which is 8 MHz */
    BCSCTL1 &= ~XT2OFF; /* enable XT2 clk */
    BCSCTL3 |= XT2S_2; /* Mode 2 for XT2 : 2 - 16 MHz */
    /* select XT2 as SMCLK source*/
    BCSCTL2 |= SELS; /* SMCLK Source Select 0:DCOCLK / 1:XT2CLK/LFXTCLK */
    BCSCTL2 |= DIVS_0; /* SMCLK Divider 0: /1 */
#endif
}

void initGPIO()
{
    /* LED GPIO */
    P4DIR = BIT0;
    /* UART0 GPIO */
    /* P3.5 = RXD, P3.4=TXD */
    P3SEL |= BIT4 + BIT5;
    /* UART1 GPIO */
    /* P3.6 = RXD, P3.7=TXD */
    P3SEL |= BIT6 + BIT7;

    /* PWM GPIO */
#ifdef TA
    /* P1.2 and P1.3 */
    P1DIR = BIT2 + BIT3;
    P1SEL = BIT2 + BIT3; /* SET P1.2 and P1.3 as PWM OUTPUT function */
#else
    /* SET P4.1 and P4.2 as PWM OUTPUT function */
    P4DIR |= BIT1 + BIT2;
    P4SEL |= BIT1 + BIT2;
#endif
    /* SET P1.4 P1.5 as UART CONTROL BITs */
    /* UART LOGIC CONTROL GPIO */
    P1DIR |= BIT4 + BIT5;
    /* init AD9833 GPIO */
    P2DIR |= BIT2 + BIT3 + BIT4;
    /* init Current overflow protection */

#if 1
    /* SET P2.1 as current overflow protection pin*/
    /* inner pull down */
    P2REN |= BIT1;
    P2OUT &= ~BIT1;
    /* enable P1 bit5 interrupt, rising edge trigger */
    P2IES &= ~BIT1;
    /* clear P1 bit5 IF flag */
    P2IFG &= ~BIT1;
    /* enable P1 bit5 IF flag */
    P2IE |= BIT1;
#else
    /* inner pull up */
    P2REN |= BIT1;
    P2OUT |= BIT1;
    /* enable P1 bit5 interrupt, falling edge trigger */
    P2IES |= BIT1;
    /* clear P1 bit5 IF flag */
    P2IFG &= ~BIT1;
    /* enable P1 bit5 IF flag */
    P2IE |= BIT1;
#endif
}
//******************************************************************************
// initPWM ************************************************************************
// init the PWM output wave *
// the PWM work as up/down mode, in which the counter of the TACCCR if half of the usual *
// use SMCLK as CLK source *
//******************************************************************************
void initPWM()
{
    static const unsigned char Val = 0;  /* serve as dead time */
    static const unsigned int period = 400; /* PWM period period = CLK_freq / (2*output_freq) on account of MC_3 mode */
#ifdef TA
    TACCR0 = period - 1; /* PWM period */
    TACCTL1 |= OUTMOD_6; /* output mode 6- PWM toggle/set */
    TACCR1 = 200 - Val; /* SET pwm duty cycle */
    TACCTL2 |= OUTMOD_2;  /* PWM output mode: 2 - PWM toggle/reset */
    TACCR2 = 200 + Val; /* SET pwm duty cycle */
    TACTL |= TASSEL_2 + MC_1; /* SMCLK as CLK,  */
#endif
#ifdef TB
    TBCCR0 = period - 1;
    TBCCTL1 |= OUTMOD_6;
    TBCCR1 = 200 - Val;
    TBCCTL2 |= OUTMOD_2;
    TBCCR2 = 200 + Val;
    TBCTL |= TBSSEL_2 + MC_1;
#endif
#if defined(TA) && defined(TB)
#error You should only choose TA or TB!
#endif
}
void DisablePWM(void)
{
#ifdef TA
    /* PWM GPIO */
    P1DIR |= BIT2 + BIT3;
    P1SEL &= ~BIT2 + ~BIT3; /* RESET P1.2 and P1.3 as PWM OUTPUT function */
    P1OUT &= ~BIT2 + ~BIT3; /* output low to deinit PWM */
#endif
}
void EnablePWM(void)
{
#ifdef TA
    /* PWM GPIO */
    P1SEL |= BIT2 + BIT3; /* SET P1.2 and P1.3 as PWM OUTPUT function */
#endif
}
/**/

//******************************************************************************
// Main ************************************************************************
// Enters LPM0 if SMCLK is used and waits for UART interrupts. If ACLK is used *
// then the device will enter LPM3 mode instead. The UART RX interrupt handles *
// the received character and echoes it.                                       *
//******************************************************************************
uint8_t rx0_val[255] = {0}; // USART0 RX buffer
uint8_t rx1_val[255] = {0}; // USART1 RX buffer

static uint8_t UC0RX_pos = 0; // USART0 RX string ptr position
static uint8_t UC1RX_pos = 0; // USART1 RX string ptr position

void main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    initClockTo16MHz();
    initGPIO();
    initUART();
    initPWM();
    delay_us(255);
    delay_us(255);
    delay_us(255);
    intiAD9833(1400,0,SIN_WAVE,0 );   //1.4MHz = 1400 kHz,  ÆµÂÊ¼Ä´æÆ÷0£¬ÕýÏÒ²¨Êä³ö ,³õÏàÎ»0 */

    __bis_SR_register(LPM0_bits + GIE);       // Since SMCLK is source, enter LPM0, interrupts enabled

}
//******************************************************************************
// UART RX Interrupt ***********************************************************
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (IFG2 & UCA0RXIFG)
    {
        uint8_t rx_val;
        rx_val = UCA0RXBUF; //Must read UCxxRXBUF to clear the flag
        if(rx_val != 0x0A)// end of the str is 0x0d follow with 0x0a
        {
            rx0_val[UC0RX_pos++] = rx_val; // receive RX str
        }else
        {
            rx0_val[UC0RX_pos++] = '\0';// end of RX str
        //SendUCA1Data(rx0_val);
#if 1
         USART1_SendStr(rx0_val);
#else
         USART0_SendStr(rx0_val);
#endif
        UC0RX_pos = 0;
        }
    }
}
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCI1RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB1RX_VECTOR))) USCI1RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (UC1IE & UCA1RXIFG)
    {
        uint8_t rx_val;
        rx_val = UCA1RXBUF; //Must read UCxxRXBUF to clear the flag
        if(rx_val != 0x0A)
        {
            rx1_val[UC1RX_pos++] = rx_val; // receive RX str
        }else
        {
            rx1_val[UC1RX_pos++] = '\0';// end of RX str
#if 0
            USART1_SendStr(rx1_val);
#else
            USART0_SendStr(rx1_val);
#endif
            UC1RX_pos = 0; // clear RX str ptr
        }
    }
}
/* happen when output current overflow */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void P2BIT1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB1RX_VECTOR))) USCI1RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    static int count;
    P4OUT ^= BIT0;
    /* clear P2 bit1 IF */
    if(!(count%2))
    {
        DisablePWM();
    }
    else
    {
        EnablePWM();
    }
    P2IFG &= ~BIT1;
    count++;
}

