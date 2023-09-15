#include "tamago.h"

int hardware_setup()
{
	stdio_init_all();

	if(Init_OLED() != 0) {
		return -1;
	}
	OLED_1in5_Init();
	// clearing the screen right after turning it on
	OLED_1in5_Clear();

	// if(Init_Battery() != 0) {
	// 	return -1;
	// }

	if(Init_Buttons() != 0) {
		return -1;
	}

	// if(Init_Buzzer() != 0) {
	// 	return -1;
	// }

	DEV_Delay_ms(500);
	return 0;
}

int image_init()
{
	// memory allocation for screen image buffer
	if((ScreenImage = (UBYTE *)malloc(ScreenSize)) == NULL) {
		return -1;
	}

	// init buffer params
	Paint_NewImage(ScreenImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);
	// set scale for 4 bits 16 colors greyscale
	Paint_SetScale(16);
	// reset buffer
	Paint_Clear(BLACK);
	// send buffer to oled screen to print
	// here: not needed since we used the OLED_1in5_Clear() in hardware init
	// OLED_1in5_Display(ScreenImage); 

}


int main() {

	if(hardware_setup() != 0) {
		return -1;
	}
	image_init();
	tama_init();

	// Main loop

	// debug_buttons();

	// OLED_canarticho();
	// buzzTest();
	// debug_battery();
	
	// debug_images();
	debug_overlay();

	
}


int OLED_canarticho(void)
{	
	Paint_SelectImage(ScreenImage);
	DEV_Delay_ms(50);
	Paint_Clear(BLACK);

	int frameIndex = 0;

	while (true)
	{
		Paint_DrawBitMap(c4nar[frameIndex]);
		OLED_1in5_Display(ScreenImage);
		DEV_Delay_ms(100);	
		Paint_Clear(BLACK);	

		if (gpio_get(RBUTT) && frameIndex < 16)
		{
			frameIndex++;
		}
		else if (gpio_get(RBUTT) && frameIndex == 16)
		{
			frameIndex = 0;
		}
		else if (gpio_get(LBUTT) && frameIndex > 0)
		{
			frameIndex--;
		}
		else if (gpio_get(LBUTT) && frameIndex == 0)
		{
			frameIndex = 16;
		}
		

	}
}

int tama_init(void)
{
	// initialize random species
	int time = get_absolute_time();
	srand(time);
	tama.type = rand() % (species_nb - 1); 
	// from the species, create the image loop of the tama pet?

}


int buzzTest(void)
{
	note_timer_struct noteTimer;
	// Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num = pwm_gpio_to_slice_num(BUZZ);
	noteTimer.slice_num = slice_num;


	// play_melody(&noteTimer, HappyBirday,140);
    // // wait until is done
    // while(!noteTimer.Done);

	play_melody(&noteTimer, HarryPotter,144);
    // wait until is done
    while(!noteTimer.Done);
}


int debug_battery(void)
{
	// Select ScreenImage to be sure
	Paint_SelectImage(ScreenImage);
	DEV_Delay_ms(50);
	Paint_Clear(BLACK);

	float bus_voltage = 0;
	float shunt_voltage = 0;
	float power = 0;
	float current = 0;
	float P=0;
	uint16_t value;

	setCalibration_32V_2A();
	double testnb = -765.25;

	while (true)
	{	
		
		bus_voltage = getBusVoltage_V();         // voltage on V- (load side)
		current = getCurrent_mA()/1000;               // current in mA
		P = (bus_voltage -3)/1.2*100;

		if(P<0)
		{
			P=0;
		}
		else if (P>100)
		{
			P=100;
		}

		Paint_DrawNum(10, 0, bus_voltage, &Font12, 3, 0x1, 0xb);
		// not working because wrong function when negative numbers
		Paint_DrawNum(10, 15, current, &Font12, 3, 0x1, 0xb);
		Paint_DrawNum(10, 30, P, &Font12, 1, 0x1, 0xb);
		Paint_DrawNum(10, 45, testnb, &Font12, 3, 0x1, 0xb);
		
		
		OLED_1in5_Display(ScreenImage);
		sleep_ms(1000);
		Paint_Clear(BLACK);
	}
}

int debug_buttons(void)
{
	Paint_SelectImage(ScreenImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

	// Paint_DrawNum(10, 0, tama.type, &Font16, 4, 0x1, 0xb);
	// OLED_1in5_Display(BlackImage);
	while (true)
	{	
		if (gpio_get(RBUTT))
		{
			Paint_DrawString_EN(10, 0, "right", &Font16, 0x1, 0xb);
		}
		if (gpio_get(MBUTT))
		{
			Paint_DrawString_EN(10, 0, "middle", &Font16, 0x1, 0xb);
		}
		if (gpio_get(LBUTT))
		{
			Paint_DrawString_EN(10, 0, "left", &Font16, 0x1, 0xb);
		}
		
		OLED_1in5_Display(ScreenImage);
		Paint_Clear(BLACK);
	}

}

int debug_overlay(void)
{
	Paint_SelectImage(ScreenImage);

	while (true)
	{
		if (gpio_get(RBUTT) && cursor < 8)
		{
			cursor++;
		}
		else if (gpio_get(RBUTT) && cursor == 8)
		{
			cursor = zero;
		}
		else if (gpio_get(LBUTT) && cursor > 0)
		{
			cursor--;
		}
		else if (gpio_get(LBUTT) && cursor == 0)
		{
			cursor = i;
		}

		// clearing overlay
		Paint_DrawRectangle(1, 1, 128, 21, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		Paint_DrawRectangle(1, 108, 128, 128, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		

		// we divide top and bottom in 4 each
		// screen is 128 --> 32 for each icon, starting at 0, 32, 64, 98
		// for debug, chars with font20: 14*20 so to be in middle start at 9, 41, 75, 107
		// /!\ bug probably coming from the way rectangles are drawn: drawChar and drawRectangle both use 1-128 instead of 0-127, + seems to be a border of 1pixl ?
		// --> todo: find a better way to clear and draw than rectangles, probably for{}+setPixel
		switch (cursor)
		{
		case zero:
			break;
		case infos:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(9, 107, 'b', &Font20, 0xe, 0x3);
			break;
		case c:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(41, 107, 'c', &Font20, 0xe, 0x3);
			break;
		case d:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(75, 107, 'd', &Font20, 0xe, 0x3);
			break;
		case e:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(107, 107, 'e', &Font20, 0xe, 0x3);
			break;
		case f:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(9, 0, 'f', &Font20, 0xe, 0x3);
			break;
		case g:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(41, 0, 'g', &Font20, 0xe, 0x3);
			break;
		case h:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(75, 0, 'h', &Font20, 0xe, 0x3);
			break;
		case i:
			Paint_DrawRectangle(1, 1, 128, 21, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawRectangle(1, 108, 128, 128, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
			Paint_DrawChar(107, 0, 'i', &Font20, 0xe, 0x3);
			break;

		}

		OLED_1in5_Display(ScreenImage);
		sleep_ms(100);
	}
	

}

int debug_images(void)
{

	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) { //No enough memory
      return -1;
  	}
  	// Paint_NewImage(BlackImage, OLED_1in5_WIDTH/4, OLED_1in5_HEIGHT/2, 0, BLACK);
	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);  
	// UWORD Imagesize = 128*128;
	// if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
	// 	return -1;
	// }

	// Paint_NewImage(BlackImage, 128, 128, 0, BLACK);

	Paint_SetScale(16); // this is setting 16 bit color greyscale (1.5 oled)
	// 1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

	OLED_1in5_Display(BlackImage);


	// OLED_1in5_Display(pokemon);

	// OLED_1in5_Display_Part(farfetchd, 0,0,56,56);
	// OLED_1in5_Display_Part(farfetchd_gen3, 0,30,64,94);
	
	Paint_DrawImage(farfetchd_gen3, 0, 0, 64, 64);
	Paint_DrawImage(farfetchd_gen3, 64, 0, 64, 64);
	Paint_DrawImage(farfetchd_gen3, 0, 64, 64, 64);
	Paint_DrawImage(farfetchd_gen3, 64, 64, 64, 64);
	OLED_1in5_Display(BlackImage);


}

int OLED_1in5_test(void)
{
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

// Add IR lib?
// Add sprites

/* Game logic: beginning
    instantiate tama (random or use internal clock idk)
    attributes: age, hunger, happiness, sickness (bool?), iq, weight
*/