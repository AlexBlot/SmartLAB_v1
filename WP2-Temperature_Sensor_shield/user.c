/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void InitApp(void)
{
    //disable analog functions
    ANSELA = 0x00; // Turn off analog inputs port A
    ANSELB = 0x00; // Turn off analog inputs port B
    ANSELC = 0x00; // Turn off analog inputs port C

    TRISCbits.TRISC0 = 0;               // ICSPDAT
    TRISCbits.TRISC1 = 0;               // ICSPCLK
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;               // green led output
    TRISCbits.TRISC5 = 0;               // red led output
    TRISCbits.TRISC6 = 0;               // CSN, chip select not, port RC6, izlazni
    TRISCbits.TRISC7 = 0;     // SDO, serial data output, port RC7, izlazni
    
    TRISAbits.TRISA5 = 0;               // IRQ = 1 turn off sensors           
    TRISAbits.TRISA4 = 0;               // CE, chip enable, port RA4, izlazni


    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;               // switch A
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;               // switch B             



}

void timer_setup_and_start_IE(void)
{
    // Timer1 set-up
    
    T1CONbits.TMR1ON = 0;                     // Timer1 OFF
    T1CONbits.nT1SYNC = 1;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.T1CKPS = 0b11;                     // 11 - 8, 00 - 1  
    T1CONbits.TMR1CS = 0b11;                    // 00 - FOSC/4, 01 - FOSC, 11 - LFINTOSC
    TMR1H = 0x5F;           
    TMR1L = 0xE0; 
    TMR_CNT = 0;
    T1CONbits.TMR1ON = 1;                     // Timer1 ON
    
    PIR1bits.TMR1IF = 0;                      // Clear flag

    INTCON = 0x00;
    INTCONbits.PEIE = 1;                // Enable Perpherial Interrups
    INTCONbits.GIE = 1;                 // Enable Global Interrupt  
    PIE1bits.TMR1IE = 1;                // Enable interrupt

    
}

