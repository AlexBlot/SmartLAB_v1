/*
 * File:   rc522.c
 * Author: Student
 *
 * Created on June 18, 2018, 2:27 PM
 */


#include <xc.h>
#include "rc522.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "user.h"
#include "system.h"
#include "spi.h"



// Write to the register
void WriteRegister(uint8_t reg, uint8_t val)
{
    CSN_RFID = 0;                                    // CSN enable, negative logic
    //MSB first, first command then value
    SPI_transfer(( reg << 1 ) & 0x7E);
    SPI_transfer(val);
    CSN_RFID = 1;                                    // CSN disable
}

////Address is 3-5 bytes, LSB first
//void WriteAddress(uint8_t reg, uint8_t num, uint8_t * addr)
//{
////    CSN = 0;
//    SPI_transfer(((reg<<1)&0x7E)| reg);
//    for (int i=0; i<num; i++)
//    SPI_transfer(addr[i]);
////    CSN = 1;
//}



uint8_t ReadRegister(uint8_t reg)
{

    uint8_t val;
    
    CSN_RFID = 0;                      //CSN low, enable
  
    SPI_transfer((( reg << 1 ) & 0x7E) | 0x80);         //transfer command
    val=SPI_transfer(0x00);
    //PI_transfer(0x00); // Read the value back. Send 0 to stop reading.
  
    CSN_RFID = 1;
  
    return val;
}


void MFRC522_Clear_Bit( char addr, char mask )
{
     WriteRegister( addr, ReadRegister( addr ) & (~mask) );
}
void MFRC522_Set_Bit( char addr, char mask )
{
     WriteRegister( addr, ReadRegister( addr ) | mask );
}
void MFRC522_Reset()
{
    WriteRegister( COMMANDREG, PCD_RESETPHASE );

}
void MFRC522_AntennaOn()
{
 MFRC522_Set_Bit( TXCONTROLREG, 0x03 );
}
void MFRC522_AntennaOff()
{
 MFRC522_Clear_Bit( TXCONTROLREG, 0x03 );
}
void MFRC522_Init()
{
    GREENLED = 1; // turn ON red LED   
    __delay_ms(50);
    GREENLED = 0; // turn OFF red LED  
    REDLED = 1; // turn ON green LED  
    __delay_ms(50);
    REDLED = 0; // turn OFF green LED 
    
  
    
     MFRC522_Reset();

     
    WriteRegister(TMODEREG, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    WriteRegister( TPRESCALERREG, 0x3E ); //TModeReg[3..0] + TPrescalerReg
    
    WriteRegister( TRELOADREGL, 30 );       
    WriteRegister( TRELOADREGH, 0 );
        
    WriteRegister( TXAUTOREG, 0x40 );    // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
    WriteRegister( MODEREG, 0x3D );      // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)

        
     //MFRC522_Clear_Bit( STATUS2REG, 0x08 );//MFCrypto1On=0
     //MFRC522_Wr( RXSELREG, 0x86 );      //RxWait = RxSelReg[5..0]
     //MFRC522_Wr( RFCFGREG, 0x7F );     //RxGain = 48dB
     MFRC522_AntennaOn();
}

/**************************************************************************/
/*!

  @brief   Sends a command to a tag.

  @param   cmd     The command to the MFRC522 to send a command to the tag.
  @param   data    The data that is needed to complete the command.
  @param   dlen    The length of the data.
  @param   result  The result returned by the tag.
  @param   rlen    The number of valid bits in the resulting value.

  @returns Returns the status of the calculation.
           MI_ERR        if something went wrong,
           MI_NOTAGERR   if there was no tag to send the command to.
           MI_OK         if everything went OK.

 */
/**************************************************************************/

char MFRC522_ToCard( char command, char *sendData, char sendLen, char *backData, unsigned int *backLen )
{
  char _status = MI_ERR;
  char irqEn = 0x00;
  char waitIRq = 0x00; ///< The bits in the ComIrqReg register that signals successful completion of the command.
  char lastBits;
  char n;
  unsigned int i;
 
  /*
    char *sendData;		///< Pointer to the data to transfer to the FIFO.
	char sendLen;		///< Number of bytes to transfer to the FIFO.
	char *backData;		///< nullptr or pointer to buffer if data should be read back after executing the command.
    char *backLen;		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
  */
  
  switch (command)
  {
    case PCD_AUTHENT:       //Certification cards close
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE:    //Transmit FIFO data
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
  }
  
  
  WriteRegister( COMMIENREG, irqEn | 0x80 );  //Interrupt request    
  MFRC522_Clear_Bit( COMMIRQREG, 0x80 );   //Clear all interrupt request bit
  MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );   //FlushBuffer=1, FIFO Initialization
  WriteRegister( COMMANDREG, PCD_IDLE );    // Clear all seven interrupt request bits
  
  //Writing data to the FIFO
  for ( i=0; i < sendLen; i++ )
  {
    WriteRegister( FIFODATAREG, sendData[i] );
  }
  //Execute the command
  WriteRegister( COMMANDREG, command );
  if (command == PCD_TRANSCEIVE )
  {
    MFRC522_Set_Bit( BITFRAMINGREG, 0x80 ); //StartSend=1,transmission of data starts  
  }
  //Waiting to receive data to complete
  //i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
  
  i = 2000;    
  do
  {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = ReadRegister( COMMIRQREG );
    i--;
  } while ((i!=0) && !(n&0x01) && !(n&waitIRq));
  
          
  MFRC522_Clear_Bit( BITFRAMINGREG, 0x80 );    //StartSend=0
  if (i != 0)
  {
    if( !( ReadRegister( ERRORREG ) & 0x1B ) ) //BufferOvfl Collerr CRCErr ProtecolErr
    {
     _status = MI_OK;   
      if ( n & irqEn & 0x01 )
      {
        _status = MI_NOTAGERR;       //??
      }
      if ( command == PCD_TRANSCEIVE )
      {
        n = ReadRegister( FIFOLEVELREG );
        lastBits = ReadRegister( CONTROLREG ) & 0x07;
        if (lastBits)
        {
          *backLen = (n-1) * 8 + lastBits;
        }
        else
        {
          *backLen = n * 8;
        }
        if (n == 0)
        {
          n = 1;
        }
        if (n > 16)
        {
          n = 16;
        }
        //Reading the received data in FIFO
        for (i=0; i < n; i++)
        {
          backData[i] = ReadRegister( FIFODATAREG );
        }
  
 // backData[i] = 0;
      }
    }
    else
    {
      _status = MI_ERR;
    }
  }
  //MFRC522_Set_Bit( CONTROLREG, 0x80 );
  //MFRC522_Wr( COMMANDREG, PCD_IDLE );
  return _status;
}


/**************************************************************************/
/*

  @brief   Checks to see if there is a tag in the vicinity.

  @param   mode  The mode we are requsting in.
  @param   type  If we find a tag, this will be the type of that tag.
                 0x4400 = Mifare_UltraLight
                 0x0400 = Mifare_One(S50)
                 0x0200 = Mifare_One(S70)
                 0x0800 = Mifare_Pro(X)
                 0x4403 = Mifare_DESFire

  @returns Returns the status of the request.
           MI_ERR        if something went wrong,
           MI_NOTAGERR   if there was no tag to send the command to.
           MI_OK         if everything went OK.

 */
/**************************************************************************/ 

char MFRC522_Request( char reqMode, char *TagType )
{
  char _status;
  unsigned backBits;            //The received data bits
  WriteRegister( BITFRAMINGREG, 0x07 ); //TxLastBists = BitFramingReg[2..0]   ???
  TagType[0] = reqMode;
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, TagType, 1, TagType, &backBits );
  if ( (_status != MI_OK) || (backBits != 0x10)) // 
  {
    _status = MI_ERR;
  }
  return _status;
}

/**************************************************************************/ 
/*SELF TEST*/
/*
int PerformSelfTest() {
	// This follows directly the steps outlined in 16.1.1
	// 1. Perform a soft reset.
	MFRC522_Reset();
	
	// 2. Clear the internal buffer by writing 25 bytes of 00h
	unsigned char ZEROES[25] = {0x00};
	WriteRegister(FIFOLEVELREG, 0x80);		// flush the FIFO buffer
	WriteAddress(FIFODATAREG, 25, ZEROES);	// write 25 bytes of 00h to FIFO
	WriteRegister(COMMANDREG, PCD_MEM);		// transfer to internal buffer
	
	// 3. Enable self-test
	WriteRegister(AUTOTESTREG, 0x09);
	
	// 4. Write 00h to FIFO buffer
	WriteRegister(FIFODATAREG, 0x00);
	
	// 5. Start self-test by issuing the CalcCRC command
	WriteRegister(COMMANDREG, PCD_CALCCRC);
	
	// 6. Wait for self-test to complete
	unsigned char n;
	for (uint8_t i = 0; i < 0xFF; i++) {
		// The datasheet does not specify exact completion condition except
		// that FIFO buffer should contain 64 bytes.
		// While selftest is initiated by CalcCRC command
		// it behaves differently from normal CRC computation,
		// so one can't reliably use DivIrqReg to check for completion.
		// It is reported that some devices does not trigger CRCIRq flag
		// during selftest.
		n = ReadRegister(FIFOLEVELREG);
		if (n >= 64) {
			break;
		}
	}
	WriteRegister(COMMANDREG, PCD_IDLE);		// Stop calculating CRC for new content in the FIFO.
	
	// 7. Read out resulting 64 bytes from the FIFO buffer.
	unsigned char result[64];
	ReadAddress(FIFODATAREG, 64, result, 0);
	
	// Auto self-test done
	// Reset AutoTestReg register to be 0 again. Required for normal operation.
	WriteRegister(AUTOTESTREG, 0x00);
	
	// Determine firmware version (see section 9.3.4.8 in spec)
	unsigned version = ReadRegister(VERSIONREG);
	
	// Pick the appropriate reference values
	const unsigned char *reference;
	switch (version) {
		case 0x88:	// Fudan Semiconductor FM17522 clone
			reference = FM17522_firmware_reference;
			break;
		case 0x90:	// Version 0.0
			reference = MFRC522_firmware_referenceV0_0;
			break;
		case 0x91:	// Version 1.0
			reference = MFRC522_firmware_referenceV1_0;
			break;
		case 0x92:	// Version 2.0
			reference = MFRC522_firmware_referenceV2_0;
			break;
		default:	// Unknown version
			return 0; // abort test
	}
	
	// Test passed; all is good.
	return 1;
} // End PCD_PerformSelfTest()

*/
/*SELF TEST*/
/**************************************************************************/ 




void MFRC522_CRC( char *dataIn, char length, char *dataOut )
{
char i, n;
    MFRC522_Clear_Bit( DIVIRQREG, 0x04 );
    MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );    
    
 //Escreve dados no FIFO        
    for ( i = 0; i < length; i++ )
    {   
        WriteRegister( FIFODATAREG, *dataIn++ );   
    }
    
    WriteRegister( COMMANDREG, PCD_CALCCRC );
        
    i = 0xFF;
    //Espera a finalização do Calculo do CRC
    do 
    {
        n = ReadRegister( DIVIRQREG );
        i--;
    }
    while( i && !(n & 0x04) );        //CRCIrq = 1
        
    dataOut[0] = ReadRegister( CRCRESULTREGL );
    dataOut[1] = ReadRegister( CRCRESULTREGM );        
}




char MFRC522_SelectTag( char *serNum )
{
  char i;
  char _status;
  char size;
  unsigned recvBits;
  char buffer[9];
  
  //MFRC522_Clear_Bit( STATUS2REG, 0x08 );   //MFCrypto1On=0
  
  buffer[0] = PICC_SElECTTAG;
  buffer[1] = 0x70;
  
  for ( i=2; i < 7; i++ )
  {
    buffer[i] = *serNum++;
  }
  
  MFRC522_CRC( buffer, 7, &buffer[7] );             
  
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits );
  if ( (_status == MI_OK) && (recvBits == 0x18) )
  {
    size = buffer[0];
  }
  else
  {
    size = 0;
  }
  return size;
}


//hibernation
void MFRC522_Halt()
{
  unsigned unLen;
  char buff[4];
  
  buff[0] = PICC_HALT;
  buff[1] = 0;
  MFRC522_CRC( buff, 2, &buff[2] );
  MFRC522_Clear_Bit( STATUS2REG, 0x80 );
  MFRC522_ToCard( PCD_TRANSCEIVE, buff, 4, buff, &unLen );
  MFRC522_Clear_Bit( STATUS2REG, 0x08 );
}
char MFRC522_Auth( char authMode, char BlockAddr, char *Sectorkey, char *serNum )
{
  char _status;
  unsigned recvBits;
  char i;
  char buff[12];
  
  //Verify the command block address + sector + password + card serial number
  buff[0] = authMode;
  buff[1] = BlockAddr;
  
  for ( i = 2; i < 8; i++ )
  {
    buff[i] = Sectorkey[i-2];
  }
  
  for ( i = 8; i < 12; i++ )
  {
    buff[i] = serNum[i-8];
  }
  
  _status = MFRC522_ToCard( PCD_AUTHENT, buff, 12, buff, &recvBits );
  
  if ( ( _status != MI_OK ) || !( ReadRegister( STATUS2REG ) & 0x08 ) )
  {
    _status = MI_ERR;
  }
  
  return _status;
}


char MFRC522_Write( char blockAddr, char *writeData )
{
  char _status;
  unsigned recvBits;
  char i;
  char buff[18];
  buff[0] = PICC_WRITE;
  buff[1] = blockAddr;
  
  MFRC522_CRC( buff, 2, &buff[2] );
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, buff, 4, buff, &recvBits );
  if ( (_status != MI_OK) || (recvBits != 4) || ( (buff[0] & 0x0F) != 0x0A) )
  {
    _status = MI_ERR;
  }
  if (_status == MI_OK)
  {
    for ( i = 0; i < 16; i++ )                //Data to the FIFO write 16Byte
    {
      buff[i] = writeData[i];
    }
    
    MFRC522_CRC( buff, 16, &buff[16] );
    _status = MFRC522_ToCard( PCD_TRANSCEIVE, buff, 18, buff, &recvBits );
    if ( (_status != MI_OK) || (recvBits != 4) || ( (buff[0] & 0x0F) != 0x0A ) )
    {
      _status = MI_ERR;
    }
  }
  return _status;
}


char MFRC522_Read( char blockAddr, char *recvData )
{
  char _status;
  unsigned unLen;
  recvData[0] = PICC_READ;
  recvData[1] = blockAddr;
  
  MFRC522_CRC( recvData, 2, &recvData[2] );
  
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, recvData, 4, recvData, &unLen );
  if ( (_status != MI_OK) || (unLen != 0x90) )
  {
    _status = MI_ERR;
  }
  return _status;
}


char MFRC522_AntiColl( char *serNum )
{
  char _status;
  char i;
  char serNumCheck = 0;
  unsigned unLen;
  WriteRegister( BITFRAMINGREG, 0x00 );                //TxLastBists = BitFramingReg[2..0]
  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
  MFRC522_Clear_Bit( STATUS2REG, 0x08 );
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, serNum, 2, serNum, &unLen );
  if (_status == MI_OK)
  {
    for ( i=0; i < 4; i++ )
    {
      serNumCheck ^= serNum[i];
    }
    
    if ( serNumCheck != serNum[4] )
    {
      _status = MI_ERR;
    }
  }
  return _status;
}


//0x0044 = Mifare_UltraLight
//0x0004 = Mifare_One (S50)
//0x0002 = Mifare_One (S70)
//0x0008 = Mifare_Pro (X)
//0x0344 = Mifare_DESFire


char MFRC522_isCard( char *TagType ) 
{
    if (MFRC522_Request( PICC_REQIDL, TagType ) == MI_OK)
        return 1;
    else
        return 0; 
}


char MFRC522_ReadCardSerial( char *str )
{
char _status; 
 _status = MFRC522_AntiColl( str );
 str[5] = 0;
 if (_status == MI_OK)
  return 1;
 else
  return 0;
}
