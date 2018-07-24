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
#include "rc522.h"
#include "spi.h"
#include "nRF24L01P.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

// CONFIG1
#pragma config FOSC = INTOSC        // Oscillator Selection Bits (ECH, External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pins)
#pragma config WDTE = OFF           // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF          // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON           // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF             // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF          // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF       // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF           // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF            // Flash Memory Self-Write Protection (Write protection off)
#pragma config CPUDIV = NOCLKDIV    // CPU System Clock Selection Bit (CPU system clock divided by 6)
#pragma config USBLSCLK = 48MHz     // USB Low SPeed Clock Selection bit (System clock expects 48 MHz, FS/LS USB CLKENs divide-by is set to 8.)
#pragma config PLLMULT = 3x         // PLL Multipler Selection Bit (3x Output Frequency Selected)
#pragma config PLLEN = DISABLED     // PLL Enable Bit (3x or 4x PLL Enabled)
#pragma config STVREN = OFF         // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO            // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF          // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF            // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

char key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char writeData[] = "smartLAB";

/******************************************************************************/
/* Authorized tags Declaration                                           */
/******************************************************************************/
char GTSN1[4] = {0x7B, 0xB3, 0x52, 0xC3}; // Blue tag
char GTSN2[4] = {0x0C, 0x09, 0x52, 0xA3}; // Blue & Black tag
char GTSN3[4] = {0x70, 0xEB, 0xB7, 0x2B}; // White tag
char GTSN4[4] = {0x70, 0x15, 0xA3, 0x2B}; // White & Black tag





void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    
    //INTCONbits.GIE = 0;
    //INTCONbits.PEIE = 0;
    InitApp();
    /* Initialize I/O and Peripherals for application */  
    SPI_init();   
    //WriteRegister(NRF_CONFIG, 0x00); // turn off module  
    //CSN = 0;
    
    //Initialisation of the RFID module
    MFRC522_Init();
      
    
   TRISCbits.TRISC1 = 0; // IPCSCLK, Output, Pin locker
    
   
    
    unsigned version = ReadRegister(VERSIONREG);
    
    unsigned char TagType;


    char size;
    char i;
    char msg[12];
    char UID[6];
    SW = 3;
    
    INTCONbits.IOCIE = 1;               // Enable IOC Interrupts 
    INTCONbits.IOCIF = 0;  

    
    //pound = PerformSelfTest();
    
    while(1)
        
    {
      
        PORTCbits.RC1 = 0;
   
        /******************************************************************************/
        /*                Status of the door                                          */
        /******************************************************************************/
        
        if ((SW == 1) || (SW == 2) || (SW == 3) || (SW == 4))
        {       
            INTCONbits.IOCIE = 0;    
            // send message
            CE = 0;
            SPI_init();
            nRF_Setup_Write(); 
            FlushTXRX();
            WriteRegisterRF(NRF_STATUS,0x70);         // Reset status register
            __delay_ms(2);
            CE = 1;
            __delay_us(150);         
                
            uint8_t data0[5] = {0,1,0,3,13};           // four bytes to be compatible to developed USB hub (3 - arbitrarly, 13 - terminator)
            data0[0] = 3;
            data0[2] = SW-1;
            WritePayload(5, data0); 
            __delay_ms(5);  
            FlushTXRX();
            WriteRegister(NRF_CONFIG, 0x00);        // turn off module
            __delay_ms(5);                            
            
            SW = 0;
            
            GREENLED = 1; // turn ON red LED   
            __delay_ms(50);
            GREENLED = 0; // turn OFF red LED  
            REDLED = 1; // turn ON green LED  
            __delay_ms(50);
            REDLED = 0; // turn OFF green LED 
            
            INTCONbits.IOCIF = 0; 
            IOCAFbits.IOCAF0 = 0;
            INTCONbits.IOCIF = 0;
            INTCONbits.IOCIE = 1;  
            
          }  
         /******************************************************************************/
        /*                                RFID                                         */
        /******************************************************************************/   
     
        if (MFRC522_isCard(&TagType))       
        {       
         
        if (MFRC522_ReadCardSerial (&UID))             
         {
            size = MFRC522_SelectTag( &UID );  
            

                /******************************************************************************/
                /* RADIOCOMMUNICATION                                         */
                /******************************************************************************/
                //SPI_init();
               // WriteRegister(NRF_CONFIG, 0x00); // turn off module    
                //InitApp();  
                
                
                /* SEND MESSAGE */
                CE = 0;
                SPI_init();
                nRF_Setup_Write(); 
                FlushTXRX();
                WriteRegisterRF(NRF_STATUS,0x70);         // Reset status register
                __delay_ms(2);
                CE = 1;
                __delay_us(150);         

                uint8_t data[5] = {0,0,0,0,0};           // four bytes to be compatible to developed USB hub (3 - arbitrarly, 13 - terminator)
                data[0] = 30;
                data[1] = UID[0];
                data[2] = UID[1];
                data[3] = UID[2];
                data[4] = UID[3];

                
                WritePayload(5, data); 
                __delay_ms(5); 
                //__delay_us(200);  
                FlushTXRX();
                WriteRegisterRF(NRF_CONFIG, 0x00);        // turn off module
                __delay_ms(5); 

                GREENLED = 1; // turn ON red LED   
                
                
                 /*RECEIVE MESSAGE*/
                CE = 0;
                SPI_init();
                nRF_Setup_Read(); 
                __delay_ms(2);
                CE = 1;
                __delay_us(170); 
                

                i = 2000;
                do 
                {
                   

                            __delay_ms(50);
                            uint8_t data2[5] = {0,0,0,0,0};

                                if (nRF_available()) 
                                {    
                                    ReadPayload(5, data2);  
                                    FlushTXRX();                    

                                        if (data2[0]==70) // Identify the HUB
                                        { 
                                            if (data2[1]==0x1) // Identify the HUB
                                            { 
                                                /*ACCESS GRANTED*/
                                                GREENLED = 1; // turn ON red LED   
                                                __delay_ms(50);
                                                GREENLED = 0; // turn OFF red LED  
                                                REDLED = 1; // turn ON green LED  
                                                __delay_ms(50);
                                                REDLED = 0; // turn OFF green LED 
                                                /******************************************************************************/
                                                /* Open the locker                                           */
                                                /******************************************************************************/
                                                PORTCbits.RC1 = 1;
                                                __delay_ms(1000);
                                                PORTCbits.RC1 = 0;
                                                break;
                                            }
                                            else if (data2[1]==0x0) // Identify the HUB
                                            { 
                                                /*ACCESS DENIED*/
                                                REDLED = 1; // turn ON green LED  
                                                __delay_ms(100);
                                                REDLED = 0; // turn OFF green LED 
                                                break;
                                            }
                                        }

                                    }
                    i--;
                }
                while(i!=0); 
                
                /*RECEIVE MESSAGE*/
                
                GREENLED = 0;

                /*RADIOCOMMUNICATION*/
  
                    
             
            }
                            
        
        
         MFRC522_Halt () ;
      }


   }

}