#include "nrf24l01.h"



int NRF24L01P::init(){

	int ret;

	CE = 1;


	CsDisable();

	setSpiParams();


	uint8_t RXTX_ADDR[5];// same address as the yawns

	RXTX_ADDR[4] = 0xAB;
	RXTX_ADDR[3] = 0xAC;
	RXTX_ADDR[2] = 0xAD;
	RXTX_ADDR[1] = 0xAE;
	RXTX_ADDR[0] = 0xAF;





	ret = writeRx(EN_AA, 0x01); // enable AA on data pipe 0




	if(ret != RF_RET_SUCCESS){



				return ret;
			}


	ret= writeRx(EN_RXADDR, 0x01); // enable RX address on data pipe 0

	if(ret != RF_RET_SUCCESS){



				return ret;
			}


	ret = writeRx(SETUP_AW,0x03); // Address width,'01'-3 bytes,'10'-4 bytes,'11'-5 bytes

	if(ret != RF_RET_SUCCESS){



				return ret;
			}

	ret = writeRx(SETUP_RETR, 0x2F); // wait 750 us, up to 15 retransmissions

	if(ret != RF_RET_SUCCESS){



				return ret;
			}

	ret = writeRx(RF_CH,0x2C); // Channel 44 --> 2.4 GHz + 44 MHz

	if(ret != RF_RET_SUCCESS){



				return ret;
			}

	ret = writeRx(RF_SETUP, 0x06); // 1 Mbps, 0 dBm
	//WriteRegister(RF_SETUP, 0x26);     // 250 kbps, 0 dBm
	//WriteRegister(RF_SETUP, 0x00);     // 1 Mbps, -18 dBm

	if(ret != RF_RET_SUCCESS){



				return ret;
			}

	ret = writeRx(RX_PW_P0, 0x05); // Number of bytes in RX payload in data pipe 0

	if(ret != RF_RET_SUCCESS){



				return ret;
			}

	ret = writeRx(NRF_CONFIG,0x7B); // pwr_up , pRx, en crc, 1 byte, interrupt not reflected +

	if(ret != RF_RET_SUCCESS){



				return ret;
			}

	writeRx(NRF_STATUS,0x70); // Reset status register


	WriteAddress(RX_ADDR_P0, 5, RXTX_ADDR); // RX addr pipe 0

	WriteAddress(TX_ADDR, 5, RXTX_ADDR); // TX addr, 5 bytes


	WriteCommand(FLUSH_RX);
	WriteCommand(FLUSH_TX);
	CE=1;
	wait_ms(2); // power up time, 1.5 ms specified
	return(ret);


}

void NRF24L01P::WriteCommand(uint8_t command){

	setSpiParams();

	CsDisable();

	CsEnable();

	spi.write(command);

	CsDisable();

	wait_us(4);
	//reset the SPI parameters to base board default.
	resetSpiParams();

}



void NRF24L01P::WriteAddress(uint8_t reg, uint8_t num, uint8_t * addr){

	uint8_t cmd;

	setSpiParams();

	CsDisable();

	CsEnable();

	cmd = W_REG | reg;

	spi.write(cmd);
	for(int i=0; i<num;i++){
		spi.write(addr[i]);
	}

	CsDisable();

	wait_us(4);
	//reset the SPI parameters to base board default.
	resetSpiParams();



}


int NRF24L01P::registerRead (uint8_t reg){

	uint8_t temp;
	uint8_t temp1;

	//set SPI parameters
	setSpiParams();

    CsDisable();

    CsEnable();

    temp = R_REG | reg;
    spi.write(temp);

    temp1=spi.write(R_STATUS);

    CsDisable();
   	//reset the SPI line configuration.
    resetSpiParams();

	return(temp1);

}




int NRF24L01P::writeRx(uint8_t reg,uint8_t data){

	uint8_t updated_reg_val;
	uint8_t cmd;

	//set SPI parameters
	setSpiParams();

	CsDisable();
	CsEnable();


	cmd=W_REG | reg;
	spi.write(cmd);
	spi.write(data);

	CsDisable();

	wait_us(4);
	//reset the SPI parameters to base board default.
	resetSpiParams();



	updated_reg_val = registerRead(reg);

	    //check if value written successfully or not.
	    if(updated_reg_val == data){


	        return RF_RET_SUCCESS;
	    }else {


	        return RF_SPI_ERR;
	    }

}

void NRF24L01P::ReadPayload(uint8_t num, uint8_t *data){

	//set SPI parameters
	setSpiParams();

	CsDisable();
	CsEnable();


	spi.write(RX_PAYLOAD); // command to read a payload

	for(uint8_t i=0;i<num;i++){
		data[i]=spi.write(R_STATUS); // send dummy bytes to read incoming data
	}

	CsDisable();

	wait_us(4);
	//reset the SPI parameters to base board default.
	resetSpiParams();

	CE=1;
	wait_ms(10);



}

void NRF24L01P::WritePayload(uint8_t num,uint8_t *data){
	//set SPI parameters
	setSpiParams();

	CsDisable();
	CsEnable();

	spi.write(TX_PAYLOAD); // command to write a payload

	for(uint8_t i=0;i<num;i++){
		spi.write(data[i]);
	}

	CsDisable();

	CE=1;

	wait_us(12);

	CE=0;

	//reset the SPI parameters to base board default.
	resetSpiParams();




}


void NRF24L01P::CsEnable(void)
{





	wait_us(7);


	//set cs = 0;
	CSN=0;




}


// i2cCsDisable()
// disable CS for the LED driver module.
void NRF24L01P::CsDisable(void)
{


	//set cs = 1;
	CSN=1;



	wait_us(7);
}


void NRF24L01P::setSpiParams(void)
{
	spi.format(RF_SPI_FRAME_SIZE, RF_SPI_MODE);
	spi.frequency(RF_SPI_FREQUENCY);
}

//Sets the SPI parameters to default values.Should be called
//after each SPI communication to led driver.
void NRF24L01P::resetSpiParams(void)
{
	int bits = 8;
	int mode = 0;
	int freq = 1000000;

	spi.format(bits, mode);
	spi.frequency(freq);
}

