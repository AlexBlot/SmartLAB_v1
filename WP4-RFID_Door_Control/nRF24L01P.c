
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

#define _BV(n) (1 << n)


//FUNCTIONS
//flush TX and RX
void FlushTXRX()
{
    WriteRegisterRF(NRF_STATUS, 0x70);
    WriteCommand(FLUSH_RX);
    WriteCommand(FLUSH_TX);
}

// Write to the register
void WriteRegisterRF(uint8_t reg, uint8_t val)
{
    CSN_RF = 0;                                    // CSN_RF enable, negative logic
    //MSB first, first command then value
    SPI_transfer(W_REG | reg); 
    SPI_transfer(val);
    CSN_RF = 1;                                    // CSN_RF disable
}

//Address is 3-5 bytes, LSB first
void WriteAddress(uint8_t reg, uint8_t num, uint8_t * addr)
{
    CSN_RF = 0;
    SPI_transfer(W_REG | reg);
    for (int i=0; i<num; i++)
    SPI_transfer(addr[i]);
    CSN_RF = 1;
}

uint8_t ReadRegisterRF(uint8_t reg)
{
  uint8_t temp;
  uint8_t temp1;
 
  CSN_RF = 0;                      //CSN_RF low, enable
  temp = R_REG | reg;
  SPI_transfer(temp);           //transfer command
  
  temp1= SPI_transfer(R_STATUS); // 1 byte dummy
  CSN_RF = 1;
  
  return temp1;
}

void WriteCommand(uint8_t command) //write command word
{
  CSN_RF = 0;   //active
  SPI_transfer(command); //transfer command
  CSN_RF = 1; //inactive
}

void WritePayload(uint8_t num, uint8_t *data)
{
    CSN_RF = 0;                            
    SPI_transfer(TX_PAYLOAD);           // transfer to the buffer
    for ( uint8_t i=0; i<num; i++)      // transfer all bytes
        SPI_transfer(data[i]);                            
    CSN_RF = 1;                            // inactive
    //pulse CE
    CE = 1;
    __delay_us(12);                     // > 10 us
    CE = 0;
}

void ReadPayload(uint8_t num, uint8_t *data)
{
    CSN_RF = 0;                            
    SPI_transfer(RX_PAYLOAD);           // read to the buffer
    for ( uint8_t i=0; i<num; i++)      // read all bytes
        data[i] = SPI_transfer(0xFF);                            
    CSN_RF = 1;                            // inactive
    __delay_ms( 1 );
    
}



void nRF_Setup_Write()
{  
    //setting RXTX address
    uint8_t RXTX_ADDR[5];// = { 0xAA, 0xAA, 0xAA}; //address
    RXTX_ADDR[4] = 0xAB;
    RXTX_ADDR[3] = 0xAC;
    RXTX_ADDR[2] = 0xAD;
    RXTX_ADDR[1] = 0xAE;
    RXTX_ADDR[0] = 0xAF;
    
    CE = 0;
    WriteAddress(TX_ADDR, 5, RXTX_ADDR);          // TX addr, 5 bytes
    WriteAddress(RX_ADDR_P0, 5, RXTX_ADDR);       // RX addr pipe 0
    WriteRegisterRF(EN_AA, 0x01);                 // enable AA on data pipe 1
    WriteRegisterRF(EN_RXADDR, 0x01);             // enable RX address on data pipe 1
    WriteRegisterRF(SETUP_AW, 0x03);              // Address width,'01'-3 bytes,'10'-4 bytes,'11'-5 bytes
    WriteRegisterRF(SETUP_RETR, 0x2F);            // wait 750 us, up to 15 retransmissions
    //WriteRegisterRF(RF_CH, 0x14);                 // Channel 20 --> 2.4 GHz + 20 MHz
    WriteRegisterRF(RF_CH, 0x2C);                 // Channel 44 --> 2.4 GHz + 44 MHz
    WriteRegisterRF(RF_SETUP, 0x06);              // 1 Mbps, 0 dBm
    WriteRegisterRF(NRF_STATUS,0x70);             // Reset status register
    WriteRegisterRF(RX_PW_P0, 0x05);              // Number of bytes in RX payload in data pipe 0
    WriteRegisterRF(NRF_CONFIG,0x7A);             // pwr_up , ptx, en crc, 1 byte, interrupt not reflected
    __delay_ms(2); //POWER-ON TIME 1.5ms specified
    
 
}



void nRF_Setup_Read()
{  
    //setting RXTX address
    uint8_t RXTX_ADDR[5];// = { 0xAA, 0xAA, 0xAA}; //address
    RXTX_ADDR[4] = 0xAB;
    RXTX_ADDR[3] = 0xAC;
    RXTX_ADDR[2] = 0xAD;
    RXTX_ADDR[1] = 0xAE;
    RXTX_ADDR[0] = 0xAF;
    

    WriteAddress(TX_ADDR, 5, RXTX_ADDR);          // TX addr, 5 bytes
    WriteAddress(RX_ADDR_P0, 5, RXTX_ADDR);       // RX addr pipe 0
    WriteRegisterRF(EN_AA, 0x01);                 // enable AA on data pipe 1
    WriteRegisterRF(EN_RXADDR, 0x01);             // enable RX address on data pipe 1
    WriteRegisterRF(SETUP_AW, 0x03);              // Address width,'01'-3 bytes,'10'-4 bytes,'11'-5 bytes
    WriteRegisterRF(SETUP_RETR, 0x2F);            // wait 750 us, up to 15 retransmissions
    //WriteRegisterRF(RF_CH, 0x14);                 // Channel 20 --> 2.4 GHz + 20 MHz
    WriteRegisterRF(RF_CH, 0x28);                 // Channel 40 --> 2.4 GHz + 40 MHz
    WriteRegisterRF(RF_SETUP, 0x06);              // 1 Mbps, 0 dBm
    WriteRegisterRF(NRF_STATUS,0x70);             // Reset status register
    WriteRegisterRF(RX_PW_P0, 0x05);              // Number of bytes in RX payload in data pipe 0
    WriteRegisterRF(NRF_CONFIG,0x7B);             // pwr_up , ptx, en crc, 1 byte, interrupt not reflected
   
    
    __delay_ms(2); //POWER-ON TIME 1.5ms specified
    
    CE = 1;
    
 
}

uint8_t nRF_available()
{     
     if (( ReadRegisterRF(NRF_STATUS) == 0x40 )) //_BV(RX_EMPTY) - _BV(6)
     {  
        return 1;
     }    
     return 0;
}