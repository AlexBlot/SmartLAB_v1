/* 
 * File:   spi.h
 * Author:  Ninoslav Budimir
 *
 * Created on 07 July 2016, 11:38
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

 
void SPI_init();
uint8_t SPI_transfer(uint8_t data);


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

