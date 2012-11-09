/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UTIL_H__
#define	__UTIL_H__

#ifdef _WIN32
	#include <winsock2.h>
#else
	#include <arpa/inet.h>
#endif


typedef union {
	unsigned int		i;
	unsigned char		c[4];
} UTIL_ENDIAN_CONVERT;

unsigned int utilHtonl(unsigned int val);
unsigned int utilNtohl(unsigned int val);
char *utilPathTranslate(const char *path);
int utilStringToIntArray(const char *str, const char *delimiter, int *dest, int max_tokens);
void utilBlockToHostEndian(unsigned int *block, int elements);
unsigned int utilStringSum(const char *str);


#endif
