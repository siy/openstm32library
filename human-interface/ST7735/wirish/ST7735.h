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
// Graphics library by ladyada/adafruit 
// MIT license
//Modified by Sergiy Yevtushenko <Sergiy.Yevtushenko@gmail.com>

#include "wirish.h"

#define ST7735_NOP 0x0
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN  0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON  0x12
#define ST7735_NORON  0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1
#define ST7735_PORTRAIT 1
#define ST7735_LANDSCAPE 2

class ST7735 {
public:
	ST7735(HardwareSPI& spi, uint8 CS, uint8 RS, uint8 RST);
	void init(uint8 mode);

	// drawing primitives!
	void drawPixel(uint8 x, uint8 y, uint16 color);
	void drawLine(int16 x, int16 y, int16 x1, int16 y1, uint16 color);
	void fillScreen(uint16 color);
	void drawVerticalLine(uint8 x0, uint8 y0, uint8 length, uint16 color);
	void drawHorizontalLine(uint8 x0, uint8 y0, uint8 length, uint16 color);
	void drawFastLine(uint8 x0, uint8 y0, uint8 l, uint16 color, uint8 flag);
	void drawRect(uint8 x, uint8 y, uint8 w, uint8 h, uint16 color);
	void fillRect(uint8 x, uint8 y, uint8 w, uint8 h, uint16 color);
	void drawCircle(uint8 x0, uint8 y0, uint8 r, uint16 color);
	void fillCircle(uint8 x0, uint8 y0, uint8 r, uint16 color);

	void drawString(uint8 x, uint8 y, const char *c, uint16 color, uint8 size = 1);
	void drawChar(uint8 x, uint8 y, char c, uint16 color, uint8 size = 1);

	void setRotation(uint8 m);
	uint8 getRotation(void) { return madctl; }

	void inversionOn() { writeFullCommand(ST7735_INVON); }
	void inversionOff() { writeFullCommand(ST7735_INVOFF); }

	uint8 width, height;

private:
	void setAddrWindow(uint8 x0, uint8 y0, uint8 x1, uint8 y1);
	void spiwrite(uint8);
	void spiwrite(uint32 size, uint8* buf);
	void writecommandAndParms(uint8 c, uint32 numparms, uint8* params);
	void writeFullCommand(uint8);
	void writecommand(uint8);

	void writecommand(uint8 cmd, uint8 param) {
		writecommandAndParms(cmd, 1, &param);
	}
	void writecommand(uint8 cmd, uint8 param1, uint8 param2) {
		uint8 params[] = {param1, param2};
		writecommandAndParms(cmd, 2, params);
	}
	void writecommand(uint8 cmd, uint8 param1, uint8 param2, uint8 param3) {
		uint8 params[] = {param1, param2, param3};
		writecommandAndParms(cmd, 3, params);
	}

	void pushColor(uint16 color, int count);

	void drawPixelInt(uint8 x, uint8 y, uint16 color) {
		setAddrWindow(x, y, x + 1, y + 1);
		pushColor(color, 1);
	}

	void fillRectInt(uint8 x, uint8 y, uint8 w, uint8 h, uint16 color) {
		setAddrWindow(x, y, x + w, y + h);
		pushColor(color, w * h);
	}

	HardwareSPI& _spi;
	uint8 _cs, _rs, _rst;
	uint8 madctl;
};
