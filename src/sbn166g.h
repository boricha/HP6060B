#ifndef SBN166G_H_
#define SBN166G_H_
/*
 * $Id: sbn166g_i2c.h 01:02 PM 2019-10-18 ssk  $
 *
 * Winstar WG20232A-TMI-V#A(202x32) LCD I2C(PCA9555) interface library.
 * Designed for AVANT SBN166G Dot-matrix STN LCD Driver with 32-row x 80-column
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
#define LCD_RST_PORT         PORTD
#define LCD_RST_DDR          DDRD
#define LCD_RST_PIN          1    // THE HARDWARE RESET IS EDGE-SENSITIVE. IT IS NOT LEVEL-SENSITIVE.

#define LCD_CONTROL_PORT     PORTC
#define LCD_CONTROL_DDR      DDRC
#define LCD_A0_PIN           4    // Instruction/Data select, H:D0-D7 are display data L:D0-D7 are control data
#define LCD_RW_PIN           5    // Read/Write control signal. L:Active (H:Read, L:Write)

#define LCD_CHIP1_PORT       PORTD
#define LCD_CHIP1_DDR        DDRD
#define LCD_CS1_PIN          0    // Enable signal (E) for the 68-type microcontroller

#define LCD_CHIP2_PORT       PORTB
#define LCD_CHIP2_DDR        DDRB
#define LCD_CS2_PIN          0    // Enable signal (E) for the 68-type microcontroller

#define LCD_CHIP3_PORT       PORTB
#define LCD_CHIP3_DDR        DDRB
#define LCD_CS3_PIN          1    // Enable signal (E) for the 68-type microcontroller

#define LCD_DATA_L_PORT      PORTC
#define LCD_DATA_L_DDR       DDRC // [0..3]
#define LCD_DATA_L_INPUT     PINC

#define LCD_DATA_H_PORT      PORTD 
#define LCD_DATA_H_DDR       DDRD // [4..7]
#define LCD_DATA_H_INPUT     PIND

#define LCD_CHIP1_START_X    0    // the CHIP1 start pixel of column
#define LCD_CHIP2_START_X    61   // the CHIP2 start pixel of column
#define LCD_CHIP3_START_X    141  // the CHIP3 start pixel of column
#define LCD_CHIP1_MAX_COL    61   // the CHIP1 max column of pixel
#define LCD_CHIP2_MAX_COL    140  // the CHIP2 max column of pixel
#define LCD_CHIP3_MAX_COL    201  // the CHIP3 max column of pixel

// command function equates for SBN166G LCD Display Controller
#define LCD_DISP_OFF         0xAE // turn LCD panel OFF
#define LCD_DISP_ON          0xAF // turn LCD panel ON
#define LCD_START_LINE       0xC0 // Specifies RAM line corresponding to top line of display.
#define LCD_SET_PAGE         0xB8 // set page address
#define LCD_SET_COL          0x00 // set column address
#define LCD_SET_ADC_NOR      0xA0 // Column/Segment Mapping Register,  set for normal direction
#define LCD_SET_ADC_REV      0xA1 // Column/Segment Mapping Register   set for reverse direction
#define LCD_STATIC_OFF       0xA4 // normal drive
#define LCD_STATIC_ON        0xA5 // static drive (power save)
#define LCD_DUTY_16          0xA8 // driving duty 1/16
#define LCD_DUTY_32          0xA9 // driving duty 1/32
#define LCD_SET_RMW_START    0xE0 // Read-Modify-Write
#define LCD_SET_RMW_END      0xEE // release the Read-Modify-Write
#define LCD_RESET            0xE2 // soft reset command

// Chip Select
#define LCD_CHIP_1           1
#define LCD_CHIP_2           2
#define LCD_CHIP_3           4
#define LCD_CHIP_ALL         7
#define LCD_RIGHT	         201  // the right most pixel        0 ~ 201
#define LCD_BOTTOM	         31   // the bottom of pixel         0 ~ 31

/* LCD screen and bitmap image array consants */
#define LCD_X_BYTES		    (LCD_RIGHT + 1)
#define LCD_Y_BYTES		    (LCD_BOTTOM + 1) / 8 // The number of Page (4page x 8bit = 32 pixels)

// Dot Color modes
#define LCD_DOT_CLR          0
#define LCD_DOT_SET          1
#define LCD_DOT_XOR          2

// AC timing for interface with a 68-type microcontroller at VDD=5 volts(unit:nano second)
#define LCD_tAS              0.02 // 20  tAS1,2:Address setup time with respect to R/W,C/S,C/D (ctrl line changes to E high)
#define LCD_tDS              0.08 // 80  Data setup time (Write data lines setup to dropping E)
#define LCD_tACC             0.90 // 90  Data access time(E high to valid read data)
#define LCD_tEWR             0.10 // 200 Enable pulse width READ(minimum E hi pulse width)
#define LCD_tEWW             0.08 // 160 Enable pulse width WRITE(minimum E hi pulse width)

typedef struct
{
  uint8_t x;
  uint8_t y;
} coordinates;

extern coordinates	_glcd_coord;

// function prototype
extern void glcd_init(void);
extern void glcd_clear(uint8_t fillchar);
extern void glcd_gotoxy(uint8_t x,  uint8_t y);
extern void glcd_offsetwrite(uint8_t data);
extern void glcd_bitmap(const uint8_t* bitmap, uint8_t x, uint8_t y,  const uint8_t color);
#endif
/*
* EOF
*/

