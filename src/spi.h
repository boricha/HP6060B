#ifndef _SPI_H_
#define _SPI_H_
/*
 * Copyright (c) 2009 Andrew Smallbone <andrew@rocketnumbernine.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
/**
 \Revision History
  Note. The Revision History showns the most amenddment first

  07:16 PM 2019-06-04 AM Add __AVR_ATmega168P__ 
*/
 

#include <avr/io.h>

#define SPI_PORT           PORTB
#define SPI_DDR            DDRB

// create alias for the different SPI chip pins - code assumes all on port B
#if(defined(__AVR_ATmega8__)            || \
    defined(__AVR_ATmega48__)           || \
    defined(__AVR_ATmega88__)           || \
    defined(__AVR_ATmega168__)          || \
    defined(__AVR_ATmega168P__)         || \
    defined(__AVR_ATmega328__)          || \
    defined(__AVR_ATmega328P__))        
 #define SPI_SS_PIN        2            
 #define SPI_MOSI_PIN      3            
 #define SPI_MISO_PIN      4            
 #define SPI_SCK_PIN       5            
#elif(defined(__AVR_ATmega164P__)       || \
      defined(__AVR_ATmega324P__)       || \
      defined(__AVR_ATmega644P__)       || \
      defined(__AVR_ATmega32__))   
 #define SPI_SS_PIN        4 
 #define SPI_MOSI_PIN      5 
 #define SPI_MISO_PIN      6 
 #define SPI_SCK_PIN       7 
#elif(defined(__AVR_AT90USB82__)        || \
      defined(__AVR_AT90USB162__))
 #define SPI_SS_PIN        0
 #define SPI_SCK_PIN       1
 #define SPI_MOSI_PIN      2
 #define SPI_MISO_PIN      3 
#else
 #error unknown processor - add to spi.h
#endif

#define spi_start()       (SPI_PORT &= ~_BV(SPI_SS_PIN))
#define spi_stop()        (SPI_PORT |=  _BV(SPI_SS_PIN))

// SPI clock modes
#define SPI_MODE_0         0x00 // Sample(read) (Rising)  Setup (Falling) CPOL=0, CPHA=0
#define SPI_MODE_1         0x01 // Sample(read) (Falling) Setup (Rising)  CPOL=0, CPHA=1
#define SPI_MODE_2         0x02 // Sample(read) (Falling) Setup (Rising)  CPOL=1, CPHA=0
#define SPI_MODE_3         0x03 // Sample(read) (Rising)  Setup (Falling) CPOL=1, CPHA=1

/*
*  Synchronization by clock
*  CPOL (clock polarity)
*  CPHA (clock phase)
*
*  At CPOL= 0 the base value of the clock is zero, while at CPOL = 1 the base value of the clock is one
*  CPHA = 0 means sample (read) on the first clock edge, while CPHA = 1 means sample on the second clock
*/

// data direction
#define SPI_LSB            1    // send least significant bit (bit 0) first, the LSB of the data word is transmitted first.
#define SPI_MSB            0    // send most  significant bit (bit 7) first, the MSB of the data word is transmitted first.

// whether to raise interrupt when data received (SPIF bit received)
#define SPI_NO_INTERRUPT   0
#define SPI_INTERRUPT      1

// slave or master with clock diviser
#define SPI_SLAVE          0xF0
#define SPI_MSTR_CLK4      0x00 // fosc/4   SPI is only guaranteed to work at fosc/4 or lower 
#define SPI_MSTR_CLK16     0x01 // fosc/16 
#define SPI_MSTR_CLK64     0x02 // fosc/64 
#define SPI_MSTR_CLK128    0x03 // fosc/128

// SPI2X: Double SPI Speed Bit('1':Enable) - (have no effect on the Slave)
#define SPI_MSTR_CLK2      0x04 // chip Fosc/2     
#define SPI_MSTR_CLK8      0x05 // chip Fosc/8 
#define SPI_MSTR_CLK32     0x06 // chip Fosc/32

#define spi_enable()       SPCR |=  _BV(SPE)
#define spi_disable()      SPCR &= ~_BV(SPE)

// setup spi
extern void spi_init(uint8_t mode,     // timing mode SPI_MODE[0-4]
	                 uint8_t dord,     // data direction SPI_LSB|SPI_MSB
	                 uint8_t interrupt,// whether to raise interrupt on recieve
	                 uint8_t clock);   // clock diviser

// Send and Receive a byte of data (Master Mode)
// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
extern uint8_t spi_transfer(uint8_t data);
#endif
