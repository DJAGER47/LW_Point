#include "ssd1283A.h"

#include "main.h"
#include "cmsis_os.h"

//#define _delay HAL_Delay
#define _delay osDelay
//==============================================================================
//==============================================================================

#define SPI_DISP    hspi3
extern SPI_HandleTypeDef SPI_DISP;
extern DMA_HandleTypeDef hdma_spi3_tx;

#define CS_Port       LED_CS_GPIO_Port
#define CS_Pin        LED_CS_Pin

#define Reset_Port    LED_RES_GPIO_Port
#define Reset_Pin     LED_RES_Pin
#define DC_Port       LED_DC_GPIO_Port
#define DC_Pin        LED_DC_Pin
//#define LED_Port      LED_LED_GPIO_Port
//#define LED_Pin       LED_LED_Pin

#define RESET_HIGH()    HAL_GPIO_WritePin(Reset_Port, Reset_Pin, GPIO_PIN_SET)
#define RESET_LOW()     HAL_GPIO_WritePin(Reset_Port, Reset_Pin, GPIO_PIN_RESET)
#define DC_HIGH()       HAL_GPIO_WritePin(DC_Port, DC_Pin, GPIO_PIN_SET)
#define DC_LOW()        HAL_GPIO_WritePin(DC_Port, DC_Pin, GPIO_PIN_RESET)

#define CS_HIGH()     HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_SET)
#define CS_LOW()      HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_RESET)

#define LED_HIGH()     //HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET)
#define LED_LOW()      //HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET)
//==============================================================================


SSD1283A::SSD1283A() :
		WIDTH(130),
		HEIGHT(130),
		_width(130),
		_height(130),
		_rotation(2),
		_inversion_bit(0),
		xSemaphore(NULL){
}

void SSD1283A::init(void) {
	xSemaphore = xSemaphoreCreateMutexStatic( &xMutexBuffer );

	RESET_HIGH();
	DC_HIGH();

	CS_HIGH();
	RESET_LOW();
	_delay(2);
	RESET_HIGH();

	setBackLight(false);
	_delay(100);

	  const uint16_t SSD1283A_regValues[] =
	  {
	    0x10, 0x2F8E,
	    0x11, 0x000C,
	    0x07, 0x0021,
	    0x28, 0x0006,
	    0x28, 0x0005,
	    0x27, 0x057F,
	    0x29, 0x89A1,
	    0x00, 0x0001,
	    TFTLCD_DELAY16, 100,
	    0x29, 0x80B0,
	    TFTLCD_DELAY16, 30,
	    0x29, 0xFFFE,
	    0x07, 0x0223,
	    TFTLCD_DELAY16, 30,
	    0x07, 0x0233,
	    0x01, 0x2183,
	    0x03, 0x6830,
	    0x2F, 0xFFFF,
	    0x2C, 0x8000,
	    0x27, 0x0570,
	    0x02, 0x0300,
	    0x0B, 0x580C,
	    0x12, 0x0609,
	    0x13, 0x3100,
	  };

	_init_table16(SSD1283A_regValues, sizeof(SSD1283A_regValues));
	invertDisplay(false);
	fillScreen(WHITE);
}

void SSD1283A::drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	xSemaphoreTake(xSemaphore, portMAX_DELAY);
	if (x < 0) w += x, x = 0;
	if (y < 0) h += y, y = 0;
	if (x + w > _width)  w = _width - x;
	if (y + h > _height) h = _height - y;
	if ((w < 1) || (h < 1)) return;
	_startTransaction();
	_setWindowAddress(x, y, x + w - 1, y + h - 1);
	_writeCommand(0x22);
	_writeData16(color, w * h);
	_endTransaction();
	xSemaphoreGive(xSemaphore);
}

void SSD1283A::drawLineV(int16_t x, int16_t y, int16_t h, uint16_t color) {
	drawRectangle(x, y, 1, h, color);
}

void SSD1283A::drawLineH(int16_t x, int16_t y, int16_t w, uint16_t color) {
	drawRectangle(x, y, w, 1, color);
}

void SSD1283A::fillScreen(uint16_t color) {
	drawRectangle(0, 0, WIDTH, HEIGHT, color);
}

void SSD1283A::drawImage(uint8_t x, uint8_t y, const tImage *image, bool transparent, uint16_t color, uint16_t ncolor) {
	uint8_t value = 0;
	uint8_t x0, y0;
	uint8_t counter = 0;
	const uint8_t *pdata = (const uint8_t*) image->data;

	xSemaphoreTake(xSemaphore, portMAX_DELAY);
	// rows
	for (y0 = 0; y0 < image->height; y0++) {
		// columns
		for (x0 = 0; x0 < image->width; x0++) {
			// load new data
			if (counter == 0) {
				value = *pdata++;
				counter = img_data_block_size;
			}
			counter--;

			if (value & 0x80) // set pixel
				drawPixel(x + x0, y + y0, color);
			else if (!transparent)
				drawPixel(x + x0, y + y0, ncolor);

			value = value << 1;
		}
	}
	xSemaphoreGive(xSemaphore);
}

uint8_t SSD1283A::drawText(uint8_t X, uint8_t Y, const char *str, const tFont &font, bool transparent, uint16_t color, uint16_t ncolor) {
	uint8_t x1 = X;
	while (*str != 0){
		const tChar *ch = _find_char_by_code(*str, font);
		if (ch != 0) {
			drawImage(x1, Y, ch->image, transparent, color, ncolor);
			x1 += ch->image->width;
		}
		str++;
	}
	return x1;
}

uint8_t SSD1283A::drawChar(uint8_t X, uint8_t Y, char c, const tFont &font, bool transparent, uint16_t color, uint16_t ncolor){
	const tChar *ch = _find_char_by_code((int)c, font);
	if (ch == 0) return 0;

	drawImage(X, Y, ch->image, transparent, color, ncolor);
	return X + ch->image->width;
}

uint8_t SSD1283A::lenPixText(const char *str, const tFont &font) {
	uint8_t x = 0;
	while (*str != 0){
		const tChar *ch = _find_char_by_code(*str, font);
		if (ch != 0)  x += ch->image->width;
		str++;
	}
	return x;
}




uint16_t SSD1283A::color565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void SSD1283A::drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h) {
	if (!pcolors)
		return; // canvas might be empty
	int16_t x2, y2; // Lower-right coord
	if ((x >= _width) ||     // Off-edge right
			(y >= _height) ||      // " top
			((x2 = (x + w - 1)) < 0) ||  // " left
			((y2 = (y + h - 1)) < 0))
		return; // " bottom

	int16_t bx1 = 0, by1 = 0; // Clipped top-left within bitmap
	int16_t saveW = w;        // Save original bitmap width value
	if (x < 0) // Clip left
			{
		w += x;
		bx1 = -x;
		x = 0;
	}
	if (y < 0) // Clip top
			{
		h += y;
		by1 = -y;
		y = 0;
	}
	if (x2 >= _width)
		w = _width - x; // Clip right
	if (y2 >= _height)
		h = _height - y; // Clip bottom

	pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
	_startTransaction();
	_setWindowAddress(x, y, x + w - 1, y + h - 1); // Clipped area
	while (h--) // For each (clipped) scanline...
	{
		_writeData16(pcolors, w); // Push one (clipped) row
		pcolors += saveW; // Advance pointer by one full (unclipped) line
	}
	_endTransaction();
}




void SSD1283A::setBackLight(bool lit) {
	if (lit)
		LED_HIGH();
	else
		LED_LOW();
}


//==============================================================================
//private

void SSD1283A::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
		return;
	}
	_startTransaction();
	_setWindowAddress(x, y, x, y);
	_writeData16(color);
	_endTransaction();
}

void SSD1283A::setRotation(uint8_t r) {
	_rotation = r & 3;
	_width = (_rotation & 1) ? HEIGHT : WIDTH;
	_height = (_rotation & 1) ? WIDTH : HEIGHT;
	_startTransaction();
	switch (_rotation) {
	// reg(0x01) bit RL 0x0100 doesn't work
	case 0:
		_writeCommandData16(0x01, _inversion_bit | 0x2183);
		_writeCommandData16(0x03, 0x6830);
		break;
	case 1:
		_writeCommandData16(0x01, _inversion_bit | 0x2283);
		_writeCommandData16(0x03, 0x6808);
		break;
	case 2:
		_writeCommandData16(0x01, _inversion_bit | 0x2183);
		_writeCommandData16(0x03, 0x6800);
		break;
	case 3:
		_writeCommandData16(0x01, _inversion_bit | 0x2283);
		_writeCommandData16(0x03, 0x6838);
		break;
	}
	_endTransaction();
	setWindowAddress(0, 0, _width - 1, _height - 1);
	setVerticalScroll(0, HEIGHT, 0);
}

void SSD1283A::invertDisplay(bool i) {
	_inversion_bit = i ? 0x0800 : 0x0000;
	setRotation(_rotation);
}

void SSD1283A::setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	_startTransaction();
	_setWindowAddress(x1, y1, x2, y2);
	_endTransaction();
}

void SSD1283A::setVerticalScroll(int16_t top, int16_t scrollines, int16_t offset) {
	//int16_t bfa = HEIGHT - top - scrollines;
	int16_t vsp;
	//int16_t sea = top;
	if (offset <= -scrollines || offset >= scrollines) {
		offset = 0; //valid scroll
	}
	vsp = top + offset; // vertical start position
	if (offset < 0) {
		vsp += scrollines;          //keep in unsigned range
	}
	//sea = top + scrollines - 1;
	_writeCommandDataTransaction16(0x41, vsp);
}

// *** private methods
const tChar* SSD1283A::_find_char_by_code(int code, const tFont &font) {
	int count = font.length;
	int first = 0;
	int last = count - 1;
	int mid = 0;

	if (count > 0) {
		if ((code >= font.chars[0].code)
				&& (code <= font.chars[count - 1].code)) {
			while (last >= first) {
				mid = first + ((last - first) / 2);

				if (font.chars[mid].code < code)
					first = mid + 1;
				else if (font.chars[mid].code > code)
					last = mid - 1;
				else
					break;
			}

			if (font.chars[mid].code == code)
				return (&font.chars[mid]);
		}
	}

	return (0);
}

void SSD1283A::_startTransaction() {
	CS_LOW();
}

void SSD1283A::_endTransaction() {
	CS_HIGH();
}

void SSD1283A::_writeCommand(uint8_t cmd) {
	DC_LOW();
	HAL_SPI_Transmit(&SPI_DISP, &cmd, sizeof(cmd), HAL_MAX_DELAY);
	DC_HIGH();
}

void SSD1283A::_writeData(uint8_t data) {
	DC_HIGH();
	HAL_SPI_Transmit(&SPI_DISP, &data, sizeof(data), HAL_MAX_DELAY);
}

void SSD1283A::_writeData16(uint16_t data) {
	DC_HIGH();
	uint16_t d = ((data >> 8) & 0x00FF) | ((data << 8) & 0xFF00);
	HAL_SPI_Transmit(&SPI_DISP, (uint8_t*) &d, sizeof(data), HAL_MAX_DELAY);
}

void SSD1283A::_writeData16(uint16_t data, uint16_t n) {
	DC_HIGH();
	uint16_t d = ((data >> 8) & 0x00FF) | ((data << 8) & 0xFF00);
	//while (n-- > 0) HAL_SPI_Transmit(&SPI_periph, (uint8_t*) &d, sizeof(data), HAL_MAX_DELAY);
	//hdma_spi3_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_spi3_tx.Init.MemInc = DMA_MINC_DISABLE;
	HAL_DMA_Init(&hdma_spi3_tx);
	HAL_SPI_Transmit_DMA(&SPI_DISP, (uint8_t*)&d, n*2);
	while(SPI_DISP.State != HAL_SPI_STATE_READY){
		osDelay(1);
	}
}

void SSD1283A::_writeData16(const uint16_t *data, uint16_t n) {
	DC_HIGH();
	hdma_spi3_tx.Init.MemInc = DMA_MINC_ENABLE;
	//hdma_spi3_tx.Init.MemInc = DMA_MINC_DISABLE;
	HAL_DMA_Init(&hdma_spi3_tx);
	HAL_SPI_Transmit_DMA(&SPI_DISP, (uint8_t*) data, n * 2);
	while (SPI_DISP.State != HAL_SPI_STATE_READY) {
		osDelay(1);
	}
}

void SSD1283A::_writeCommandData16(uint8_t cmd, uint16_t data) {
	DC_LOW();
	HAL_SPI_Transmit(&SPI_DISP, &cmd, sizeof(cmd), HAL_MAX_DELAY);
	DC_HIGH();
	uint16_t d = ((data >> 8) & 0x00FF) | ((data << 8) & 0xFF00);
	HAL_SPI_Transmit(&SPI_DISP, (uint8_t*) &d, sizeof(data), HAL_MAX_DELAY);
}

void SSD1283A::_writeCommandDataTransaction16(uint8_t cmd, uint16_t data) {
	_startTransaction();
	_writeCommandData16(cmd, data);
	_endTransaction();
}

void SSD1283A::_setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	switch (_rotation) {
	case 0:
		_writeCommand(0x44);
		_writeData(x2 + 2);
		_writeData(x1 + 2);
		_writeCommand(0x45);
		_writeData(y2 + 2);
		_writeData(y1 + 2);
		_writeCommand(0x21);
		_writeData(y1 + 2);
		_writeData(x1 + 2);
		//Serial.print("_setWindowAddress "); Serial.print(y1 + 2); Serial.print(", "); Serial.println(x1 + 2);
		break;
	case 1:
		_writeCommand(0x44);
		_writeData(HEIGHT - y1 + 1);
		_writeData(HEIGHT - y2 + 1);
		_writeCommand(0x45);
		_writeData(WIDTH - x1 - 1);
		_writeData(WIDTH - x2 - 1);
		_writeCommand(0x21);
		_writeData(WIDTH - x1 - 1);
		_writeData(HEIGHT - y1 + 1);
		//Serial.print("_setWindowAddress "); Serial.print(WIDTH - x1 - 1); Serial.print(", "); Serial.println(HEIGHT - y1 + 1);
		break;
	case 2:
		_writeCommand(0x44);
		_writeData(WIDTH - x1 + 1);
		_writeData(WIDTH - x2 + 1);
		_writeCommand(0x45);
		_writeData(HEIGHT - y1 + 1);
		_writeData(HEIGHT - y2 + 1);
		_writeCommand(0x21);
		_writeData(HEIGHT - y1 + 1);
		_writeData(WIDTH - x1 + 1);
		//Serial.print("_setWindowAddress "); Serial.print(HEIGHT - y1 + 1); Serial.print(", "); Serial.println(WIDTH - x1 + 1);
		break;
	case 3:
		_writeCommand(0x44);
		_writeData(y2 + 2);
		_writeData(y1 + 2);
		_writeCommand(0x45);
		_writeData(x2);
		_writeData(x1);
		_writeCommand(0x21);
		_writeData(x1);
		_writeData(y1 + 2);
		//Serial.print("_setWindowAddress "); Serial.print(x1); Serial.print(", "); Serial.println(y1 + 2);
		break;
	}
	_writeCommand(0x22);
}

void SSD1283A::_init_table16(const void *table, int16_t size) {
	uint16_t *p = (uint16_t*) table;
	while (size > 0) {
		uint16_t cmd = *p++;
		uint16_t d = *p++;
		if (cmd == TFTLCD_DELAY16) {
			_delay(d);
		} else {
			_writeCommandDataTransaction16(cmd, d);
		}
		size -= 2 * sizeof(int16_t);
	}
}
