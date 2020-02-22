/*
 * $Id: sbn166g_i2c.c 01:02 PM 2019-10-18 ssk  $
 *
 * Winstar WG20232A-TMI-V#A(202x32) LCD library.
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

/**
 *
 * A Winstar WG20232A-TMI-V#A with 202x32 Graphic LCD module has been used
 * for testing. This module supports "read/write".
 * There is option to read data from the SBN166G RAM.
 *
 * IMPORTANT NOTE
 *
 * RESET Pin
 *
 * Hardware RESET and interface type selection.
 * This pin is a dual function pin. It can be used to reset the SBN1661G_X and select the type of interface timing.
 * The hardware RESET is edge-sensitive. IT IS NOT LEVEL-SENSITIVE
 *
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "sbn166g.h"

coordinates	 _glcd_coord;
uint8_t _control_byte=0;

// Low level LCD Controller Interface Support function
static void _glcd_command(const uint8_t data, uint8_t device);
static void _glcd_data(const uint8_t data);
static void _chip_select(uint8_t x);
static void _chip_unselect(void);
static uint8_t _glcd_read_data(void);

// routine to initialize the operation of the LCD display subsystem
void glcd_init(void)
{
  // output port
  //LCD_RST_DDR     |=  _BV(LCD_RST_PIN);
  LCD_CONTROL_DDR |=  _BV(LCD_A0_PIN)  | _BV(LCD_RW_PIN);
  LCD_CHIP1_DDR   |=  _BV(LCD_CS1_PIN);
  LCD_CHIP2_DDR   |=  _BV(LCD_CS2_PIN);
  LCD_CHIP3_DDR   |=  _BV(LCD_CS3_PIN);

  // lcd data high & low nibble port output
  LCD_DATA_H_DDR  |=  0xf0;
  LCD_DATA_L_DDR  |=  0x0f;

  // The hardware RESET is edge-sensitive. It is not level-sensitive. !!!
  // The value is relative to the RESET pulse edge.
  // That is, 10 •ÏS after the last RESET edge, the device is completely reset
//  LCD_RST_PORT    &= ~_BV(LCD_RST_PIN);
// _delay_us(50);
//
//  // The hardware RESET is edge-sensitive. It is not level-sensitive. !!!
//  LCD_RST_PORT    |=  _BV(LCD_RST_PIN);    // 68-type microcontroller for interface.

//  _glcd_command(LCD_RESET, LCD_CHIP_ALL);
//  _delay_ms(5);

  _glcd_command(LCD_STATIC_OFF, LCD_CHIP_ALL);
  _glcd_command(LCD_DUTY_32, LCD_CHIP_ALL);
  _glcd_command(LCD_SET_ADC_NOR, LCD_CHIP_ALL);
  _glcd_command(LCD_SET_RMW_END, LCD_CHIP_ALL);
  _glcd_command(LCD_START_LINE, LCD_CHIP_ALL);
  _glcd_command(LCD_DISP_ON, LCD_CHIP_ALL);
}

/**
 * Send LCD controller instruction command
 * Input: instruction to send to LCD controller
 * Returns: none
*/
static void _glcd_command(const uint8_t data, uint8_t device)
{
  uint8_t portmask;

  LCD_CONTROL_PORT &= ~(_BV(LCD_A0_PIN));   // Low : Display Control data, High : Display data
  LCD_CONTROL_PORT &= ~(_BV(LCD_RW_PIN));   // Low : Write Control signal, High : Read Control signal

  portmask = LCD_DATA_H_PORT & 0x0f;
  LCD_DATA_H_PORT = portmask | (data & 0xf0);

  portmask = LCD_DATA_L_PORT & 0xf0;
  LCD_DATA_L_PORT = portmask | (data & 0x0f);

  //_delay_us(LCD_tAS);                   // tAS1,2:Address setup time with respect to R/W,C/S,C/D (ctrl line changes to E high)

  if( device & LCD_CHIP_1 ) LCD_CHIP1_PORT |= _BV(LCD_CS1_PIN);
  if( device & LCD_CHIP_2 ) LCD_CHIP2_PORT |= _BV(LCD_CS2_PIN);
  if( device & LCD_CHIP_3 ) LCD_CHIP3_PORT |= _BV(LCD_CS3_PIN);
  //_delay_us(LCD_tEWW);                  // Enable pulse width WRITE(minimum E hi pulse width)
 _chip_unselect();
}
/**
 * Send data byte to LCD controller
 * Input: instruction to send to LCD controller
 * Returns: none
*/
static void _glcd_data(uint8_t data)
{
  uint8_t portmask;
  if(_glcd_coord.x == LCD_CHIP2_START_X)	// chip2 X-address = 0
  {
  	_glcd_command(LCD_SET_COL, LCD_CHIP_2);
  }
  else
  if(_glcd_coord.x == LCD_CHIP3_START_X)	// chip2 X-address = 0
  {
  	_glcd_command(LCD_SET_COL, LCD_CHIP_3);
  }

  LCD_CONTROL_PORT |=  (_BV(LCD_A0_PIN));       // High: Display data, Low : Display Control data
  LCD_CONTROL_PORT &= ~(_BV(LCD_RW_PIN));       // Low : Write Control signal,High: Read Control signal

  portmask = LCD_DATA_H_PORT & 0x0f;
  LCD_DATA_H_PORT = portmask | (data & 0xf0);

  portmask = LCD_DATA_L_PORT & 0xf0;
  LCD_DATA_L_PORT = portmask | (data & 0x0f);
  //_delay_us(LCD_tAS);                       // tAS1,2:Address setup time with respect to R/W,C/S,C/D (ctrl line changes to E high)

  _chip_select(_glcd_coord.x);
  //_delay_us(LCD_tEWW);                      // Enable pulse width WRITE(minimum E hi pulse width)
  _chip_unselect();
}

/* Specify a controller between  0 and LCD_RIGHT  */
static void _chip_select(uint8_t x)
{
  // Controller 1 area
  if( x < LCD_CHIP2_START_X)
  {
  	LCD_CHIP1_PORT |= _BV(LCD_CS1_PIN);
  }
  // Controller 2 area
  else
  if(x >= LCD_CHIP2_START_X && x < LCD_CHIP3_START_X)
  {
    LCD_CHIP2_PORT |= _BV(LCD_CS2_PIN);
  }
  // Controller 3 area
  else
  if( x >=  LCD_CHIP3_START_X)
  {
    LCD_CHIP3_PORT |= _BV(LCD_CS3_PIN);
  }
}
static void _chip_unselect(void)
{
  LCD_CHIP1_PORT &= ~_BV(LCD_CS1_PIN);
  LCD_CHIP2_PORT &= ~_BV(LCD_CS2_PIN);
  LCD_CHIP3_PORT &= ~_BV(LCD_CS3_PIN);
}
/*
 * Send data byte to LCD controller (Pixel Aligment Write)
 * Input: data
 * Returns: none
 *
*/
void glcd_offsetwrite(const uint8_t data)
{
  if(_glcd_coord.x > LCD_RIGHT)  return;    //  reached a end column
  if(_glcd_coord.y > LCD_BOTTOM) return;    //  reached a end row

  uint8_t savedata;
  uint8_t yoffset  = _glcd_coord.y % 8;
  uint8_t changeYaxis;

  if(yoffset)
  {
    // first page
    savedata =  _glcd_read_data();
    savedata |= (data << yoffset);
    _glcd_data(savedata);

    // second page
    if((_glcd_coord.y+8) > LCD_BOTTOM)
    {
       glcd_gotoxy(_glcd_coord.x+1, _glcd_coord.y);
       return;
    }
    glcd_gotoxy(_glcd_coord.x, _glcd_coord.y+8);
    savedata = _glcd_read_data();
    savedata |= data >> (8-yoffset);

    _glcd_data(savedata);
    changeYaxis = _glcd_coord.y-8;
  }
  else
  {
    _glcd_data(data);
    changeYaxis = _glcd_coord.y;
  }
  glcd_gotoxy(_glcd_coord.x+1, changeYaxis);
}
 /**
 * Read a data byte from the given position
 *
 * @return Data read from LCD ram
 *
 * @note for Non-Framebuffer
 *
*/
static uint8_t _glcd_read_data(void)
{
  uint8_t data;

  _glcd_command(LCD_SET_RMW_START, LCD_CHIP_ALL);    // Read-Modify-Write Start

    LCD_DATA_H_DDR   &=  0x0f;           // high nibble input
    LCD_DATA_L_DDR   &=  0xf0;           // low  nibble input

    LCD_CONTROL_PORT |=  _BV(LCD_A0_PIN);  // High : Display data,        Low : Display Control data
    LCD_CONTROL_PORT |=  _BV(LCD_RW_PIN);  // High : Read Control signal, Low : Write Control signal
#if (F_CPU >= 16000000)
    _delay_us(LCD_tAS);                    // Address setup time with respect to R/W,C/S,C/D (ctrl line changes to E high)
#endif

    // dummy read
    _chip_select(_glcd_coord.x);         // Enable signal (E) for the 68-type microcontroller
    _chip_unselect();

    // read
    _chip_select(_glcd_coord.x);         // Enable signal (E) for the 68-type microcontroller
   //_delay_us(LCD_tACC);                 // Data access time(E high to valid read data)

    // Get data from LCD data
    data = (LCD_DATA_H_INPUT & 0xf0) | (LCD_DATA_L_INPUT & 0x0f);

    _chip_unselect();

    LCD_DATA_H_DDR  |=  0xf0;           // output
    LCD_DATA_L_DDR  |=  0x0f;           // output

  _glcd_command(LCD_SET_RMW_END, LCD_CHIP_ALL);     // Read-Modify-Write END

  return(data);
}
 /**
 * go to at x,y to the given position
 *
 * @param x X coordinate, a value from 0 to LCD_RIGHT
 * @param y Y coordinate, a value from 0 to LCD_BOTTOM
 *
 * Sets the current position at location the specified x,y
 * x and y are relative to the 0,0 origin of the display which
 * is the upper left corner.
 * Requests to set pixels outside the range of the display will be ignored.
 *
 * @note for Non-Framebuffer
 *
*/
void glcd_gotoxy(uint8_t x,  uint8_t y)
{
  if(x > LCD_RIGHT)  return;      //  reached a end column
  if(y > LCD_BOTTOM) return;      //  reached a end row

  uint8_t device = 0;

  // save new coordinates
  _glcd_coord.x = x;
  _glcd_coord.y = y;

  y >>= 3;     // page coordinate of the cursor (0 ~ LCD_Y_BYTES)

  if(x < LCD_CHIP2_START_X)                          // 0 ~ LCD_CHIP2_START_X - 1 (Controller 1 area)
  {
  	device = LCD_CHIP_1;
  }
  else
  if((x >= LCD_CHIP2_START_X) && (x < LCD_CHIP3_START_X)) // LCD_CHIP2_START_X ~ LCD_CHIP3_START_X - 1 (Controller 2 area)
  {
    device = LCD_CHIP_2;
    x -= LCD_CHIP2_START_X;
  }
  else
  if( x >=  LCD_CHIP3_START_X)                        // LCD_CHIP3_START_X ~ LCD_RIGHT Controller 3 area)
  {
    device = LCD_CHIP_3;
    x -= LCD_CHIP3_START_X;
  }

  _glcd_command(LCD_SET_COL  + x, device);
  _glcd_command(LCD_SET_PAGE + y, LCD_CHIP_ALL);
}
/**
 * set pixel at x,y to the given color
 *
 * @param x X coordinate, a value from 0 to LCD_RIGHT
 * @param y Y coordinate, a value from 0 to LCD_BOTTOM
 * @param color BLACK or CLEAR or INVERT
 *
 * Sets the pixel at location x,y to the specified color.
 * x and y are relative to the 0,0 origin of the display which
 * is the upper left corner.
 * Requests to set pixels outside the range of the display will be ignored.
 *
 * @note
 *
*/
//void glcd_dot(const uint8_t x, const uint8_t y, const uint8_t color)
//{
//  unsigned char *pBuffer;  /* pointer used for optimisation */
//
//  if(x > LCD_RIGHT)  return;    //  reached a end column
//  if(y > LCD_BOTTOM) return;    //  reached a end row
//
//  pBuffer = &(_framebuffer[y/8][x]);    //  page, y/8 or  y >> 3;
//
//  uint8_t rest = y % 8;
//  switch (color)
//  {
//    case LCD_DOT_CLR:
//      *pBuffer &= ~(_BV(rest));    // 0x01 << (rest));
//       break;
//
//    case LCD_DOT_XOR:
//      *pBuffer ^=  (_BV(rest));    // 0x01 << (rest));
//        break;
//
//  	case LCD_DOT_SET:
//    default:
//      *pBuffer |=  _BV(rest);      // 0x01 << (rest);
//       break;
//  }
//}

// clear screen
void glcd_clear(uint8_t fillchar)
{
  uint8_t portmask;

  for(uint8_t page=LCD_Y_BYTES; page--;)
  {
  	_glcd_command(LCD_SET_PAGE+page,LCD_CHIP_ALL);
  	_glcd_command(LCD_SET_COL,LCD_CHIP_ALL);

    LCD_CONTROL_PORT |=  (_BV(LCD_A0_PIN));  // High : Display data,        Low : Display Control data
    LCD_CONTROL_PORT &= ~(_BV(LCD_RW_PIN));  // Low : Write Control signal, High : Read Control signal

    for(uint8_t x=LCD_X_BYTES; x--;)
    {
      _chip_select(x);

       portmask = LCD_DATA_H_PORT & 0x0f;
       LCD_DATA_H_PORT = portmask | (fillchar & 0xf0);

       portmask = LCD_DATA_L_PORT & 0xf0;
       LCD_DATA_L_PORT = portmask | (fillchar & 0x0f);

      _chip_unselect();
    }
  }
}
/**
 * Draw a glcd bitmap image
 *
 * @param bitmap a ponter to the bitmap data
 * @param x the x coordinate of the upper left corner of the bitmap
 * @param y the y coordinate of the upper left corner of the bitmap
 * @param color BLACK, WHITE or INVERT
 *
 * Draws a bitmap image with the upper left corner at location x,y
 * The bitmap data is assumed to be in program memory.
 *
 * @note : Bit byte Aligment : LSB to MSB, Top to Bottom !
 */
void glcd_bitmap(const uint8_t* bitmap, uint8_t x, uint8_t y,  const uint8_t color)
{
  if(x > LCD_RIGHT)  return;    //  reached a end column
  if(y > LCD_BOTTOM) return;    //  reached a end row

  uint8_t width  = pgm_read_byte(bitmap++);
  uint8_t height = pgm_read_byte(bitmap++);

  height /= 8;

  for(uint8_t page=0; page<height; page++)
  {
    glcd_gotoxy(x, y + (page*8) );
	for(uint8_t i = 0; i < width; i++)
	{
      uint8_t data = pgm_read_byte(bitmap++);

      if(color == LCD_DOT_CLR)
      {
        data &= ~data;
      }
      else
      if(color == LCD_DOT_XOR)
      {
        data ^= data;
      }
      glcd_offsetwrite(data);
	}
  }
}
/**
 * EOF
*/