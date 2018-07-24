
#include <xc.h>
#include <pic16lf1459.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "user.h"
#include "system.h"
#include "spi.h"
#include "nRF24L01P.h"

void nRF_Setup()
{  
    //setting RXTX adress
    uint8_t RXTX_ADDR[5];// = { 0xAA, 0xAA, 0xAA}; //adresa
    RXTX_ADDR[4] = 0xAB;
    RXTX_ADDR[3] = 0xAC;
    RXTX_ADDR[2] = 0xAD;
    RXTX_ADDR[1] = 0xAE;
    RXTX_ADDR[0] = 0xAF;
    
    CE = 0;
    WriteAddress(TX_ADDR, 5, RXTX_ADDR);        // TX addr, 5 bytes
    WriteAddress(RX_ADDR_P0, 5, RXTX_ADDR);     // RX addr pipe 0
    WriteRegister(EN_AA, 0x01);                 // enable AA on data pipe 1
    WriteRegister(EN_RXADDR, 0x01);             // enable RX address on data pipe 1
    WriteRegister(SETUP_AW, 0x03);              // Address width,'01'-3 bytes,'10'-4 bytes,'11'-5 bytes
    WriteRegister(SETUP_RETR, 0x2F);            // wait 750 us, up to 15 retransmissions
    WriteRegister(RF_CH, 0x2C);                 // Channel 44 --> 2.4 GHz + 20 MHz
    WriteRegister(RF_SETUP, 0x06);              // 1 Mbps, 0 dBm
    WriteRegister(NRF_STATUS,0x70);             // Reset status register
    WriteRegister(RX_PW_P0, 0x05);              // Number of bytes in RX payload in data pipe 0
    WriteRegister(NRF_CONFIG,0x7A);             // pwr_up , ptx, en crc, 1 byte, interrupt not reflected
    __delay_ms(2); //POWER-ON TIME 1.5ms specified
    
 
}
//FUNCTIONS
//flush TX and RX
void FlushTXRX()
{
    WriteRegister(NRF_STATUS, 0x70);
    WriteCommand(FLUSH_RX);
    WriteCommand(FLUSH_TX);
}

// Write to the register
void WriteRegister(uint8_t reg, uint8_t val)
{
    CSN = 0;                                    // CSN enable, negative logic
    //MSB first, first command then value
    SPI_transfer(W_REG | reg); 
    SPI_transfer(val);
    CSN = 1;                                    // CSN disable
}

//Address is 3-5 bytes, LSB first
void WriteAddress(uint8_t reg, uint8_t num, uint8_t * addr)
{
    CSN = 0;
    SPI_transfer(W_REG | reg);
    for (int i=0; i<num; i++)
    SPI_transfer(addr[i]);
    CSN = 1;
}

uint8_t ReadRegister(uint8_t reg)
{
  uint8_t temp;
  uint8_t temp1;
 
  CSN = 0;                      //CSN low, enable
  temp = R_REG | reg;
  SPI_transfer(temp);           //transfer command
  
  temp1= SPI_transfer(R_STATUS); // 1 byte dummy
  CSN = 1;
  
  return temp1;
}

void WriteCommand(uint8_t command) //write command word
{
  CSN = 0;   //active
  SPI_transfer(command); //transfer command
  CSN = 1; //inactive
}

void WritePayload(uint8_t num, uint8_t *data)
{
    CSN = 0;                            
    SPI_transfer(TX_PAYLOAD);           // transfer to the buffer
    for ( uint8_t i=0; i<num; i++)      // transfer all bytes
        SPI_transfer(data[i]);                            
    CSN = 1;                            // inactive
    //pulse CE
    CE = 1;
    __delay_us(12);                     // > 10 us
    CE = 0;
}