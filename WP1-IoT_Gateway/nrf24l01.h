#ifndef _NRF24L01_H
#define	_NRF24L01_H

#include "SPI2.h"
#include "mbed.h"

//SPI configuration parameters.
#define RF_SPI_FRAME_SIZE	(8)
#define	RF_SPI_FREQUENCY	(1000000)
#define	RF_SPI_MODE			(0) //MODE 0 SPI interface
#define	RF_MOSI				(p5)
#define RF_MISO				(p6)
#define	RF_SCLK				(p4)


//komandne naredbe za SPI (nRF24l01+)
#define R_REG 0x00      //
#define W_REG 0x20      //
#define RF24_REGISTER_MASK 0x1F
#define RX_PAYLOAD 0x61 //uzimaj sa stoga
#define TX_PAYLOAD 0xA0 //stavi na stog
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define ACTIVATE 0x50
#define R_STATUS 0xFF

//Registri nRF24l01+ modula s pripadaju?im adresama
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
#define RX_ADDR_P2 0x0C // 1 bajt
#define RX_ADDR_P3 0x0D // 1 bajt
#define RX_ADDR_P4 0x0E // 1 bajt
#define RX_ADDR_P5 0x0F //1 bajt
#define TX_ADDR 0x10    // transmit address, 5 bytes
#define RX_PW_P0 0x11   // Number of bytes in RX payload in data pipe0,pipe1, pipe2, pipe3, pipe4, pipe5
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17 //FIFO status registar
#define DYNPD 0x1C
#define FEATURE 0x1D

#define	RF_RET_SUCCESS	(0) 	///<API return value, for successful call.
#define	RF_SPI_ERR		(-2)	///<API return value, if the API encounters an SPI error.

DigitalOut CSN(p7);
DigitalOut CE(p16);
DigitalOut CSWifi(p14);

class NRF24L01P{
	public:
		/// Constructor.
		/// Initializes object for Radio Module class.
		NRF24L01P() : spi(RF_MOSI, RF_MISO, RF_SCLK){};


		///Read registers (generic).
		///@param reg(in)
		///		Required register address.
		///@param pData(in/out)
		///		Pointer to data which will store the read value.
		///@return
		///		#LED_SPI_ERR, if function exits due to some SPI errors.\n
		///		#LED_INVALID_ARG, if NULL pData pointer is passed. \n
		///		#LED_RET_SUCCESS, on success.
		int registerRead (uint8_t reg);

		///Write to a  register.
		///@param reg(in)

		///@param data(in)
		///		Data to be written on  register.
		///@return
		///		#LED_SPI_ERR, if function exits due to some SPI errors.\n
		///		#LED_RET_SUCCESS, on success.
		int writeRx(uint8_t reg,uint8_t data);

		void WriteAddress(uint8_t reg, uint8_t num, uint8_t * addr);

		void WriteCommand(uint8_t command);

		void ReadPayload(uint8_t num, uint8_t *data);

		void WritePayload(uint8_t num, uint8_t *data);


		int init(void);

	private:

		///SPI class to communicate with NRF24L01P module.
		SPI2 spi;

		///Enable the spi interface with NRF24L01P module.
		///@param
		///		None
		///@return
		///		None
		void CsEnable(void);

		///Disable the SPI interface with NRF24L01P module.
		///@param
		///		None
		///@return
		///		None
		void CsDisable(void);

		///set the SPI interface parameters for interfacing with radio module.
		///@param
		///		none
		///@return
		///		none
		void setSpiParams(void);

		///Sets the SPI parameters to base board defaults.Should be called
		///after each SPI communication to radio module.
		///@param
		///		none
		///@return
		///		none
		void resetSpiParams(void);

};



#endif /* NRF24L01P_H */
