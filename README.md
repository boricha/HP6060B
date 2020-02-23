# HP6060B
HP 6060B 3-60V/0-60A 300W SYSTEM DC ELECTRONIC LOAD  Graphic LCD Bridge
![alt text](https://github.com/boricha/HP6060B/blob/master/tada.jpg)

/*
 * $Id: main.c 3:06 PM 11/28/2019 ssk  $
 *
 * HP 6060B Graphic LCD .
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

## Digial waveform
![alt text](https://github.com/boricha/HP6060B/blob/master/src/singleMF.png)        
## Message Frame Structure(MF)
![alt text](https://github.com/boricha/HP6060B/blob/master/src/MessageFrame.jpg)        
## Schematic
![alt text](https://github.com/boricha/HP6060B/blob/master/schematic/hp6060bv1.5.jpg)
## PCB(top)
![alt text](https://github.com/boricha/HP6060B/blob/master/schematic/top(oshpark).png)
## PCB(bottom)
![alt text](https://github.com/boricha/HP6060B/blob/master/schematic/bottom(oshpark).png)

