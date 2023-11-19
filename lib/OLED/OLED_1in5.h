#ifndef __OLED_1IN5_H
#define __OLED_1IN5_H		

#include "../Config/DEV_Config.h" // beware relative path

/********************************************************************************
function:	
		Define the full screen height length of the display
********************************************************************************/

#define OLED_1in5_WIDTH  128 // OLED width
#define OLED_1in5_HEIGHT 128 // OLED height


void OLED_1in5_Init(void);
void OLED_1in5_Clear(void);
void OLED_1in5_Display(UBYTE *Image);
void OLED_1in5_Display_Part(UBYTE *Image, UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend);
void OLED_Scroll(); // use reg commands 0x26/0x27
void OLED_Brightness(); 
void OLED_Contrast();
static void OLED_Reset(void);
static void OLED_WriteReg(uint8_t Reg);
static void OLED_WriteData(uint8_t Data);
static void OLED_InitReg(void);
static void OLED_SetWindow(UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend);


#endif  
	 
