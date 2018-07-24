/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>             /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>            /* HiTech General Include File */
#endif
#include <stdint.h>             /* For uint8_t definition */
#include <stdbool.h>            /* For true/false definition */
#include <pic16lf1459.h>
#include "system.h"             /* System funct/params, like osc/peripheral config */
#include "user.h"               /* User funct/params, such as InitApp */      
#include "nRF24L01P.h"
#include <math.h>
#include <stdio.h> 
#include <stdlib.h>
#include "i2c_temp.h"
#include "spi.h"


#define I2C_SLAVE 0b1001000                    // 0x48 Slave device I2C address
#define I2C_WRITE 0    
#define I2C_READ 1                  
#define TEMP_REG 0x0          
#define CONFIG_REG 0x1   

int i=0;

void interrupt isr()
{

    /****************************************************************
    ***    Get and Send Temperature during sleep operation     *****
    ****************************************************************/
    ConfigureOscillator();   
    InitApp(); 

    uint8_t read_byte[2];

    if(PIR1bits.TMR1IF)
    {

        PIR1bits.TMR1IF = 0;
        INTCONbits.GIE = 0;
        T1CONbits.TMR1ON = 0;                     // Timer1 OFF
        PIE1bits.TMR1IE = 0;

        i++;
        
        if (i == 6){                            // wait the sixth interruption to wait 60 seconds between each temperature measurement and sending
            
    /******************************************************************************/
    /* Get temperature                                                          */
    /******************************************************************************/

            IRQ = 0;                                // Turn on sensors

            I2C_Initialize();            
            __delay_ms(30);


            // Read one byte 
            i2c_Start();                            // send Start
            i2c_Address(I2C_SLAVE, I2C_WRITE);      // Send slave address - write operation
            i2c_Write(TEMP_REG);	                // Set register
            i2c_Restart();                          // Restart
            i2c_Address(I2C_SLAVE, I2C_READ);       // Send slave address - read operation	       

            read_byte[0] = i2c_Read(1);             // Read two bytes
            read_byte[1] = i2c_Read(0);             // If one byte to be read, (0) should be on byte  	
                                                        // e.g.1 byte=  i2c_Read(0);              
            i2c_Stop();                             // send Stop

            IRQ = 1;                                // Turn off sensors

        /******************************************************************************/
        /* Send the temperature                                                         */
        /******************************************************************************/

//            __delay_ms(200);

            // send message

            CE = 0;
            SPI_init();
            nRF_Setup(); 
            FlushTXRX();
            WriteRegister(NRF_STATUS,0x70);         // Reset status register
            __delay_ms(2);
            CE = 1;
            __delay_us(150);         

            uint8_t data[5] = {0,0,0,0,0};           // four bytes to be compatible to developed USB hub 
            data[0] = 1;
            data[1] = 5;
            data[2] = read_byte[0];
            data[3] = read_byte[1];

            WritePayload(5, data);

            __delay_ms(5);  

            FlushTXRX();

            WriteRegister(NRF_CONFIG, 0x00);  // turn off module 
            i = 0;
                    
        }   

        T1CONbits.TMR1ON = 1;                     // Timer1 ON
        PIR1bits.TMR1IF = 0;
        PIE1bits.TMR1IE = 1;

        INTCONbits.GIE = 1;
        TMR1H = 0x5F;                            // period of 10 sec
        TMR1L = 0xE0;


        ConfigureOscillator_interrupt();

    }

}

void main(void)
{
    ConfigureOscillator_interrupt();
    InitApp(); 
    timer_setup_and_start_IE();
   
 
    while(1){  

        SLEEP();
        
     }        
   
}


