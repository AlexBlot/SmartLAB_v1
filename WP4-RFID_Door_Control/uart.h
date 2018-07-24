/* 
 * File:   uart.h
 * Author: Ninoslav Budimir
 *
 * Created on 15 November 2016, 14:46
 */
#define _XTAL_FREQ       16000000L
#define BAUDRATE 9600

unsigned char buffer[100]; //UART RX buffer
uint8_t cc=0;              //RX buffer counter

//UART function prototypes
void set_uart(void);
void uart_write_byte(unsigned char);
unsigned char uart_read_byte();
void uart_write_text( unsigned char*);
void uart_write_string(const unsigned char*);
char uart_tx_empty();
char uart_data_ready();
char set_baud_rate(const long int);
void uart_read_text(unsigned char*, unsigned int);
void interrupt ISR(void);


