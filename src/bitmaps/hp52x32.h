/*
 * $Id: manmool200x32.h 2013/01/29 22:50:05  ssk  $
 *
 * This program is part of Winstar WG20232A-TMI-V#A  with 202x32 Graphic LCD Tutorial.
 * Designed for AVANT SBN166G Dot-matrix STN LCD Driver with 32-row x 80-column based LCDs on Atmels AVR MCUs
 *
 * Copyright (C) 2012  ssk.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

/**
 * \mainpage
 *  
 * HP Logo
 * 
 * Bit byte Aligment : LSB to MSB, Top to Bottom !
 * \author ssk.
*/
#include <inttypes.h>
#include <avr/pgmspace.h>

const uint8_t hp52x32[] PROGMEM = 
{
  52, // width
  32, // height 
	//     ################# ####  ####################    
	//   ##################  ###  #######################  
	//  ##################   ###      #################### 
	//  ################    ####         ################# 
	// ################     ###           #################
	// ###############      ###            ################
	// ##############      ####             ###############
	// #############       ###               ##############
	// #############      ####                #############
	// ############       ####                 ############
	// ############       #######   ########   ############
	// ###########       #########  ########   ############
	// ###########       #### ####  ###  ###    ###########
	// ###########       ###  ###  ####  ###    ###########
	// ###########      ####  ###  #### ####    ###########
	// ###########      ###  ####  ###  ###     ###########
	// ###########      ###  ###  ####  ###     ###########
	// ###########     #### ####  #### ####     ###########
	// ###########     ###  ####  ###  ###      ###########
	// ###########     ###  ###  ####  ###      ###########
	// ###########    #### ####  #### ####     ############
	// ############   ###  ####  ########      ############
	// ############             ########       ############
	// #############            ###           #############
	// ##############           ###          ##############
	// ##############          ####         ###############
	// ###############         ###          ###############
	// #################      ####        #################
	//  #################     ####       ################# 
	//  ####################  ###     #################### 
	//   ################### ####  ######################  
	//     ################  #### #####################    
	0xF0, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x07, 0x03, 0xC1, 0xF8, 0xFF, 0x7F, 0x0F, 0x01, 0x00, 0x02, 0x03, 0x03, 0x03, 0x07, 0x07, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xF0, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF8, 0xFF, 0x7F, 0x1F, 0x8F, 0xFC, 0xFC, 0xFC, 0x18, 0x00, 0xE0, 0xFC, 0xFC, 0x7C, 0x0C, 0xCC, 0xFC, 0xFC, 0x7C, 0x00, 0x00, 0x01, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x80, 0x00, 0x00, 0x30, 0x3E, 0x3F, 0x1F, 0x03, 0x30, 0x3E, 0x3F, 0x3F, 0x07, 0xC0, 0xF8, 0xFF, 0x7F, 0x7F, 0x63, 0x70, 0x7E, 0x3F, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0xE0, 0x60, 0x00, 0xC0, 0xF8, 0xFE, 0xFF, 0x1F, 0x83, 0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF8, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x0F, 


};
