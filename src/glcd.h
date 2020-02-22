#ifndef _GLCD_H_
#define _GLCD_H_
/*
 * $Id: glcd.h 11:39 AM 2019-10-18 ssk  $
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
#include <avr/pgmspace.h>   

// Font Configuration
#define FONT_ENGLISH          0   // 0:English Font
                                            
// Font Indices                             
#define FONT_SIZE       	  0   // byte
#define FONT_FIXED_WIDTH	  2   // pixel
#define FONT_HEIGHT		   	  3   // pixel
#define FONT_FIRST_CHAR		  4   
#define FONT_END_CHAR 		  5   
#define FONT_WIDTH_TABLE	  6   // bytes
/*
* functions relating to bitmap font
*/
extern void glcd_selectfont(const uint8_t* font, uint8_t color, uint8_t type, int8_t sbl);
extern uint8_t   glcd_readfont(const uint8_t* ptr);
extern uint8_t* _glcd_font;
extern uint8_t  _glcd_fontcolor;

extern void glcd_puts(char *str);
extern void glcd_puts_p(PGM_P str);
extern void glcd_putc(uint8_t c);
#define glcd_puts_P(__s) glcd_puts_p(PSTR(__s))
#define isfixedwidth(font)  (glcd_readfont(font+FONT_FIXED_WIDTH) > 0)
#endif
/*
* EOF
*/