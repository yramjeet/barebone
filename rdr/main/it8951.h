#pragma once
#include <stdint.h>
#include <stdbool.h>

// PIN DEFINITION
#define IT8951_SPI_HOST SPI3_HOST
#define IT8951_MISO     18
#define IT8951_MOSI     17
#define IT8951_CLK      16
#define IT8951_CS       15
#define IT8951_RST      7
#define IT8951_HRDY     6

typedef struct
{
 uint16_t PanelW;
 uint16_t PanelH;
 uint16_t ImgBufAddrL;
 uint16_t ImgBufAddrH;
 uint16_t FWVersion[8]; //16 Bytes String
 uint16_t LUTVersion[8]; //16 Bytes String
} DeviceInfo;

#define IT8951_CMD_GET_DEV_INFO   0x0302


// typedef unsigned char TByte; //1 byte
// typedef unsigned short TWord; //2 bytes
// typedef unsigned long TDWord; //4 bytes

// #define IT8951_CMD_MEM_BST_WR    0x0011
// #define IT8951_CMD_DISPLAY_AREA  0x0034

// #define IT8951_PIXEL_WHITE 0xFF
// #define IT8951_PIXEL_BLACK 0x00

bool it8951_init(void);
// void it8951_deinit(void);
// bool it8951_clear_screen(void);