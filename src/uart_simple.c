/*
 * $Id: uart.c 11:15 AM 12/5/2019  ssk  $
 *
 * simple uart library for helpful debugging
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
#include <stdio.h>   // printf, FILE
#include "uart_simple.h"

void uart_init(void)
{
#if defined(U2X)
  UCSRA = _BV(U2X);             // improve baud rate error by using 2x clk
  UBRRL = (((10 * F_CPU / (8UL * UART_BAUD)) - 10) + 5) / 10;
  //UCSR0A = _BV(U2X);              // improve baud rate error by using 2x clk
  //UBRR0L = (((10 * F_CPU / (8UL * UART_BAUD)) - 10) + 5) / 10; //xi modification: original formula is wrong, because it doesn't round the result to the nearest integer (causing some baudrates to be way out of spec). This formula is rounding the result using only integers ((10 * result + 5) / 10), which is equivalent to add 0.5 to the result, then taking only the integer part.
#else
  UBRRL = (((10 * F_CPU / (16UL * UART_BAUD)) - 10) + 5) / 10;
  //UBRR0L = (((10 * F_CPU / (16UL * UART_BAUD)) - 10) + 5) / 10;
#endif

  //UCSR0B = _BV(TXEN0);
  UCSRB = _BV(TXEN);

  // ANSI Escape sequences - VT100 / VT52
  // http://ascii-table.com/ansi-escape-sequences-vt-100.php

  //^[ = Esc
  // reset RIS             Reset terminal to initial state        ^[c
  // clearscreen ED2       Clear entire screen                    ^[[2J

  //uart_puts_P("\33c\33[2J");            // Reset & Clear Terminal
  //_delay_ms(100);
#ifdef __DEBUG_MODE__
  printf("\033[36;1m>>> Welcome Debug Mode %lubps, UBRR:%d\033[m\r\n",UART_BAUD, UBRRL);
  //printf("\033[36;1m>>> Welcome Debug Mode %lubps, UBRR:%d\033[m\r\n",UART_BAUD, UBRR0L);
#endif
}

#ifdef __DEBUG_MODE__
int uart_putchar(char c, FILE *stream)
{
  //if (c == '\n') uart_putc('\r');
  uart_putc(c);
  return 0;
}
#endif

void uart_putc(char data)
{
  //loop_until_bit_is_set(UCSR0A, UDRE0);
  //UDR0 = data;
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = data;
}

void uart_puts(const unsigned char *str)
{
  register char c;
  while((c=*str++)) uart_putc(c);

}
//uint8_t uart_getc(void)
//{
////  loop_until_bit_is_set(UCSRA, RXC);
////  return UDR;
//  loop_until_bit_is_set(UCSR0A, RXC0);
//  return UDR0;
//}

void uart_puts_p(const char *progmem_s)
{
  register char c;
  while((c= pgm_read_byte(progmem_s++))) uart_putc(c);
}

/*
 * EOF
 */