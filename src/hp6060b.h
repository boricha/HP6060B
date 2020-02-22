#ifndef HP6060B_H_
#define HP6060B_H_
/*
 * $Id: hp6060b.h 3:33 PM 12/1/2019 ssk  $
 *
 * HP 6060B Display protocol decoding.
 *
 * MIT License
 *
 * Copyright (c) 2019 ssk.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
*/
#define CTRL_PORT       PORTD
#define CTRL_DDR        DDRD
#define CTRL_INPUT      PIND
#define CTRL_PWO        2       // INT0 : Falling Edge
#define CTRL_SYNC       3       // INT1 : Any logical change
/*
* The command was originally 10bits LSB First,
* but since the first 2 bits are always 0 and atmel SPI is 8bit system
* Accordingly 8Bits is used here
* +-----+--------------+------------------+---------------+
* |SYNC#|command field |10bit(originally) |8bit(used hear)|
* +-----+--------------+-----+------------+----------+----+
* |    0|Select LCD    |0x3f0|0b1111110000|0b11111100|0xfc|
* |    1|Unknown       |0x2e0|0b1011100000|0b10111000|0xb8|
* |    2|A Register    |0x028|0b0000101000|0b00001010|0x0a|
* |    3|B Register    |0x068|0b0001101000|0b00011010|0x1a|
* |    4|Annunciator   |0x2f0|0b1011110000|0b10111100|0xbc|
* |    5|C Register    |0x0a8|0b0010101000|0b00101010|0x2a|
* |    6|Display On/Off|0x320|0b1100100000|0b11001000|0xc8|
* +-----+--------------+------------------+---------------+
*
* Remembering that bits are transmitted from least significant bit (LSB) to most significant bit (MSB),
* aka. LSB First
*/
// command field in message frame (order of transmission)
#define MF_START_MF          0xfc  // 0x3F0 start message frame (select the display)
#define MF_UNCHECK_2E0       0xb8  // 0x2E0 undocummented command (we will ignore it)
#define MF_REGISTER_A        0x0a  // 0x028 registers A for the 12 digits
#define MF_REGISTER_B        0x1a  // 0x068 registers B for the 12 digits
#define MF_ANNUNCIATOR       0xbc  // 0x2F0 the annunciators (eg the little arrows that are below the digits).
                                   // The annunciators will follow in the data frame ("INA" line):
                                   // there is one bit per annunciator, and the top right annunciator is transmitted first
#define MF_REGISTER_C        0x2a  // 0x0A8 registers C for the 12 digits
#define MF_DISPLAY_ONOFF     0xc8  // 0x320 toggle the display ON/OFF

// command index in message frame structure
#define MF_IDX_REGISTER_A    0      
#define MF_IDX_REGISTER_B    1      
#define MF_IDX_REGISTER_C    2      
#define MF_IDX_ANNUNCIATOR   3 
//#define MF_IDX_START_MF      4     // an unused command - ignored
//#define MF_IDX_UNCHECK_2E0   5     // an unused command - ignored
//#define MF_IDX_DISPLAY       6     // an unused command - ignored 

// data size in the each command field
#define MF_SZ_REGISTER_A     6
#define MF_SZ_REGISTER_B     6 
#define MF_SZ_REGISTER_C     6
#define MF_SZ_ANNUNCIATOR    2    
//#define MF_SZ_START_MF       1    // an unused - ignored  
//#define MF_SZ_UNCHECK_2E0    1    // an unused - ignored 
//#define MF_SZ_DISPLAY        1    // an unused - ignored 

#define isDataBusActive()   (bit_is_set(CTRL_INPUT, CTRL_PWO))
#define isDataBusIdle()     (bit_is_clear(CTRL_INPUT, CTRL_PWO))
#define isCommand()         (bit_is_set(CTRL_INPUT, CTRL_SYNC))
#define isData()            (bit_is_clear(CTRL_INPUT, CTRL_SYNC))

// Magic numbers
#define MF_SZ_COMMAND        4    // command count per message frame
#define MF_SZ_DATA           6    // max data count per each command
#define MF_MAX_DIGIT         12   
#define MF_DATA_VALID        1    // message frame are received (display ready)
#define MF_DATA_INVALID      0
#define MF_ANNUNCIATOR_CHAR  127  // '▼' Annunciator character code (system_5_5x7.h)
#define MF_PUNCT_NONE        32   // ' ' Punctuation character code (hp6060b_punct.h)
#define MF_PUNCT_COMMA       33   // ',' Punctuation character code (hp6060b_punct.h)
#define MF_PUNCT_DOT         34   // '.' Punctuation character code (hp6060b_punct.h)
#define MF_PUNCT_COLON       35   // ':' Punctuation character code (hp6060b_punct.h)

// message frame structure
typedef struct
{
  uint8_t cmd;
  uint8_t dsz;
  uint8_t data[MF_SZ_DATA];
  uint8_t valid;
} tMessageFrame;
        
#endif