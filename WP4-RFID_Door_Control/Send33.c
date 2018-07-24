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

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

// CONFIG1
#pragma config FOSC = INTOSC        // Oscillator Selection Bits (ECH, External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pins)
#pragma config WDTE = OFF           // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF          // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON           // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF             // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF          // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF       // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF           // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

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

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/


void main(void) 
{

    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC7 = 0; // SDO, serial data output
    
    REDLED = 0;
    GREENLED = 0;

    while (1)
  {
        REDLED = 0;
        GREENLED = 0;
        PORTCbits.RC7 = 0;

        if (PORTAbits.RA0 == 1) // A
        {
            GREENLED = 1;  // LED ON
            REDLED = 1;
            PORTCbits.RC7 = 1;
            __delay_ms(30); // 2                                                                                                            Second Delay
        }

  }
}

