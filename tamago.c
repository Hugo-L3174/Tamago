#include "tamago.h"

int main() {

    //OLED_1in5_test();
	//OLED_pic();
	OLED_canarticho();
}

//todo: use Paint_DrawBitMap and Paint_DrawBitMapBlock from GUI_Paint

int OLED_pic(void)
{
	if(DEV_ModuleInit() != 0) {
		return -1;
	}
	OLED_1in5_Init();
	DEV_Delay_ms(500);	

	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
		return -1;
	}

	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);	
	Paint_SetScale(16);
	// 1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

	while (true)
	{
	// 2.Drawing on the image
	Paint_DrawBitMap(can128rgb1);

	// 3.Show image 
	OLED_1in5_Display(BlackImage);
	DEV_Delay_ms(2000);	
	Paint_Clear(BLACK);	

	
	/*Paint_DrawBitMap(test2);
	OLED_1in5_Display(BlackImage);
	DEV_Delay_ms(2000);	
	Paint_Clear(BLACK);	
	Paint_DrawBitMap(test3);
	OLED_1in5_Display(BlackImage);
	DEV_Delay_ms(2000);	
	Paint_Clear(BLACK);	
	Paint_DrawBitMap(test4);
	OLED_1in5_Display(BlackImage);
	DEV_Delay_ms(2000);	
	Paint_Clear(BLACK);	*/

	

	}

	
}


int OLED_canarticho(void)
{	
	if(DEV_ModuleInit() != 0) {
		return -1;
	}
	OLED_1in5_Init();
	DEV_Delay_ms(500);	

	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
		return -1;
	}

	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);	
	Paint_SetScale(16);
	// 1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

	while (true)
	{
		Paint_DrawBitMap(can128rgb1);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb2);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb3);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb4);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb5);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb6);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb7);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb8);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb9);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb10);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb11);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb12);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb13);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb14);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb15);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);

		Paint_DrawBitMap(can128rgb16);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb17);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		Paint_DrawBitMap(can128rgb18);
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);		
	}
}

int OLED_1in5_test(void)
{
	if(DEV_ModuleInit() != 0) {
		return -1;
	}
	OLED_1in5_Init();
	DEV_Delay_ms(500);	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			return -1;
	}
	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);	
	Paint_SetScale(16);
	//1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		
		// 2.Drawing on the image		
		Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);			
		Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);		
		// 3.Show image on page1
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(2000);			
		Paint_Clear(BLACK);

		// Drawing on the image
		for(UBYTE i=0; i<16; i++){
			Paint_DrawRectangle(0, 8*i, 127, 8*(i+1), i, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		}			
		// Show image on page2
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(2000);	
		Paint_Clear(BLACK);	
		
		// Drawing on the image			
		Paint_DrawString_EN(10, 0, "waveshare", &Font16, 0x1, 0xb);
		Paint_DrawString_EN(10, 17, "hello world", &Font8, 0x2, 0xc);
		Paint_DrawNum(10, 30, 123.456789, &Font8, 4, 0x3, 0xd);
		Paint_DrawNum(10, 43, 987654, &Font12, 5, 0x4, 0xe);
		// Show image on page2
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(2000);	
		Paint_Clear(BLACK);		
			

		// Drawing on the image
		/*if (GUI_ReadBmp_16Gray("./pic/1in5.bmp", 0, 0) == 0)
		{
			// Show image on page4
			OLED_1in5_Display(BlackImage);
		}
		
		DEV_Delay_ms(2000);		
		Paint_Clear(BLACK);	*/

		OLED_1in5_Clear();
	}
}


// Add right oled lib
// Add IR lib?
// Add buttons
// Add sprites

/* Game logic: beginning
    instantiate tama (random or use internal clock idk)
    attributes: age, hunger, happiness, sickness (bool?), iq, weight
*/