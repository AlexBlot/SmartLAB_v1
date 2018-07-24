/* 
 * File:   nRF24L01P.h
 * Author: Ninoslav Budimir
 *
 * Created on 07 July 2016, 11:39
 */

#ifndef NRF24L01P_H
#define	NRF24L01P_H

#ifdef	__cplusplus
extern "C" {
#endif

//commands for the SPI (nRF24l01+)
#define R_REG 0x00      		// writing to the register
#define W_REG 0x20      		// reading from the register
#define RF24_REGISTER_MASK 0x1F 
#define RX_PAYLOAD 0x61 		// take from the stack
#define TX_PAYLOAD 0xA0 		// put to the stack
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define ACTIVATE 0x50
#define R_STATUS 0xFF

//Registri nRF24l01+ modula s pripadajucim adresama
#define NRF_CONFIG 0x00 // konfiguracijski registar
#define EN_AA 0x01      // enable AutoAcknowledgement registar
#define EN_RXADDR 0x02  // enable RX addreses
#define SETUP_AW 0x03   // setup of addres widths
#define SETUP_RETR 0x04 // setup of automatic retransmision
#define RF_CH 0x05      // RF channel
#define RF_SETUP 0x06   // RF setup registar
#define NRF_STATUS 0x07 // NRF status register
#define OBSERVE_TX 0x08 // tramsmit observe registar
#define CD 0x09         // carrier detect
#define RX_ADDR_P0 0x0A // Receive adress data - pipe 0, 5 bytes
#define RX_ADDR_P1 0x0B // Receive address data - pipe 1, 5 bytes
#define RX_ADDR_P2 0x0C // 1 byte
#define RX_ADDR_P3 0x0D // 1 byte
#define RX_ADDR_P4 0x0E // 1 byte 
#define RX_ADDR_P5 0x0F // 1 byte
#define TX_ADDR 0x10    // transmit address, 5 bytes
#define RX_PW_P0 0x11   // Number of bytes in RX payload in data pipe0,pipe1, pipe2, pipe3, pipe4, pipe5
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17 // FIFO status registar
#define DYNPD 0x1C
#define FEATURE 0x1D    

void nRF_Setup();
void FlushTXRX();
void WriteRegister(uint8_t reg, uint8_t val);
void WriteAddress(uint8_t reg, uint8_t num, uint8_t* addr);
uint8_t ReadRegister(uint8_t reg);
void WriteCommand(uint8_t command);
void WritePayload(uint8_t num, uint8_t* data);
    
#ifdef	__cplusplus
}
#endif

#endif	/* NRF24L01P_H */

