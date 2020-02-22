/*
 * $Id: glcd.c 09:03 AM 2019-10-19 ssk  $
 *
 * Functions relating to graphics. e.g bitmap font display, drawing lines, rectangles, circles etc.
 *
 * Designed for Dot Matrix Graphic LCD
 *
 * Originally Adapted from
 *    Most Functions   : GLCD Version 3 (Beta) Jun 9 2010 <http://arduino.cc/forum/index.php/topic,22208.0.html>
 *    Font Function&Data: Martin Thomas, Kaiserslautern, Germany <eversmith@heizung-thomas.de>
 *                       avr-gcc 3.4.3/avr-libc 1.2.3 (WinAVR 2/2005)
 *    Cube Drawing     : Osama's Lab KS0108 GLCD library <https://sites.google.com/site/osamaslab>
 *    Line Drawing     : http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
 *    Circle Drawing   : http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 *    Bar Graph, Etc,. : http://agock.com/software/glcd-graphic-lcd-library/
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
*  \Revision History
*  Note. The Revision History showns the most amenddment first
*
*  1:18 PM 12/4/2019   remove unnecessary function (cube, line, bar, cicle)
*  09:03 AM 2019-10-19 Merge font.c functions & clear-out #define
*  2014-12-25  7:12:23 PM Add Bar graph Function
*  2014-12-24  3:19:42 PM Circle function bug fix & add graph function
*                         Originally Adapted from  http://agock.com/software/glcd-graphic-lcd-library/
*  2014-12-22  2:55:53 AM Initial creation (based on Truly Semiconductors MCG2305-A1-E with 192x64 Graphic LCD)
*
*/
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "sbn166g.h"
#include "glcd.h"

uint8_t   _glcd_fontcolor;
 int8_t   _glcd_sbl;        // space between letters;

uint8_t*  _glcd_font;
uint8_t glcd_readfont(const uint8_t* ptr)
{
  return pgm_read_byte(ptr);
}

void glcd_selectfont(const uint8_t* font, uint8_t color, uint8_t type, int8_t sbl)
{
  _glcd_font  = (uint8_t *)font;  // save new font
  _glcd_fontcolor = color;             // save new font color
  _glcd_sbl       = sbl;               // save Space between letter
}

/**
 * output a character
 *
 * @param c the character to output
 *
 * If the character will not fit on the current text line
 * inside the text area,
 * the text position is wrapped to the next line.
 *
 */
void glcd_putc(uint8_t c)
{
  if(_glcd_coord.x > LCD_RIGHT)  return;    // reached a end column
  if(_glcd_coord.y > LCD_BOTTOM) return;    // reached a end row
  if(c             < 0x20)       return;    // special character

 uint16_t index = 0;
  uint8_t x;        // current X coordinate 0..255
  uint8_t y;        // current Y coordinate 0..63 (LOOK! datatype uint8_t)
  uint8_t height    = glcd_readfont(_glcd_font+FONT_HEIGHT);  
  
  uint8_t page      = (height+7)/8;
  uint8_t firstChar = glcd_readfont(_glcd_font+FONT_FIRST_CHAR);  
  
  uint8_t charCount = glcd_readfont(_glcd_font+FONT_END_CHAR) - glcd_readfont(_glcd_font+FONT_FIRST_CHAR) + 1;
  uint8_t width;    // font width pixels

  if(c < firstChar || c >= (firstChar+charCount)) return; // invalid char

  c-= firstChar;

  if(isfixedwidth(_glcd_font))
  {
    width = glcd_readfont(_glcd_font+FONT_FIXED_WIDTH);
    index = c*page*width+FONT_WIDTH_TABLE;
  }
  else
  {
    // variable width font, read width data, to get the index
    /*
     * Because there is no table for the offset of where the data
     * for each character glyph starts, run the table and add up all the
     * widths of all the characters prior to the character we need to locate.
     */

    // read width data, to get the index
    for(uint16_t i=0; i<c; i++) index += glcd_readfont(_glcd_font+FONT_WIDTH_TABLE+i);

    index = index*page+charCount+FONT_WIDTH_TABLE;

    // Finally, fetch the width of our character
    width = glcd_readfont(_glcd_font+FONT_WIDTH_TABLE+c);
  }

  x = _glcd_coord.x;
  y = _glcd_coord.y;

  for(uint8_t i=0; i<page; i++)         /* each vertical byte, x axis */
  {
    for(uint8_t j=0; j<width; j++)      /* each Horizontal pixel, y axis */
    {
      uint8_t data = glcd_readfont(_glcd_font+index+(i*width)+j);
      if(_glcd_fontcolor == LCD_DOT_XOR)
      {
        data = ~data;
      }
      glcd_offsetwrite(data);
    }

    if(_glcd_coord.y+8 > LCD_BOTTOM)
    {
      break;
    }
    glcd_gotoxy(x, _glcd_coord.y+8);
  }
  glcd_gotoxy(x+width+_glcd_sbl, y);
}

// Character data put string
void glcd_puts(char *str)
{
  uint8_t c;

  while((c = *str++))
  {
    glcd_putc(c);
  }
}

// PROGMEM data put string
void glcd_puts_p(PGM_P str)
{
  uint8_t c;

  while((c = pgm_read_byte(str++)))
  {
    glcd_putc(c);
  }
}
/*
* EOF
*/