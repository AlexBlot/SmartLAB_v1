#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define GREENLED    LATCbits.LATC4
#define REDLED      LATCbits.LATC5

/*
#define     MAX_LEN               16        // Maximum length of an array. CHECK IF IT IS MAXIMUM LENGTH/SIZE OF FIFO!!!!!!!!!!!

//MF522 MFRC522 error codes.
#define     MI_OK                 0         // Everything A-OK.
#define     MI_NOTAGERR           1         // No tag error
#define     MI_ERR                2         // General error

//MF522 Command word
#define     MFRC522_IDLE          0x00      // NO action; Cancel the current command
#define     MFRC522_MEM           0x01      // Store 25 unsigned char into the internal buffer.
#define     MFRC522_GENID         0x02      // Generates a 10 unsigned char random ID number.
#define     MFRC522_CALCCRC       0x03      // CRC Calculate or selftest.
#define     MFRC522_TRANSMIT      0x04      // Transmit data
#define     MFRC522_NOCMDCH       0x07      // No command change.
#define     MFRC522_RECEIVE       0x08      // Receive Data
#define     MFRC522_TRANSCEIVE    0x0C      // Transmit and receive data,
#define     MFRC522_AUTHENT       0x0E      // Authentication Key
#define     MFRC522_SOFTRESET     0x0F      // Reset

//Mifare_One tag command word
#define     MF1_REQIDL            0x26      // find the antenna area does not enter hibernation
#define     MF1_REQALL            0x52      // find all the tags antenna area
#define     MF1_ANTICOLL          0x93      // anti-collision
#define     MF1_SELECTTAG         0x93      // election tag
#define     MF1_AUTHENT1A         0x60      // authentication key A
#define     MF1_AUTHENT1B         0x61      // authentication key B
#define     MF1_READ              0x30      // Read Block
#define     MF1_WRITE             0xA0      // write block
#define     MF1_DECREMENT         0xC0      // debit
#define     MF1_INCREMENT         0xC1      // recharge
#define     MF1_RESTORE           0xC2      // transfer block data to the buffer
#define     MF1_TRANSFER          0xB0      // save the data in the buffer
#define     MF1_HALT              0x50      // Sleep


//------------------ MFRC522 registers---------------
//Page 0:Command and Status
#define     Reserved00            0x00
#define     CommandReg            0x01
#define     CommIEnReg            0x02
#define     DivIEnReg             0x03
#define     CommIrqReg            0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG
#define     Reserved20            0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg              0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     Reserved31            0x3C
#define     Reserved32            0x3D
#define     Reserved33            0x3E
#define     Reserved34            0x3F
//-----------------------------------------------
*/
/*
CARACTERISTIQUES
Tension d'Alim : 3.3V
Suporte ISO / IEC 14443A/MIFARE
Distance maxi de fonctionnement  :40 milímetres
Interface SPI (Hardware ou Software)
Buffer FIFO de 64 bytes
Consommation  13 ~ 26mA
Temperature : -25 ~ +85
*/

//0x0044 = Mifare_UltraLight
//0x0004 = Mifare_One (S50)
//0x0002 = Mifare_One (S70)
//0x0008 = Mifare_Pro (X)
//0x0344 = Mifare_DESFire

// Firmware data for self-test
// Reference values based on firmware version
// Hint: if needed, you can remove unused self-test data to save flash memory
//
// Version 0.0 (0x90)
// Philips Semiconductors; Preliminary Specification Revision 2.0 - 01 August 2005; 16.1 self-test
const unsigned char MFRC522_firmware_referenceV0_0[]  = {
	0x00, 0x87, 0x98, 0x0f, 0x49, 0xFF, 0x07, 0x19,
	0xBF, 0x22, 0x30, 0x49, 0x59, 0x63, 0xAD, 0xCA,
	0x7F, 0xE3, 0x4E, 0x03, 0x5C, 0x4E, 0x49, 0x50,
	0x47, 0x9A, 0x37, 0x61, 0xE7, 0xE2, 0xC6, 0x2E,
	0x75, 0x5A, 0xED, 0x04, 0x3D, 0x02, 0x4B, 0x78,
	0x32, 0xFF, 0x58, 0x3B, 0x7C, 0xE9, 0x00, 0x94,
	0xB4, 0x4A, 0x59, 0x5B, 0xFD, 0xC9, 0x29, 0xDF,
	0x35, 0x96, 0x98, 0x9E, 0x4F, 0x30, 0x32, 0x8D
};
// Version 1.0 (0x91)
// NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
const unsigned char MFRC522_firmware_referenceV1_0[]  = {
	0x00, 0xC6, 0x37, 0xD5, 0x32, 0xB7, 0x57, 0x5C,
	0xC2, 0xD8, 0x7C, 0x4D, 0xD9, 0x70, 0xC7, 0x73,
	0x10, 0xE6, 0xD2, 0xAA, 0x5E, 0xA1, 0x3E, 0x5A,
	0x14, 0xAF, 0x30, 0x61, 0xC9, 0x70, 0xDB, 0x2E,
	0x64, 0x22, 0x72, 0xB5, 0xBD, 0x65, 0xF4, 0xEC,
	0x22, 0xBC, 0xD3, 0x72, 0x35, 0xCD, 0xAA, 0x41,
	0x1F, 0xA7, 0xF3, 0x53, 0x14, 0xDE, 0x7E, 0x02,
	0xD9, 0x0F, 0xB5, 0x5E, 0x25, 0x1D, 0x29, 0x79
};
// Version 2.0 (0x92)
// NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
const unsigned char MFRC522_firmware_referenceV2_0[]  = {
	0x00, 0xEB, 0x66, 0xBA, 0x57, 0xBF, 0x23, 0x95,
	0xD0, 0xE3, 0x0D, 0x3D, 0x27, 0x89, 0x5C, 0xDE,
	0x9D, 0x3B, 0xA7, 0x00, 0x21, 0x5B, 0x89, 0x82,
	0x51, 0x3A, 0xEB, 0x02, 0x0C, 0xA5, 0x00, 0x49,
	0x7C, 0x84, 0x4D, 0xB3, 0xCC, 0xD2, 0x1B, 0x81,
	0x5D, 0x48, 0x76, 0xD5, 0x71, 0x61, 0x21, 0xA9,
	0x86, 0x96, 0x83, 0x38, 0xCF, 0x9D, 0x5B, 0x6D,
	0xDC, 0x15, 0xBA, 0x3E, 0x7D, 0x95, 0x3B, 0x2F
};
// Clone
// Fudan Semiconductor FM17522 (0x88)
const unsigned char FM17522_firmware_reference[]  = {
	0x00, 0xD6, 0x78, 0x8C, 0xE2, 0xAA, 0x0C, 0x18,
	0x2A, 0xB8, 0x7A, 0x7F, 0xD3, 0x6A, 0xCF, 0x0B,
	0xB1, 0x37, 0x63, 0x4B, 0x69, 0xAE, 0x91, 0xC7,
	0xC3, 0x97, 0xAE, 0x77, 0xF4, 0x37, 0xD7, 0x9B,
	0x7C, 0xF5, 0x3C, 0x11, 0x8F, 0x15, 0xC3, 0xD7,
	0xC1, 0x5B, 0x00, 0x2A, 0xD0, 0x75, 0xDE, 0x9E,
	0x51, 0x64, 0xAB, 0x3E, 0xE9, 0x15, 0xB5, 0xAB,
	0x56, 0x9A, 0x98, 0x82, 0x26, 0xEA, 0x2A, 0x62
};



//MF522 Command word
#define PCD_IDLE              0x00               //NO action; Cancel the current command
#define PCD_MEM               0x01      // Store 25 unsigned char into the internal buffer.
#define PCD_AUTHENT           0x0E               //Authentication Key
#define PCD_RECEIVE           0x08               //Receive Data
#define PCD_TRANSMIT          0x04               //Transmit data
#define PCD_TRANSCEIVE        0x0C               //Transmit and receive data,
#define PCD_RESETPHASE        0x0F               //Reset
#define PCD_CALCCRC           0x03               //CRC Calculate
// Mifare_One card command word
#define PICC_REQIDL          0x26               // find the antenna area does not enter hibernation
#define PICC_REQALL          0x52               // find all the cards antenna area
#define PICC_ANTICOLL        0x93               // anti-collision
#define PICC_SElECTTAG       0x93               // election card
#define PICC_AUTHENT1A       0x60               // authentication key A
#define PICC_AUTHENT1B       0x61               // authentication key B
#define PICC_READ            0x30               // Read Block
#define PICC_WRITE           0xA0               // write block
#define PICC_DECREMENT       0xC0               // debit
#define PICC_INCREMENT       0xC1               // recharge
#define PICC_RESTORE         0xC2               // transfer block data to the buffer
#define PICC_TRANSFER        0xB0               // save the data in the buffer
#define PICC_HALT            0x50               // Sleep


//And MF522 The error code is returned when communication
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2


//------------------MFRC522 Register---------------
//Page 0:Command and Status
#define     RESERVED00            0x00    // reserved for future use
#define     COMMANDREG            0x01    // starts and stops command execution
#define     COMMIENREG            0x02    // enable and disable interrupt request control bits
#define     DIVLENREG             0x03    // enable and disable interrupt request control bits
#define     COMMIRQREG            0x04    // interrupt request bits
#define     DIVIRQREG             0x05    // interrupt request bits
#define     ERRORREG              0x06    // error bits showing the error status of the last command executed 
#define     STATUS1REG            0x07    // communication status bits
#define     STATUS2REG            0x08    // receiver and transmitter status bits
#define     FIFODATAREG           0x09    // input and output of 64 byte FIFO buffer
#define     FIFOLEVELREG          0x0A    // number of bytes stored in the FIFO buffer    
#define     WATERLEVELREG         0x0B    // level for FIFO underflow and overflow warning
#define     CONTROLREG            0x0C    // miscellaneous control registers
#define     BITFRAMINGREG         0x0D    // adjustments for bit-oriented frames
#define     COLLREG               0x0E    // bit position of the first bit-collision detected on the RF interface
#define     RESERVED01            0x0F    // reserved for future use
//PAGE 1:Command     
#define     RESERVED10            0x10    // reserved for future use
#define     MODEREG               0x11    // defines general modes for transmitting and receiving
#define     TXMODEREG             0x12    // defines transmission data rate and framing
#define     RXMODEREG             0x13    // defines reception data rate and framing
#define     TXCONTROLREG          0x14    // controls the logical behavior of the antenna driver pins TX1 and TX2
#define     TXAUTOREG             0x15    // controls the setting of the transmission modulation
#define     TXSELREG              0x16    // selects the internal sources for the antenna driver
#define     RXSELREG              0x17    // selects internal receiver settings
#define     RXTHRESHOLDREG        0x18    // selects thresholds for the bit decoder
#define     DEMODREG              0x19    // defines demodulator settings
#define     RESERVED11            0x1A    // reserved for future use
#define     RESERVED12            0x1B    // reserved for future use
#define     MFTXREG               0x1C    // controls some MIFARE communication transmit parameters
#define     MFRXREG               0x1D    // controls some MIFARE communication receive parameters
#define     RESERVED14            0x1E    // reserved for future use
#define     SERIALSPEEDREG        0x1F    // selects the speed of the serial UART interface
//PAGE 2:CFG    
#define     RESERVED20            0x20    // reserved for future use  
#define     CRCRESULTREGM         0x21    // shows the MSB and LSB values of the CRC calculation
#define     CRCRESULTREGL         0x22    // shows the MSB and LSB values of the CRC calculation
#define     RESERVED21            0x23    // reserved for future use
#define     MODWIDTHREG           0x24    // controls the ModWidth setting
#define     RESERVED22            0x25    // reserved for future use
#define     RFCFGREG              0x26    // configures the receiver gain
#define     GSNREG                0x27    // selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define     CWGSPREG              0x28    // defines the conductance of the p-driver output during periods of no modulation
#define     MODGSPREG             0x29    // defines the conductance of the p-driver output during periods of modulation
#define     TMODEREG              0x2A    // defines settings for the internal timer
#define     TPRESCALERREG         0x2B    // defines settings for the internal timer
#define     TRELOADREGH           0x2C    // defines the 16-bit timer reload value
#define     TRELOADREGL           0x2D    // defines the 16-bit timer reload value
#define     TCOUNTERVALUEREGH     0x2E    // shows the 16-bit timer value
#define     TCOUNTERVALUEREGL     0x2F    // shows the 16-bit timer value
//PAGE 3:TEST REGISTER     
#define     RESERVED30            0x30    // reserved for future use
#define     TESTSEL1REG           0x31    // general test signal configuration
#define     TESTSEL2REG           0x32    // general test signal configuration and PRBS control
#define     TESTPINENREG          0x33    // enables pin output driver on pins D1 to D7
#define     TESTPINVALUEREG       0x34    // defines the values for D1 to D7 when it is used as an I/O bus
#define     TESTBUSREG            0x35    // shows the status of the internal test bus
#define     AUTOTESTREG           0x36    // controls the digital self test
#define     VERSIONREG            0x37    // shows the software version
#define     ANALOGTESTREG         0x38    // controls the pins AUX1 and AUX2
#define     TESTDAC1REG           0x39    // defines the test value for TestDAC1
#define     TESTDAC2REG           0x3A    // defines the test value for TestDAC2
#define     TESTADCREG            0x3B    // shows the value of ADC I and Q channels
#define     RESERVED31            0x3C    // reserved for production tests
#define     RESERVED32            0x3D    // reserved for production tests
#define     RESERVED33            0x3E    // reserved for production tests
#define     RESERVED34            0x3F    // reserved for production tests
// *************************************************************


void MFRC522_Clear_Bit( char addr, char mask );
void MFRC522_Set_Bit( char addr, char mask );
void MFRC522_Reset(void);
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
void MFRC522_Init(void);
char MFRC522_ToCard( char command, char *sendData, char sendLen, char *backData, unsigned *backLen );
char MFRC522_Request( char reqMode, char *TagType );
void MFRC522_CRC( char *dataIn, char length, char *dataOut );
char MFRC522_SelectTag( char *serNum );
void MFRC522_Halt(void);
char MFRC522_Auth( char authMode, char BlockAddr, char *Sectorkey, char *serNum );
char MFRC522_Write( char blockAddr, char *writeData );
char MFRC522_Read( char blockAddr, char *recvData );
char MFRC522_AntiColl( char *serNum );
char MFRC522_isCard( char *TagType );
char MFRC522_ReadCardSerial( char *str );
unsigned char  MFRC_getVersion(void);


void WriteRegister(uint8_t reg, uint8_t val);
//void WriteAddress(uint8_t reg, uint8_t num, uint8_t* addr);
uint8_t ReadRegister(uint8_t reg);
//void WriteCommand(uint8_t command);
//void WritePayload(uint8_t num, uint8_t* data);


/*
uchar serNum[5];
uchar  writeDate[16] ={'T', 'e', 'n', 'g', ' ', 'B', 'o', 0, 0, 0, 0, 0, 0, 0, 0,0};

uchar sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                            };
 uchar sectorNewKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                               };*/



