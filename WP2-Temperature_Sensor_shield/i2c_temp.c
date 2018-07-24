// Work the best
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#include <xc.h> // include processor files - each processor file is guarded.  
#include "pic16lf1459.h"
#include <stdint.h>


#define I2C_WRITE_COLLISION_STATUS_BIT         SSP1CON1bits.WCOL     // Defines the write collision status bit.
#define I2C_MODE_SELECT_BITS                   SSP1CON1bits.SSPM     // I2C Master Mode control bit.
#define I2C_MASTER_ENABLE_CONTROL_BITS         SSP1CON1bits.SSPEN    // I2C port enable control bit.

#define I2C_START_CONDITION_ENABLE_BIT         SSP1CON2bits.SEN      // I2C START control bit.
#define I2C_REPEAT_START_CONDITION_ENABLE_BIT  SSP1CON2bits.RSEN     // I2C Repeated START control bit.
#define I2C_RECEIVE_ENABLE_BIT                 SSP1CON2bits.RCEN     // I2C Receive enable control bit.
#define I2C_STOP_CONDITION_ENABLE_BIT          SSP1CON2bits.PEN      // I2C STOP control bit.
#define I2C_ACKNOWLEDGE_ENABLE_BIT             SSP1CON2bits.ACKEN    // I2C ACK start control bit.
#define I2C_ACKNOWLEDGE_DATA_BIT               SSP1CON2bits.ACKDT    // I2C ACK data control bit.
#define I2C_ACKNOWLEDGE_STATUS_BIT             SSP1CON2bits.ACKSTAT  // I2C ACK status bit.


#define I2C_SLAVE 0b1001000                    // Slave device I2C address
#define I2C_WRITE 0    
#define I2C_READ 1                  
#define TEMP_REG 0x0          
#define CONFIG_REG 0x1


void I2C_Initialize(void)
{
    TRISBbits.TRISB4 = 1;                    // set SCL and SDA pins as inputs
    TRISBbits.TRISB6 = 1;
    
    SSPBUF = 0;

    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; BF RCinprocess_TXcomplete; SMP High Speed; UA dontupdate; CKE disabled; D_nA lastbyte_address; 
    SSP1STAT = 0x00;
    // SSPEN enabled; WCOL no_collision; CKP Idle:Low, Active:High; SSPM FOSC/4_SSPxADD_I2C; SSPOV no_overflow; 
    SSP1CON1 = 0x28;
    // ACKTIM ackseq; SBCDE disabled; BOEN disabled; SCIE disabled; PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    // SSPADD 4; 
    SSP1ADD = 0x04;
    SSP1CON2 = 0x00;

}

// i2c_Wait - wait for I2C transfer to finish
void i2c_Wait(void){
    while ( ( SSPCON2 & 0x1F ) || ( SSPSTAT & 0x04 ) );
}

// i2c_Start - Start I2C communication
void i2c_Start(void)
{
    i2c_Wait();
    I2C_START_CONDITION_ENABLE_BIT = 1;
        while(I2C_START_CONDITION_ENABLE_BIT);

}

// i2c_Restart - Re-Start I2C communication
void i2c_Restart(void){
    i2c_Wait();
    I2C_REPEAT_START_CONDITION_ENABLE_BIT = 1;
        while(I2C_REPEAT_START_CONDITION_ENABLE_BIT);

}

// i2c_Stop - Stop I2C communication
void i2c_Stop(void)
{
 //   i2c_Wait();
    //while(SSPSTATbits.BF);
    I2C_STOP_CONDITION_ENABLE_BIT = 1;
            while(I2C_STOP_CONDITION_ENABLE_BIT);

}

// i2c_Write - Sends one byte of data
void i2c_Write(uint8_t data)
{
    i2c_Wait();
    SSPBUF = data;
    while(SSPSTATbits.BF);
}

// i2c_Write - Sends 2 bytes of data
void i2c_Write16(uint8_t data[])
{
    i2c_Wait();
    SSPBUF = data[0];
    while(SSPSTATbits.BF);
    i2c_Wait();
    SSPBUF = data[1];
    while(SSPSTATbits.BF);

}

// i2c_Address - Sends Slave Address and Read/Write mode
// mode is either I2C_WRITE or I2C_READ
void i2c_Address(uint8_t address, uint8_t mode)
{
    unsigned char l_address;

    l_address=address<<1;
    l_address+=mode;
    i2c_Wait();
    SSPBUF = l_address;
    while(SSPSTATbits.BF);

}

// i2c_Read - Reads a byte from Slave device
uint8_t i2c_Read(unsigned char ack)
{
    // Read data from slave
    // ack should be 1 if there is going to be more data read
    // ack should be 0 if this is the last byte of data read
    uint8_t i2cReadData;
    
  //  i2c_Wait();
    I2C_RECEIVE_ENABLE_BIT = 1;
    while(I2C_RECEIVE_ENABLE_BIT);
    i2cReadData = SSPBUF;
    while(SSPSTATbits.BF);

  //  i2c_Wait();
    
    if ( ack ) I2C_ACKNOWLEDGE_DATA_BIT =0;	        // Ack
    else       I2C_ACKNOWLEDGE_DATA_BIT =1;	        // NAck
    I2C_ACKNOWLEDGE_ENABLE_BIT =1;                    // send acknowledge sequence

    return( i2cReadData );
}

void i2c_command(uint8_t address, uint8_t command1, uint8_t command2)
{
    uint8_t config[2];
    i2c_Start();                        // send Start
    i2c_Address(I2C_SLAVE, I2C_WRITE);  // Send slave address - write operation
    i2c_Write(address);	                // Set register 
    config[0] = command1;
    config[1] = command2;

    i2c_Write16(config);	                // command
    i2c_Stop();	                        // send Stop
    
}

 //Read a char 
uint8_t i2c_temp_read(uint8_t address)
{
    uint8_t read_byte[2];
    // Read one byte 
    i2c_Start();                                // send Start
    i2c_Address(I2C_SLAVE, I2C_WRITE);          // Send slave address - write operation
    i2c_Write(address);	                        // Set register
    i2c_Restart();                              // Restart
    i2c_Address(I2C_SLAVE, I2C_READ);           // Send slave address - read operation
                                                     // If more than one byte to be read, (0) should be on last byte only 
    read_byte[0] = i2c_Read(1);                     // e.g.3 bytes= i2c_Read(2); i2c_Read(1); i2c_Read(0);
    read_byte[1] = i2c_Read(0);                                                      
                                                    
    i2c_Stop();                                 // send Stop
        
    return *read_byte;                               // return byte. 
                                                 
}


