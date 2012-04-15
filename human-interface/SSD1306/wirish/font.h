/*
 * font.h
 *
 *  Created on: Feb 23, 2012
 *      Author: siy
 */

#ifndef FONTINFO_H_
#define FONTINFO_H_

#include <WProgram.h>

typedef struct
{
	uint8 width;
	uint8 height;
	char first;
	char last;
	uint8* data;
} Font;

extern Font timesNewRoman;
extern Font freeScale5x7;
extern Font freeScale7x15;

#endif /* FONTINFO_H_ */
