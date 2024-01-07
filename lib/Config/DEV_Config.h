#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_
/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\					Hardware interface							///|
			------------------------------------------------------------------------
***********************************************************************************************************************/

#include "pico/time.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define USE_SPI 1
#define USE_IIC 0
#define IIC_CMD        0X00
#define IIC_RAM        0X40

// defines for hardware config
#define BREADBOARD		0
#define TAMA_BOARD		1

// defines to select which hardware to initialize
#define SCREEN          1
#define UPS             0
#define BUTTONS         1
#define BATTERY_MONITOR 0
#define BUZZER          1
#define BLUETOOTH       0

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

//OLED Define
#define OLED_CS         17 // GPIO 17 (physical pin 22)	is SPI0 CS on the pico	
#define OLED_RST        20 // GPIO 20 (physical pin 26) for reset pin	
#define OLED_DC         21 // GPIO 27 (physical pin 21) for DC pin (data control)
#define OLED_TX         19 // GPIO 19 is spi0 TX (corresponds to MOSI)
#define OLED_CLK        18 // also called SCK, is spi0 clock

#if BREADBOARD
#define BUZZ            14 // GPIO 14 (physical pin 19)	also PWM 7A
#define LBUTT           13 // GPIO 13 (physical pin 17)
#define MBUTT           12
#define RBUTT           11
#endif

#if TAMA_BOARD
#define BUZZ            7 // GPIO 7 is PWM B! not A
#define LBUTT           12 
#define MBUTT           11
#define RBUTT           10
#endif

#define BATT_SDA        6  // GPIO 6 (physical 9)
#define BATT_SDL        7  // GPIO 7 (physical 10)

#define OLED_CS_0      DEV_Digital_Write(OLED_CS,0)
#define OLED_CS_1      DEV_Digital_Write(OLED_CS,1)

#define OLED_RST_0      DEV_Digital_Write(OLED_RST,0)
#define OLED_RST_1      DEV_Digital_Write(OLED_RST,1)

#define OLED_DC_0       DEV_Digital_Write(OLED_DC,0)
#define OLED_DC_1       DEV_Digital_Write(OLED_DC,1)

/*------------------------------------------------------------------------------------------------------*/

UBYTE DEV_ModuleInit(void);
void  DEV_ModuleExit(void);

// Initialize OLED pins and spi communication
UBYTE Init_OLED(void);

// Initialize buttons pins
UBYTE Init_Buttons(void);

// Initialize buzzer pins
UBYTE Init_Buzzer(void);

// Initialize battery pins
UBYTE Init_Battery(void);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);
void DEV_Delay_ms(UDOUBLE xms);

void I2C_Write_Byte(uint8_t value, uint8_t Cmd);
void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

#endif
