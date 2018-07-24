
#include <xc.h>
#include <pic16lf1459.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "spi.h"
#include "user.h"

void SPI_init()
{
    SSPCON1bits.SSPEN = 1;      //enable serial port
    SSPBUF = 0;                 //clear SPI buffer    
    
    ANSELA = 0x00;              // Turn off analog inputs port A
    ANSELB = 0x00;              // Turn off analog inputs port B
    ANSELC = 0x00;              // Turn off analog inputs port C
    
    TRISBbits.TRISB6 = 0;   	// SCK ,clock, output
    TRISBbits.TRISB4 = 1;   	// SDI , serial data input
    TRISCbits.TRISC7 = 0;   	// SDO, serial data output
    TRISCbits.TRISC6 = 0;   	// CSN, chip select not, output
    TRISAbits.TRISA4 = 0;   	// CE, chip enable, output
    
    CSN = 1;                    // pin CSN disable
    CE = 0;                     // pin CE disable
    
    
    SSPSTATbits.CKE = 1;    // 0 - Transmit occurs on transition from Idle to active clock state, 
                            // 1 - Transmit occurs on transition from Active to Idle clock state,
    SSPCON1bits.CKP = 0;    // 0 - idle state for a clock is a low level
                            // 1 - idle state for a clock is a high level
    SSPSTATbits.SMP = 0;    // 1 - podatak je uzorkovan na kraju izlaznog vremena podataka
                            // 0 - na sredini
    // SPI master mode
    // 0000 = SPI Master mode, clock = FOSC/4
    // 0001 = SPI Master mode, clock = FOSC/16 
    // 0010 = SPI Master mode, clock = FOSC/64 
    SSPCON1bits.SSPM = 0b0000;
}

//function to transfer data to the buffer
uint8_t SPI_transfer(uint8_t data)  // 8 bits SPI transfer
{
    SSPBUF = data;                  // Put command into SPI buffer
    while (!SSPSTATbits.BF);        // Wait for the transfer to finish - if BF then buffer is full
    return SSPBUF;                  // Return buffer value           
}