/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{
     OSCCONbits.IRCF = 0b1110;   // Internal RC Oscillator Frequency Select - HFINTOSC (8mHz)
        /*IRCF<3:0>: Internal Oscillator Frequency Select bits
        1111 = 16 MHz or 48 MHz HF (see Section 5.2.2.1 ?HFINTOSC?)
        1110 = 8 MHz or 24 MHz HF (3x PLL) or 32 MHz HF (4x PLL) (see Section 5.2.2.1 ?HFINTOSC?)
        1101 = 4 MHz
        1100 = 2 MHz
        1011 = 1 MHz
        1010 = 500 kHz(1)
        1001 = 250 kHz(1)
        1000 = 125 kHz(1)
        0111 = 500 kHz (default upon Reset)
        0110 = 250 kHz
        0101 = 125 kHz
        0100 = 62.5 kHz
        001x = 31.25 kHz(1)
        000x = 31 kHz LF*/
     OSCCONbits.SCS  = 0b00;     // System Clock Select
        /*SCS<1:0>: System Clock Select bits
        1x = Internal oscillator block
        01 = Secondary oscillator
        00 = Clock determined by FOSC<2:0> in Configuration Words.*/
      OSCCONbits.SPLLMULT  = 0b1; 
        /*SPLLMULT: Software PLL Multiplier Select bit
        1 = 3x PLL is enabled
        0 = 4x PLL is enabled*/
      OSCCONbits.SPLLEN  = 0b0; 
      /*SPLLEN: Software PLL Enable bit
        If PLLEN in Configuration Words = 1:
        SPLLEN bit is ignored. PLL is always enabled (subject to oscillator requirements)
        If PLLEN in Configuration Words = 0:
        1 = PLL is enabled
        0 = PLL is disabled*/
}
