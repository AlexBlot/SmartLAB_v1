
#include <xc.h> // include processor files - each processor file is guarded.  
#include "pic16lf1459.h"

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


void I2C_Initialize(void);

// i2c_Wait - wait for I2C transfer to finish
void i2c_Wait(void);

// i2c_Start - Start I2C communication
void i2c_Start(void);

// i2c_Restart - Re-Start I2C communication
void i2c_Restart(void);

// i2c_Stop - Stop I2C communication
void i2c_Stop(void);

// i2c_Write - Sends one byte of data
void i2c_Write(uint8_t data);

// i2c_Write - Sends 2 bytes of data
void i2c_Write16(uint8_t data[]);

// i2c_Address - Sends Slave Address and Read/Write mode
// mode is either I2C_WRITE or I2C_READ
void i2c_Address(uint8_t address, uint8_t mode);

// i2c_Read - Reads a byte from Slave device
uint8_t i2c_Read(unsigned char ack);


void i2c_command(uint8_t address, uint8_t command1, uint8_t command2);

uint8_t i2c_temp_read(uint8_t address);
