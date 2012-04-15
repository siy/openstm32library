/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/
/*
Modified for Maple by Sergiy Yevtushenko <Sergiy.Yevtushenko@gmail.com>
*/
#include <WProgram.h>
#include "font.h"

#define swap(a, b) { uint8 t = a; a = b; b = t; }

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 128
#define LCDHEIGHT 64

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
//#define CMD_SET_BIAS_9 0xA2
//#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define CMD_SET_BOOSTER_5  1
#define CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST 0xF0
#define CMD_SET_RATION_FIRST	0xA8
#define CMD_SET_RATION_SECOND 	0
#define CMD_DCDC_CONTROL		0xAD
#define CMD_DCDC_CONTROL_OFF	0x8A
#define CMD_DCDC_CONTROL_ON		0x8B
#define CMD_SET_FREQ_DIV_FIRST	0xD5
#define CMD_SET_FREQ_DIV_SECOND 0

class SSD1306 {
public:
	SSD1306(HardwareSPI& _spi, uint8 _a0, uint8 _cs, uint8 _rst, Font& _font) : spi(_spi), a0(_a0), rst(_rst), cs(_cs), font(_font){};
	void begin(uint8 contrast);
	void set_contrast(uint8 val);
	void clear_display(void);
	void clear();
	void display();

	void setpixel(uint8 x, uint8 y, uint8 color);
	uint8 getpixel(uint8 x, uint8 y);
	void fillcircle(uint8 x0, uint8 y0, uint8 r, uint8 color);
	void drawcircle(uint8 x0, uint8 y0, uint8 r, uint8 color);
	void drawrect(uint8 x, uint8 y, uint8 w, uint8 h, uint8 color);
	void fillrect(uint8 x, uint8 y, uint8 w, uint8 h, uint8 color);
	void drawline(uint8 x0, uint8 y0, uint8 x1, uint8 y1, uint8 color);
	void drawchar(uint8 x, uint8 line, uint8 c);
	void drawstring(uint8 x, uint8 line, const char *c);

	void drawbitmap(uint8 x, uint8 y, const uint8 *bitmap, uint8 w, uint8 h, uint8 color);
	void invert(uint8 i);

private:
	HardwareSPI& spi;
	uint8 a0, rst, cs;
	Font& font;

	void init();
	void writeCommand(uint8 c);

	uint8 buffer[128*64/8];
};

extern SSD1306 GLCD;
