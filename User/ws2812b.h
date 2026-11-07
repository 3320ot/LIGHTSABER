#ifndef WS2812B_H_
#define WS2812B_H_

#include "debug.h"

#define NUM_LEDS         264
#define BUF_SIZE         NUM_LEDS * 3 * 3   // 3 bytes per led x 3x encoding

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

typedef struct {
    uint8_t g; // Green
    uint8_t r; // Red
    uint8_t b; // Blue
} RGB;

void WS2812bInit();

void WS2812bFillSolid(RGB color);

void WS2812bClear();

void WS2812bSetLed(u16 index, RGB color);

void WS2812_Send();

#endif /* WS2812B_H_ */
