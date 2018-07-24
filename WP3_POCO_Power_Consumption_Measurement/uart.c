#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "uart.h"
#include "user.h"

//****SETTING BAUD RATE****
char set_baud_rate(const long int baudrate)
{
    unsigned int x;
    x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);                    //SPRG for Low Baud Rate
    if (x>255)                                                       // If high Baud Rate required
      {
         x = (_XTAL_FREQ - baudrate*16)/(baudrate*16);              //SPBRG for High Baud Rate
         TXSTAbits.BRGH = 1;                                        //Setting High Baud Rate
         BAUDCONbits.BRG16 = 1; //16 bit baud rate generator
         SPBRG = x;
      }
     if(x<256)
      {
    SPBRG = x;                                                      //Writing SPBRG Register
    
      }
    return 0;
} 

// ***UART SETTINGS***
void set_uart(void) 
{
    ANSELA = 0x00;
    ANSELB = 0x00;          // clear the ANSEL bit for the TX and RX pins, disable analog function
    ANSELC = 0x00;
    
    //UART lines
    //TRISBbits.TRISB5 = X; // RX pin is input
    //TRISBbits.TRISB7 = X; // TX pin is output
    
     TXSTAbits.SYNC = 0;    //asynchronous mode 
     TXSTAbits.TX9  = 0;    // 8-bit transmission, 9-bit reception is not desired  
     TXSTAbits.TXEN = 1;    // Enable transmitter
     RCSTAbits.RX9 =  0; 	// 8-bit reception 
     RCSTAbits.CREN = 1; 	// enable continous receiving 
     RCSTAbits.SPEN = 1; 	// enable UART
 
    
    set_baud_rate(9600);
    
}

// ***********SENDING DATA THROUGH UART**********
// write byte
void uart_write_byte(unsigned char data)
{
 //PIR1bits.TXIF
    while(!PIR1bits.TXIF);           // wait until transmit shift register is empty
    TXREG = data;                     // write character to TXREG and start transmission
   
}
// write text, several bytes
void uart_write_text(unsigned char *text)
{
  int i,j;
  for(i=0;text[i]!='\0';i++) {
    uart_write_byte(text[i]); // write byte
    j=0;
  }

}


void uart_write_string(const unsigned char *string)
{
	while(*string) 
    {
		uart_write_byte(*string);//send character pointed to by string
        string++;
       
    }
 }



//checking transmit register
char uart_tx_empty()
{
    return TRMT; // if TRMT = '1' TSR is empty, else not
}






// ***********RECEIVING DATA THROUGH UART**********
// Data received or not
// The following function can be used to check whether the data is ready to read from the Receive Register.
// It uses flag bit RCIF which will be set when the data reception is completed
char uart_data_ready()
{
  return RCIF;
}
// Reading a character
// The following function wait till the reception is complete and reads 8 bit data from the Receive Register
unsigned char uart_read_byte()
{
  if(OERR) // If over run error, then reset the receiver
	{
		CREN = 0;
		CREN = 1;
	}
	
	while(!RCIF);  // Wait for transmission to receive
	
	return RCREG;
}
// Reading text
// The following function can be used to read a desired length of text or sequence of characters continously
void uart_read_text(unsigned char *Output, unsigned int length)
{
  for(int i=0;i<length;i++)
  Output[i] = uart_read_byte();
     
}
