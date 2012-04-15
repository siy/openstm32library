/*************************************************** 
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
/*
// Graphics library by ladyada/adafruit with init code from Rossum 
// Edited by Peter Archer to work with the LeafLabs Maple
// Modified by Sergiy Yevtushenko <Sergiy.Yevtushenko@gmail.com>
// (various performance improvements and cleanups)
*/

#include "ST7735.h"
#include "glcdfont.c"
#include <HardwareSPI.h>

#define swap(a, b) { uint16 t = a; a = b; b = t; }

ST7735::ST7735(HardwareSPI& spi, uint8 cs, uint8 rs, uint8 rst):_spi(spi), _cs(cs), _rs(rs), _rst(rst) {
}

inline void ST7735::spiwrite(uint8 c) {
	_spi.write(c);
}

inline void ST7735::spiwrite(uint32 size, uint8* buf) {
	_spi.write(buf, size);
}

void ST7735::writecommand(uint8 c) {
	digitalWrite(_rs, LOW);
	spiwrite(c);
}

void ST7735::writeFullCommand(uint8 c) {
	digitalWrite(_cs, LOW);
	digitalWrite(_rs, LOW);
	spiwrite(c);
	digitalWrite(_cs, HIGH);
}

void ST7735::writecommandAndParms(uint8 c, uint32 numparms, uint8* params) {
	writecommand(c);
	digitalWrite(_rs, HIGH);
	spiwrite(numparms, params);
}

void ST7735::setRotation(uint8 m) {
	digitalWrite(_cs, LOW);
	writecommand(ST7735_MADCTL, madctl = m);
	digitalWrite(_cs, HIGH);
}

void ST7735::setAddrWindow(uint8 x0, uint8 y0, uint8 x1, uint8 y1) {
	uint8 parmCaset[4] = {0x00, x0, 0x01, x1};
	uint8 parmRaset[4] = {0x00, y0, 0x00, y1};
	writecommandAndParms(ST7735_CASET, sizeof(parmCaset), parmCaset);	//START/END
	writecommandAndParms(ST7735_RASET, sizeof(parmRaset), parmRaset);	//START/END

	writecommand(ST7735_RAMWR); // write to RAM
}

void ST7735::pushColor(uint16 color, int count) {
	uint8 colorData[2] = {(color >> 8) & 0xFF, color & 0xFF};
	digitalWrite(_rs, HIGH);

	for(int i = 0; i < count; i++) {
		spiwrite(sizeof(colorData), colorData);
	}
}

void ST7735::drawPixel(uint8 x, uint8 y, uint16 color) {
	if ((x >= width) || (y >= height))
		return;

	digitalWrite(_cs, LOW);
	drawPixelInt(x, y, color);
	digitalWrite(_cs, HIGH);
}

void ST7735::fillScreen(uint16 color) {
	fillRect(0, 0, width, height, color);
}

void ST7735::init(uint8 mode) {
	// set pin directions
	pinMode(_rs, OUTPUT);

	_spi.begin(SPI_18MHZ, MSBFIRST, SPI_MODE_3);

	// toggle RST low to reset; CS low so it'll listen to us
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, LOW);
	if (_rst) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}

	writecommand(ST7735_SWRESET); // software reset
	delay(150);

	writecommand(ST7735_SLPOUT); // out of sleep mode
	delay(500);

	// frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	// same rate setting for all modes, normal, idle, partial
	writecommand(ST7735_FRMCTR1, 0x01, 0x2C, 0x2D); // frame rate control - normal mode
	writecommand(ST7735_FRMCTR2, 0x01, 0x2C, 0x2D); // frame rate control - idle mode
	uint8 rateParm3[6] = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}; // dot inversion mode, line inversion mode
	writecommandAndParms(ST7735_FRMCTR3, sizeof(rateParm3), rateParm3); // frame rate control - partial mode

	writecommand(ST7735_INVCTR, 0x07); // display inversion control, no inversion

	writecommand(ST7735_PWCTR1, 0xA2, 0x02, 0x84); // power control, -4.6V, AUTO mode
	writecommand(ST7735_PWCTR2, 0xC5); // power control, VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
	writecommand(ST7735_PWCTR3, 0x0A, 0x00); // power control, Opamp current small, Boost frequency
	writecommand(ST7735_PWCTR4, 0x8A, 0x2A); // power control, BCLK/2, Opamp current small & Medium low
	writecommand(ST7735_PWCTR5, 0x8A, 0xEE); // power control
	writecommand(ST7735_VMCTR1, 0x0E); // power control

	writecommand(ST7735_INVOFF); // don't invert display

	madctl = (mode == ST7735_PORTRAIT) ? 0x08 : 0x68;
	writecommand(ST7735_MADCTL, madctl); // memory access control (directions)
	writecommand(ST7735_COLMOD, 0x05); // set color mode, 16-bit color

    width = (mode == ST7735_PORTRAIT) ? 128 : 160;
    height = (mode == ST7735_PORTRAIT) ? 160 : 128;

	uint8 addrSetParms[] = {0x00, 0x00, 0x00, width - 1};
	writecommandAndParms(ST7735_CASET, sizeof(addrSetParms), addrSetParms); // column addr set
	addrSetParms[3] = height - 1;
	writecommandAndParms(ST7735_RASET, sizeof(addrSetParms), addrSetParms); // column addr set

	//Gamma sets
	uint8 gammaSet1[] = { 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
						  0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10};
	uint8 gammaSet2[] = { 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
						  0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10};

	writecommandAndParms(ST7735_GMCTRP1, sizeof(gammaSet1), gammaSet1);
	writecommandAndParms(ST7735_GMCTRN1, sizeof(gammaSet2), gammaSet2);

	writecommand(ST7735_DISPON);
	delay(100);

	writecommand(ST7735_NORON); // normal display on
	delay(10);
	digitalWrite(_cs, HIGH);
}

// draw a string from memory

void ST7735::drawString(uint8 x, uint8 y, const char *c, uint16 color, uint8 size) {
	while (c[0] != 0) {
		drawChar(x, y, c[0], color, size);
		x += size * 6;
		c++;
		if (x + 5 >= width) {
			y += 10;
			x = 0;
		}
	}
}
// draw a character
void ST7735::drawChar(uint8 x, uint8 y, char c, uint16 color, uint8 size) {
	digitalWrite(_cs, LOW);
	for (uint8 i = 0; i < 5; i++) {
		uint8 line = font[(c * 5) + i];
		for (uint8 j = 0; j < 8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					drawPixelInt(x + i, y + j, color);
				else { // big size
					fillRectInt(x + i * size, y + j * size, size, size, color);
				}
			}
			line >>= 1;
		}
	}
	digitalWrite(_cs, HIGH);
}

// fill a circle
void ST7735::fillCircle(uint8 x0, uint8 y0, uint8 r, uint16 color) {
	int16 f = 1 - r;
	int16 ddF_x = 1;
	int16 ddF_y = -2 * r;
	int16 x = 0;
	int16 y = r;

	drawVerticalLine(x0, y0 - r, 2 * r + 1, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawVerticalLine(x0 + x, y0 - y, 2 * y + 1, color);
		drawVerticalLine(x0 - x, y0 - y, 2 * y + 1, color);
		drawVerticalLine(x0 + y, y0 - x, 2 * x + 1, color);
		drawVerticalLine(x0 - y, y0 - x, 2 * x + 1, color);
	}
}

// draw a circle outline
void ST7735::drawCircle(uint8 x0, uint8 y0, uint8 r, uint16 color) {
	int16 f = 1 - r;
	int16 ddF_x = 1;
	int16 ddF_y = -2 * r;
	int16 x = 0;
	int16 y = r;

	digitalWrite(_cs, LOW);

	drawPixelInt(x0, y0 + r, color);
	drawPixelInt(x0, y0 - r, color);
	drawPixelInt(x0 + r, y0, color);
	drawPixelInt(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixelInt(x0 + x, y0 + y, color);
		drawPixelInt(x0 - x, y0 + y, color);
		drawPixelInt(x0 + x, y0 - y, color);
		drawPixelInt(x0 - x, y0 - y, color);

		drawPixelInt(x0 + y, y0 + x, color);
		drawPixelInt(x0 - y, y0 + x, color);
		drawPixelInt(x0 + y, y0 - x, color);
		drawPixelInt(x0 - y, y0 - x, color);

	}
	digitalWrite(_cs, HIGH);
}

// draw a rectangle
void ST7735::drawRect(uint8 x, uint8 y, uint8 w, uint8 h, uint16 color) {
	// smarter version
	drawHorizontalLine(x, y, w, color);
	drawHorizontalLine(x, y + h - 1, w, color);
	drawVerticalLine(x, y, h, color);
	drawVerticalLine(x + w - 1, y, h, color);
}

void ST7735::fillRect(uint8 x, uint8 y, uint8 w, uint8 h, uint16 color) {
	// smarter version
	digitalWrite(_cs, LOW);
	fillRectInt(x, y, w, h, color);
	digitalWrite(_cs, HIGH);
}

void ST7735::drawVerticalLine(uint8 x, uint8 y, uint8 length, uint16 color) {
	if (x >= width)
		return;
	if (y + length >= height)
		length = height - y - 1;

	drawFastLine(x, y, length, color, 1);
}

void ST7735::drawHorizontalLine(uint8 x, uint8 y, uint8 length, uint16 color) {
	if (y >= height)
		return;
	if (x + length >= width)
		length = width - x - 1;

	drawFastLine(x, y, length, color, 0);
}

void ST7735::drawFastLine(uint8 x, uint8 y, uint8 length, uint16 color, uint8 rotflag) {
	digitalWrite(_cs, LOW);
	if (rotflag) {
		setAddrWindow(x, y, x, y + length);
	} else {
		setAddrWindow(x, y, x + length, y + 1);
	}

	pushColor(color, length);
	digitalWrite(_cs, HIGH);
}

// bresenham's algorithm - thx wikpedia
void ST7735::drawLine(int16 x0, int16 y0, int16 x1, int16 y1, uint16 color) {
	uint16 steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint16 dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16 err = dx / 2;
	int16 ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	digitalWrite(_cs, LOW);

	for (; x0 <= x1; x0++) {
		if (steep) {
			drawPixelInt(y0, x0, color);
		} else {
			drawPixelInt(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
	digitalWrite(_cs, HIGH);
}
