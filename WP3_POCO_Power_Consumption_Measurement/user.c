/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <pic16lf1459.h>        /* For true/false definition */

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
    TRISCbits.TRISC2 = UNUSED_TRIS;
    TRISCbits.TRISC3= 0;     // RC3 AN7 input
    TRISCbits.TRISC4 = 0;               // green led output
    TRISCbits.TRISC5 = 0;               // red led output
    TRISCbits.TRISC6 = 0;               // CSN, chip select not, port RC6, izlazni
    TRISCbits.TRISC7 = UNUSED_TRIS;     // SDO, serial data output, port RC7, izlazni
    
    TRISAbits.TRISA5 = 0;               // IRQ = 1 turn off sensors           
    TRISAbits.TRISA4 = 0;               // CE, chip enable, port RA4, izlazni

    TRISBbits.TRISB4 = UNUSED_TRIS;
    TRISBbits.TRISB5 = 0;               // switch A
    TRISBbits.TRISB6 = UNUSED_TRIS;
    TRISBbits.TRISB7 = 0;               // switch B                 
}

void timer_setup_and_start_IE(void)
{
    // Timer1 set-up
    T1CONbits.TMR1ON = 0;                     // Timer1 OFF
    T1CONbits.nT1SYNC = 1;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.T1CKPS = 0b11;                     // 11 - 8, 00 - 1  
    T1CONbits.TMR1CS = 0b11;                    // 00 - FOSC/4, 01 - FOSC
    // Preload to get 10 sec interrupt
    TMR1H = 0x68;           
    TMR1L = 0x80;
    TMR_CNT = 0;
    
    T1CONbits.TMR1ON = 1;                     // Timer1 ON
    
    PIR1bits.TMR1IF = 0;                      // Clear flag

    INTCON = 0x00;
    INTCONbits.PEIE = 1;                // Enable Peripheral Interrupts
    INTCONbits.GIE = 1;                 // Enable Global Interrupt  
    PIE1bits.TMR1IE = 1;                // Enable interrupt
    
    // Timer0 set-up
   
    // Timer configuration
    
    OPTION_REGbits.INTEDG = 1;
    OPTION_REGbits.nWPUEN=1;
    OPTION_REGbits.TMR0CS=0;  
    OPTION_REGbits.TMR0SE=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS0=0;   // prescaler 8
    OPTION_REGbits.PS1=1;
    OPTION_REGbits.PS2=0;
    
    //  Period = (256 - TMR0)*(4/fosc)*(Prescaler)
    TMR0	 = 0x83;        // 0,5 ms
    
    INTCONbits.GIE  = 1;    // Enable Global Interrupt 
    INTCONbits.TMR0IE = 0;  // Enable interrupt

    // Timer2 set-up
    //PIR1bits.TMR2IF = 0;                      // clear flag   
    //T2CONbits.T2CKPS = 11;                    // 11 = Prescaler is 64, 00 = Prescaler is 1
    //T2CONbits.TMR2ON = 1;                     // Timer2 ON       
    //PIE1bits.TMR2IE = 1;                      // Enable the interrupt         
}

