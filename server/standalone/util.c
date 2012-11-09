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

#include "ldmz.h"


unsigned int utilHtonl(unsigned int val) {
	return htonl(val);
}


unsigned int utilNtohl(unsigned int val) {
	return ntohl(val);
}


char *utilPathTranslate(const char *path) {
	char *new_path;
	#ifdef _WIN32
		char *blol;
	#endif

	if ((new_path = malloc(strlen(path) + 1)) == NULL)
		return NULL;
	strcpy(new_path, path);

	#ifdef _WIN32
		blol = new_path;
		while (1) {
			blol = strstr(blol, "/");
			if (blol != NULL) {
				*blol = '\\';
				blol++;
			} else
				break;
		}
	#endif

	return new_path;
}


int utilStringToIntArray(const char *str, const char *delimiter, int *dest, int max_tokens) {
	int token;

	for (token = 0; token < max_tokens; token++) {
		sscanf(str, "%i", &dest[token]);
		str = strstr(str, delimiter);
		if (str == NULL)
			break;
		str += strlen(delimiter);
	}

	return token;
}


void utilBlockToHostEndian(unsigned int *block, int elements) {
	UTIL_ENDIAN_CONVERT ec;
	int i;

	ec.i = 0xFF000000;
	if (ec.c[0])
		return;

	/* OPTIMIZE */
	for (i = 0; i < elements; i++) {
		ec.c[0] = ((unsigned int) block[i] >> 24);
		ec.c[1] = ((unsigned int) block[i] >> 16) & 0xFF;
		ec.c[2] = ((unsigned int) block[i] >> 8) & 0xFF;
		ec.c[3] = block[i] & 0xFF;
		block[i] = ec.i;
	}

	return;
}


unsigned int utilStringSum(const char *str) {
	const unsigned char *str_u = (const unsigned char *) str;
	int i;
	unsigned int sum = 0;

	for (i = 0; str_u[i]; i++)
		sum += str_u[i];
	return sum;
}
