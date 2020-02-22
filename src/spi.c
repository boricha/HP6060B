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
 *
 * Originally Adapted from https://code.google.com/p/rocketnumbernine/source/browse/trunk/AVR/spi/?r=2
 * 
 * Serial Peripheral Interface - SPI Basics
 *  http://maxembedded.com/2013/11/serial-peripheral-interface-spi-basics/#BusInterface
 *
 */
 
/**
 \Revision History
  Note. The Revision History showns the most amenddment first
  5:42 PM 11/17/2019  bug fix Atmega32 /SS wrong pin no . Fix
  
  10:21 PM 11/13/2019 add 2 macro  
                      #define spi_enable()       SPCR |=  _BV(SPE)
                      #define spi_disable()      SPCR &= ~_BV(SPE)
  2019-03-19  3:42:35 PM Merged spi_read(),spi_write() into spi_transfer();
                         Remove disable_spi()
  2017-04-06 12:05:03 AM Add __AVR_ATmega32__  redefined Items comment
  2015-03-28 08:58:16 PM add __AVR_ATmega324P__ Resistor Set
  2014-12-27 11:45:54 AM Add MCU, Comment
  2014-12-26 11:23:43 AM Initial creation (based on SAMSUNG  UC164904 4x16 SPI)
*/
#include "spi.h"
/*
  spi_init(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK4);
 */
void spi_init(uint8_t mode, uint8_t dord, uint8_t interrupt, uint8_t clock)
{
  // specify pin directions for SPI pins on port B
  if (clock == SPI_SLAVE)
  {
    // if slave SS and SCK is input
    SPI_DDR  |=   _BV(SPI_MISO_PIN);   // output
    SPI_DDR  &=  ~_BV(SPI_MOSI_PIN);   // input
    SPI_DDR  &=  ~_BV(SPI_SCK_PIN );   // input
    SPI_DDR  &=  ~_BV(SPI_SS_PIN  );   // input
    
    //SPI_PORT |=   _BV(SPI_MISO_PIN);
    //SPI_PORT &= ~(_BV(SPI_MOSI_PIN) | _BV(SPI_SCK_PIN ) | _BV(SPI_SS_PIN));
  }
  else  // Serial I/F LCD, Flash Memory etc,..
  {
    SPI_DDR  &=  ~_BV(SPI_MISO_PIN);   // input     SID
    SPI_DDR  |=   _BV(SPI_MOSI_PIN);   // output    
    SPI_DDR  |=   _BV(SPI_SCK_PIN );   // output    SCLK/SCK
    SPI_DDR  |=   _BV(SPI_SS_PIN  );   // output    CS/SS/User Define
    
    SPI_PORT |=   _BV(SPI_MOSI_PIN) | _BV(SPI_SCK_PIN ) | _BV(SPI_SS_PIN);
    SPI_PORT &=  ~_BV(SPI_MISO_PIN);
  }

  SPCR = (( interrupt ? 1 : 0)         << SPIE)  // Bit7, SPI Interrupt Enable('1':Enable)
       | (  1                          << SPE )  // Bit6, SPI Enable('1':Enable) -  This bit must be set to enable any SPI operations.
       | (  dord                       << DORD)  // Bit5, Data Order('1':LSB, '0':MSB)
       | (((clock != SPI_SLAVE) ? 1:0) << MSTR)  // Bit4, Master/Slave Select('1':Master, '0':Slave)
       | (((mode  & 0x02)  == 2)       << CPOL)  // Bit3, Clock Polarity('1':SCK is high when idle, '0':SCK is low when idle)
       | (((mode  & 0x01))             << CPHA)  // Bit2, Clock Phase
       | (((clock & 0x02)  == 2)       << SPR1)  // Bit1, SPI Clock Rate Select 1(have no effect on the Slave)
       | (( clock & 0x01)              << SPR0); // Bit0, SPI Clock Rate Select 0(have no effect on the Slave)
                                                   /* Relationship Between SCK and the Oscillator Frequency
                                                      -----+----+----+-------------
                                                      SPI2X|SPR1|SPR0|SCK Frequency
                                                      -----+----+----+------------- 
                                                          0|   0|   0|    fosc/4     SPI is only guaranteed to work at fosc/4 or lower (Slave)
                                                          0|   0|   1|    fosc/16
                                                          0|   1|   0|    fosc/64
                                                          0|   1|   1|    fosc/128
                                                      -----+----+----+------------                                                             
                                                          1|   0|   0|    fosc/2     SPI2X: Double SPI Speed - have no effect on the Slave
                                                          1|   0|   1|    fosc/8
                                                          1|   1|   0|    fosc/32
                                                          1|   1|   1|    fosc/64
                                                      -----+----+----+-------------*/
  SPSR = (((clock & 0x04)  == 4)       << SPI2X);   // Bit0, SPI2X: Double SPI Speed Bit('1':Enable) - (have no effect on the Slave)
                                                    // SPI speed (SCK Frequency) will be doubled when the SPI is in Master mode.
}

uint8_t spi_transfer(uint8_t data)
{
  SPDR = data;
  //loop_until_bit_is_set(SPSR, SPIF);    
  while (!(SPSR & _BV(SPIF)));   
  
  return SPDR;
  
//SPDR = data;
//while (!(SPSR & _BV(SPIF))) ; // wait
//return SPDR;
}
/*
 * EOF
 */
