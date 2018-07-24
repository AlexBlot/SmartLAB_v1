/* 
 * File:   send_current_main.c
 * Author: student
 *
 * Created on 2018. srpnja 12, 10:09
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>             /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>            /* HiTech General Include File */
#endif
#include <stdint.h>             /* For uint8_t definition */
#include <stdbool.h>            /* For true/false definition */
#include <pic16lf1459.h>
#include "system.h"             /* System funct/params, like osc/peripheral config */
#include "user.h"               /* User funct/params, such as InitApp */      
#include "nRF24L01P.h"
#include "spi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

// CONFIG1
#pragma config FOSC = INTOSC        // Oscillator Selection Bits (ECH, External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pins)
#pragma config WDTE = ON            // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF          // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON           // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF             // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF          // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF       // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF           // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)
//
// CONFIG2
#pragma config WRT = OFF            // Flash Memory Self-Write Protection (Write protection off)
#pragma config CPUDIV = NOCLKDIV    // CPU System Clock Selection Bit (CPU system clock divided by 6)
#pragma config USBLSCLK = 48MHz     // USB Low SPeed Clock Selection bit (System clock expects 48 MHz, FS/LS USB CLKENs divide-by is set to 8.)
#pragma config PLLMULT = 3x         // PLL Multipler Selection Bit (3x Output Frequency Selected)
#pragma config PLLEN = DISABLED     // PLL Enable Bit (3x or 4x PLL Enabled)
#pragma config STVREN = OFF         // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO            // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF          // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF            // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
//
///******************************************************************************/
/* User Global Variable Declaration                                           */

static uint32_t millisecond = 0;
uint32_t adcUpdate_timestamp = 0;

#define ADC_BUFFER_LENGTH   40
int16_t adc_data[ADC_BUFFER_LENGTH] = {0};
uint8_t adc_data_index = 0;
uint16_t ac_value = 0;
uint8_t tmrcmpt = 0;

/******************************************************************************/
/* Interrupt routine                                                          */
/******************************************************************************/

void interrupt ISR_ROUTINE( )
{
    //ConfigureOscillator();
  InitApp();
  TRISCbits.TRISC3=1;     // RC3 AN7 input
  ANSELCbits.ANSC3=1;     // RC3 as Analog   
  /******************************************************************************/
  /* Interrupt sending data every 60 sec                                        */
  /******************************************************************************/
  if(PIR1bits.TMR1IF)
  {
          PIR1bits.TMR1IF = 0;
          T1CONbits.TMR1ON = 0; 
          //INTCONbits.GIE = 0;  // Timer1 OFF
          //PORTCbits.RC4=~PORTCbits.RC4;
          PIE1bits.TMR1IE = 0;
          
        tmrcmpt++;
        if (tmrcmpt==6)
        {
          LATCbits.LATC0=1;
          INTCONbits.TMR0IE=1;
          /******************************************************************************/
          /* Interrupt every millisecond during 40 ms to get adc values                 */
          /******************************************************************************/ 
          __delay_ms(5);

          while (adc_data_index<40)
          {
              if( TMR0IF )
              {
                //  Period = (256 - TMR0)*(4/fosc)*(Prescaler)
                TMR0 = 0x83;
                //PORTCbits.RC5=~PORTCbits.RC5; // sampling period check
                INTCONbits.TMR0IF = 0;

                // AD conversion
                ADCON0bits.GO_nDONE=1;      // Ready to start a new conversion
                while (ADCON0bits.GO_nDONE==1);
                uint8_t adc_lo = ADRESL;
                uint8_t adc_hi = ADRESH;
                adc_data[adc_data_index] =  ((uint16_t)(adc_hi << 8) | (uint16_t)adc_lo);
                adc_data_index++;         
              }
          }
          INTCONbits.TMR0IE=0;
          INTCONbits.GIE = 0;  
          adc_data_index = 0;      
          // Time to Process
          uint32_t adc_filter_val = 0;
          uint32_t dc_filter_val = 0;
          ac_value = 0;
          uint8_t i = 0;
          for(i=0; i < ADC_BUFFER_LENGTH; i++ )
          {
            dc_filter_val += (uint32_t)(adc_data[i]);
          }
          dc_filter_val = dc_filter_val / (ADC_BUFFER_LENGTH);
          dc_filter_val = 512 - dc_filter_val;
          for(i=0; i < ADC_BUFFER_LENGTH; i++ )
          {
            adc_data[i] = (adc_data[i] + (dc_filter_val)) ;
            adc_filter_val += ((uint32_t)((adc_data[i]-512)*(uint32_t)(adc_data[i]-512) ));       
          }

          adc_filter_val /= (ADC_BUFFER_LENGTH);      // mean squared sum
          ac_value = (uint16_t)sqrt(adc_filter_val);
          TMR0 = 0x83;
          /* Initialize I/O and Peripherals for application */  
          SPI_init();   
          WriteRegister(NRF_CONFIG, 0x00);        // turn off module       

          // send message

          CE = 0;
          nRF_Setup(); 
          FlushTXRX();
          WriteRegister(NRF_STATUS,0x70);         // Reset status register
          __delay_ms(2);
          CE = 1;
          __delay_us(150);         

          uint8_t data[5] = {0,0,0,0,0};           // four bytes to be compatible to developed USB hub 
          data[0] = 2;
          data[1] = 13;
          data[2] = ac_value;
          data[3] = (ac_value >> 8);

          WritePayload(5, data); 
          __delay_ms(5);

          FlushTXRX();
          WriteRegister(NRF_CONFIG, 0x00);        // turn off module 
          tmrcmpt=0;
          LATCbits.LATC0=0;
          }
          PIE1bits.TMR1IE = 1;
          T1CONbits.TMR1ON = 1;                     // Timer1 ON
          //ConfigureOscillator_interrupt();
          INTCONbits.GIE = 1;
          InitApp();
        TMR1H = 0x68;           
        TMR1L = 0x80; 
    } 
}

  /******************************************************************************/
  /* Initialisation of the AD converter                                      */
  /******************************************************************************/

void init_adc (void)
{
  // Initialisation
    
    ADRESH=0x00;
    ADRESL=0x00;
    
    // SELECT CLOCK HERE ITS 101 = Fosc/16
   
    ADCON1bits.ADCS0=1;
    ADCON1bits.ADCS1=0;
    ADCON1bits.ADCS2=1;
    
    // SELECT CHANNEL HERE ITS AN7

    ADCON0bits.CHS0=1;
    ADCON0bits.CHS1=1;
    ADCON0bits.CHS2=1;
    ADCON0bits.CHS3=0;
    ADCON0bits.CHS4=0;
    
    // RESULT FORMAT
    
    ADCON1bits.ADFM=1; // Right justified six most significant bits of ADRESH are set to 0 when the conversion result is load
    
    // A/D Positive Voltage Reference Configuration bits
    
    ADCON1bits.ADPREF0=0;  // Vref+ is connected to Vcc
    ADCON1bits.ADPREF1=0;
    
    // Conversion status bit
    
    ADCON0bits.GO_nDONE=1;
   
    // SWITCH ON ADC
    
    ADCON0bits.ADON=1;  // ADC enabled
    
    //ADCON2bits.TRIGSEL=0b011;
 
}

void main(void)
{
    ConfigureOscillator();
    InitApp();   
    timer_setup_and_start_IE(); 
    init_adc();
    
   
    while(1)
    { 
        TRISCbits.TRISC0 = 0;               // ICSPDAT
        LATCbits.LATC0 = 0;
        TRISCbits.TRISC1 = 0;               // ICSPCLK
        LATCbits.LATC1 = 0;
        TRISCbits.TRISC2 = 0;
        LATCbits.LATC2 = 0;
        TRISCbits.TRISC3 = 0;
        LATCbits.LATC3 = 0;
        TRISCbits.TRISC4 = 0;               // green led output
        LATCbits.LATC4 = 0;
        TRISCbits.TRISC5 = 0;               // red led output
        LATCbits.LATC5 = 0;
        TRISCbits.TRISC6 = 0;               // CSN, chip select not, port RC6, izlazni
        LATCbits.LATC6 = 0;
        TRISCbits.TRISC7 = 0;               // SDO, serial data output, port RC7, izlazni
        LATCbits.LATC7 = 0;
        
        TRISAbits.TRISA5 = 0;               // IRQ = 1 turn off sensors           
        LATAbits.LATA5 = 0;
        TRISAbits.TRISA4 = 0;               // CE, chip enable, port RA4, izlazni
        LATAbits.LATA4 = 0;
        
        TRISBbits.TRISB4 = 0;
        LATBbits.LATB4 = 0;
        TRISBbits.TRISB5 = 1;               // switch A
        TRISBbits.TRISB6 = 0;
        LATBbits.LATB6 = 0;
        TRISBbits.TRISB7 = 1;               // switch B 
        SLEEP();
    }
}

