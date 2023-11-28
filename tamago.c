#include "tamago.h"

int hardware_setup()
{
	stdio_init_all();
	#if SCREEN
	if(Init_OLED() != 0) {
		return -1;
	}
	OLED_1in5_Init();
	// clearing the screen right after turning it on
	OLED_1in5_Clear();
	#endif

	#if UPS
	if(Init_Battery() != 0) {
		return -1;
	}
	#endif

	// Pico W uses a CYW43 pin to get VBUS so we need to initialise it to monitor battery
    #if BATTERY_MONITOR || BLUETOOTH
    if (cyw43_arch_init() != 0) {
        return -1;
    }
    #endif

	#if BATTERY_MONITOR
	// if (adc_init() != 0){
	// 	return -1;
	// }
	adc_init();
	#endif

	#if BUTTONS
	if(Init_Buttons() != 0) {
		return -1;
	}
	#endif

	#if BUZZER
	if(Init_Buzzer() != 0) {
		return -1;
	}
	#endif

	DEV_Delay_ms(500);
	return 0;
}

int image_init()
{
	// memory allocation for screen image buffer
	UWORD ScreenMemSize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((ScreenImage_ = (UBYTE *)malloc(ScreenMemSize)) == NULL) {
		return -1;
	}

	// init buffer params
	Paint_NewImage(ScreenImage_, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);
	// set scale for 4 bits 16 colors greyscale
	Paint_SetScale(16);
	// reset buffer
	Paint_Clear(BLACK);
	// send buffer to oled screen to print
	// here: not needed since we used the OLED_1in5_Clear() in hardware init
	// OLED_1in5_Display(ScreenImage); 

}

// This is the main loop for core 0
int main() {
	time = to_ms_since_boot(get_absolute_time());
	if(hardware_setup() != 0) {
		return -1;
	}
	image_init();
	tama_init();

	// Main loop

	// OLED_canarticho();
	// buzzTest();
	
	// debug_battery();
	// debug_images();
	debug_overlay();
	// debug_bt();

}


int OLED_canarticho(void)
{	
	Paint_SelectImage(ScreenImage_);
	DEV_Delay_ms(50);
	Paint_Clear(BLACK);

	int frameIndex = 0;

	while (true)
	{
		Paint_DrawBitMap(Duck[frameIndex]);
		OLED_1in5_Display(ScreenImage_);
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
	int time = time_us_32(); //get_absolute_time();
	srand(time);
	int randomSpecies = rand() % (species_nb + 1); // +1 to get full range with modulo (?)
	strcpy(tama_.species, tamaSpecies[randomSpecies]);  
	// initialize tama values
	strcpy(tama_.name, "Zero");
	tama_.hunger = 3;
	tama_.age = 0;
	tama_.iq = 10;
	tama_.sick = 0;
	tama_.happiness = 3;
	// initialize tama sprite, position and movement
	tama_.sprite = (sprite){.frames = tamaSprites[randomSpecies], .xOrig = 0, .yOrig = 32, .goingRight = true};

}


int buzzTest(void)
{
	note_timer_struct noteTimer;
	// Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num = pwm_gpio_to_slice_num(BUZZ);
	noteTimer.slice_num = slice_num;


	play_melody(&noteTimer, HappyBirthday,140);
    // wait until is done
    while(!noteTimer.Done);

	// play_melody(&noteTimer, HarryPotter,144);
    // wait until is done
    // while(!noteTimer.Done);
}

int debug_battery(void)
{
	Paint_SelectImage(ScreenImage_);
	while (1)
	{
		// pass this in a separate file in order to have only tamago specific things here

	// Get battery status
	if (power_source(&battery_status) == PICO_OK) {
		power_str = battery_status ? "BATTERY" : "PLUGGED";
	}

	// Get voltage
	float voltage = 0;
	int voltage_return = power_voltage(&voltage);
	voltage = floorf(voltage * 100) / 100;
	int percent_val = 0;

	// Display power if it's changed
	if (old_battery_status != battery_status || old_voltage != voltage) {
		// will not execute if on battery: find an intelligent way to check battery volatge while plugged
		// if (battery_status && voltage_return == PICO_OK) {
			const float min_battery_volts = 3.0f;
			const float max_battery_volts = 4.2f;
			percent_val = ((voltage - min_battery_volts) / (max_battery_volts - min_battery_volts)) * 100;
		// }

		// Display power and remember old values
		// printf("Power %s, %.2fV%s\n", power_str, voltage, percent_buf);
		Paint_DrawString_EN(0, 0, "Power:", &Font12, 0x8, 0x1);
		Paint_DrawString_EN(44, 0, power_str, &Font12, 0x8, 0x1);
		Paint_DrawNum(0, 13, voltage, &Font12, 2, 0x8, 0x1);
		Paint_DrawNum(0, 26, percent_val, &Font12, 0, 0x8, 0x1);
		old_battery_status = battery_status;
		old_voltage = voltage;
	}
	OLED_1in5_Display(ScreenImage_);
	sleep_ms(1000);
	}
}

int debug_UPS(void)
{
	// Select ScreenImage to be sure
	Paint_SelectImage(ScreenImage_);
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
		
		
		OLED_1in5_Display(ScreenImage_);
		sleep_ms(1000);
		Paint_Clear(BLACK);
	}
}


int debug_overlay(void)
{
	Paint_SelectImage(ScreenImage_);
	add_repeating_timer_ms(2000, spriteMove_callback, NULL, &spriteMoveTimer_);

	// this sets the gpio irq callback, will be the same for the 3 buttons
	gpio_set_irq_enabled_with_callback(LBUTT, GPIO_IRQ_EDGE_RISE , true, &menu_logic);
	gpio_set_irq_enabled(MBUTT, GPIO_IRQ_EDGE_RISE , true);
	gpio_set_irq_enabled(RBUTT, GPIO_IRQ_EDGE_RISE , true);

	Paint_DrawImage(tama_.sprite.frames[frontawkwardFrame], tama_.sprite.xOrig, tama_.sprite.yOrig, 38, 49);
	OLED_1in5_Display(ScreenImage_);

	while (true)
	{
		if (menuToUpdate_ || cursorToUpdate_)
		{
			RefreshMenu();
			OLED_1in5_Display(ScreenImage_);
		}

		if (game_.currentScreen == mainScreen)
		{
			if (iconsToUpdate_)
			{
				RefreshIcons();
				OLED_1in5_Display(ScreenImage_);
			}

			// update sprite if position changed
			if (spriteToUpdate_)
			{
				RefreshSprite();
				OLED_1in5_Display(ScreenImage_);
			}
		}
		
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


	// OLED_1in5_Display_Part(farfetchd, 0,0,56,56);
	// OLED_1in5_Display_Part(farfetchd_gen3, 0,30,64,94);
	// Paint_DrawImage(pascal, 0, 0, 56, 56);
	// Paint_DrawImage(monky2, 0, 0, 64, 64);
	// Paint_DrawImage(farfetchd_gen3, 64, 0, 64, 64);
	// Paint_DrawImage(chips, 0, 64, 24, 24);

	// for (int i = 0; i < 3; i++)
	// { 
	// 	Paint_DrawImage(Mametchi[i], i*39, 0, 38, 49);
	// }
	// for (int i = 5; i < 7; i++)
	// {
	// 	Paint_DrawImage(Mametchi[i], (i-5)*39, 50, 38, 49);
	// }
	
	// Paint_ClearWindows(64, 0, 127, 24, WHITE);
	// Paint_SetRotate(ROTATE_90);
	// Paint_DrawImage(chips, 0, 0, 24, 24);
	// Paint_DrawImage(chips, 1, 25, 24, 24);


	// Paint_ClearWindows(0, 24, 63, 48, WHITE);
	// Paint_DrawImage(chips, 0, 24, 24, 24);
	Paint_DrawNum( 0, 0, 0, &Font8, 0, 0xb, 0x2);
	Paint_DrawNum( 0, 10, 0, &Font8, 1, 0xb, 0x2);
	Paint_DrawNum( 30, 0, -4, &Font8, 0, 0xb, 0x2);
	Paint_DrawNum( 30, 10, 0.3, &Font8, 1, 0xb, 0x2);
	Paint_DrawNum( 60, 0, -4.3, &Font8, 1, 0xb, 0x2);
	Paint_DrawNum( 60, 10, 4.3, &Font8, 1, 0xb, 0x2);

	// Paint_DrawImage(Mametchi[0], 0, 0, 38, 49);
	// Paint_DrawImage(Mametchi[0], 1, 49, 38, 49);
	
	// Paint_DrawImage(farfetchd_gen3, 64, 64, 64, 64);

	// 2x2x4 4 by for
	const unsigned char test[16] = 
	{ 	
		0xf0,0x00,0x00,0x00,0x23,0x78,0xaa,0xaa,
		0xaa,0x78,0x77,0xaa,0x21,0x22,0xa7,0xaa/*,
		0xaa,0x78,0x77,0xaa,0xaa,0x25,0x00,0x00,
		0x21,0x00,0x00,0x00,0x21,0x22,0xa7,0xaa*/
	};

	const unsigned char test32x16[256] = {
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
	};

	const unsigned char test17x16[136]={
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,
	0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0
	};
	// Paint_DrawImage(test17x16, 2, 50, 17, 16);
	// Paint_DrawImage(test17x16, 3, 67, 17, 16);

	// Display memory value associated with color in draw image function:
	const unsigned char *image_buffer = test17x16;
	UWORD xStart = 80;
	UWORD yStart = 80;
	UWORD W_Image = 17;
	UWORD H_Image = 16;

	UWORD x, y;
    int screenWidthBytes = 64;
    int imageWidthBytes = (W_Image%2==0)?(W_Image/2):W_Image/2+1; 
    int imageHeightBytes = H_Image;
	// UWORD w_byte=(W_Image%8)?(W_Image/8)+1:W_Image/8; //this is for 1 bit scaling
    UWORD w_byte=(W_Image%2==0)?(W_Image/2):W_Image/2+1; //this is for 4bits scaling
    // UWORD w_byte=W_Image; //this is for 8bits scaling
    UDOUBLE Addr = 0;
	UDOUBLE pAddr = 0;
	int i =0;
	// Paint_DrawImage(test32x16, 0, 0, 32, 16);
	// Paint_DrawImage(test17x16, 0, 0, 17, 16);
    // for (y = 0; y < H_Image; y++) { // h=16
    //     for (x = 0; x < w_byte; x++) { // w=8+1// w_byte is number of bytes in the image width
    //     	Addr = x + y * w_byte; // address of the pixel in the char array 
	// 		// addr y multiplier needs to be /2 if 4bits => bc 1 char is 2 pixels 
	// 		// pb is si impair width ne marche pas (là on dump juste le double pixel image dans le double pixel ecran)
	// 		// better solution: shift current char if needed
	// 		// (*(image + j*W_Image*2 + i*2+1))<<8 | (*(image + j*W_Image*2 + i*2))
	// 		// this is the color argument given to set pixel
	// 		// on prend le byte impair, shift à gauche de 8 bits puis ou logique pour combiner pair et impair en un uint_16 
	// 		// le uint_16 est écrit dans la mémoire 

	// 		// si 4bits: loop normale y sur H et x sur W
	// 		// Addr dans header = x/2 + y*(W/2:W/2+1)
	// 		// explication: x/2 (troncature) parce que l'indice du header ne doit avancer qu'une fois tous les 2 (2 infos en un char)
	// 		// pour y parce que l'indice hauteur est fait une fois toute la largeur de l'image fait (beware if impair)
	// 		// pAddr reste pareil mais x doit être aussi divisé par 2 (troncature ok, adresses de 0 à 64 et on peut shifter ou non pour avoir le bon pixel)
	// 		pAddr=x+(xStart/2)+((y+yStart)*screenWidthBytes); // address of the pixel on the screen (widthbyte depends on the screen itself, has to be 64)
	// 		Paint.Image[pAddr] = (unsigned char)image_buffer[Addr];
	// 		// Paint_DrawString_EN( 10, 20*i, "waveshare", &Font8, 0x1, 0xb);
	// 		// Paint_DrawNum( 10, 20*i, Addr, &Font8, 1, 0xb, 0x0);
	// 		// Paint_DrawNum( 60, 20*i, pAddr, &Font8, 1, 0xb, 0x0);

	// 		// Paint_DrawNum( 10, 60+20*i, Paint.Image[pAddr], &Font8, 1, 0xb, 0x0);
	// 		// Paint_DrawNum( 60, 60+20*i, image_buffer[Addr], &Font8, 1, 0xb, 0x0);
	// 		i++;
    //     }
	// }
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

void RefreshIcons()
{
    // clearing overlay
	// clear function is shifted of 1 pixel in width if breadboard and height if tama board: bug in clear windows
	Paint_ClearWindows(0, 0, 127, 24, BLACK);
	Paint_ClearWindows(0, 104, 127, 128, BLACK);

	switch (game_.mainCursor)
		{
		case none:
			break;
		case food:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(chips, 4, 104, 24, 24);
			break;
		case play:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(activities, 36, 104, 24, 24);
			break;
		case wash:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(toilet, 68, 104, 24, 24);
			break;
		case heal:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(med, 100, 104, 24, 24);
			break;
		case comm:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(connection, 4, 0, 24, 24);
			break;
		case bedtime:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(lights, 36, 0, 24, 24);
			break;
		case infos:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(informations, 68, 0, 24, 24);
			break;
		case settings:
			Paint_ClearWindows(1, 0, 128, 24, WHITE);
			Paint_ClearWindows(1, 104, 128, 128, WHITE);
			Paint_DrawImage(parameters, 100, 0, 24, 24);
			break;

		}

    iconsToUpdate_ = false;
}

void RefreshSprite()
{
    Paint_DrawRectangle(1, 25, 128, 103, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	Paint_DrawImage(tama_.sprite.frames[fronthappyFrame], tama_.sprite.xOrig, tama_.sprite.yOrig, 38, 49);
    spriteToUpdate_ = false;
}

void RefreshMenu()
{	
	Paint_Clear(BLACK);
	switch (game_.currentScreen)
	{
	case mainScreen:
		RefreshSprite();
		RefreshIcons();
		break;
	case foodScreen:
		for (UBYTE i = 0; i < foodCancel+1; i++)
		{	
			if (game_.foodCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, foodOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, foodOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case playScreen:
		for (UBYTE i = 0; i < playCancel+1; i++)
		{	
			if (game_.playCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, playOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, playOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case washScreen:
		for (UBYTE i = 0; i < washCancel+1; i++)
		{	
			if (game_.washCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, washOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, washOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case healScreen:
		for (UBYTE i = 0; i < healCancel+1; i++)
		{	
			if (game_.healCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, healOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, healOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case commScreen:
		for (UBYTE i = 0; i < commCancel+1; i++)
		{	
			if (game_.commCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, commOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, commOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case bedtimeScreen:
		for (UBYTE i = 0; i < bedtimeCancel+1; i++)
		{	
			if (game_.bedtimeCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, bedtimeOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, bedtimeOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case infosScreen: // todo: find better way to display dynamically tama attributes
		// x position: char width in font12 x char nb in option displayed + fixed 2
		Paint_DrawString_EN(30, 0, tama_.name, &Font12, 0x8, 0x1);
		Paint_DrawNum(30, 13, tama_.age, &Font12, 0, 0x8, 0x1);
		Paint_DrawNum(37, 26, tama_.hunger, &Font12, 0, 0x8, 0x1);
		Paint_DrawNum(58, 39, tama_.happiness, &Font12, 0, 0x8, 0x1);
		Paint_DrawString_EN(51, 52, tama_.species, &Font12, 0x8, 0x1);

		for (UBYTE i = 0; i < infosCancel+1; i++)
		{	
			if (game_.infoCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, infoOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, infoOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	case settingsScreen:
		for (UBYTE i = 0; i < settingsCancel+1; i++)
		{	
			if (game_.settingsCursor == i)
			{
				Paint_DrawString_EN(0, 13*i, settingsOptions[i], &Font12, 0x3, 0xe);
			}else
			{
				Paint_DrawString_EN(0, 13*i, settingsOptions[i], &Font12, 0x8, 0x1);
			}	
		}
		break;
	}
	cursorToUpdate_ = false;
	menuToUpdate_ = false;
}

void menu_logic(uint gpio, uint32_t events) 
{	
	if ((to_ms_since_boot(get_absolute_time())-time)>delayTime) {
	// Recommend to not to change the position of this line
	time = to_ms_since_boot(get_absolute_time());
	
	

    switch (gpio)
    {
    case LBUTT:
        switch (game_.currentScreen)
        {
        case mainScreen:
            if (game_.mainCursor > none)
            {
                game_.mainCursor--;
            }else
            {
                game_.mainCursor = settings;
            }
			iconsToUpdate_ = true;
            break;
        case foodScreen:
            if (game_.foodCursor > 0) // comparing to int instead of name in case order is moved
            {
                game_.foodCursor--;
            }else
            {
                game_.foodCursor = foodCancel;
            }  
			cursorToUpdate_ = true;          
            break;
        case playScreen:
            if (game_.playCursor > 0)
            {
                game_.playCursor--;
            }else
            {
                game_.playCursor = playCancel;
            } 
			cursorToUpdate_ = true;
            break;
        case washScreen:
            if (game_.washCursor > 0)
            {
                game_.washCursor--;
            }else
            {
                game_.washCursor = washCancel;
            } 
			cursorToUpdate_ = true;
            break;
        case healScreen:
            if (game_.healCursor > 0)
            {
                game_.healCursor--;
            }else
            {
                game_.healCursor = healCancel;
            } 
			cursorToUpdate_ = true;
            break;
        case commScreen:
            if (game_.healCursor > 0)
            {
                game_.healCursor--;
            }else
            {
                game_.healCursor = healCancel;
            } 
			cursorToUpdate_ = true;
            break;
        case bedtimeScreen:
            if (game_.bedtimeCursor > 0)
            {
                game_.bedtimeCursor--;
            }else
            {
                game_.bedtimeCursor = bedtimeCancel;
            } 
			cursorToUpdate_ = true;
            break;
        case infosScreen:
            if (game_.infoCursor > 0)
            {
                game_.infoCursor--;
            }else
            {
                game_.infoCursor = infosCancel;
            }
			cursorToUpdate_ = true; 
            break;
        case settingsScreen:
            if (game_.settingsCursor > 0)
            {
                game_.settingsCursor--;
            }else
            {
                game_.settingsCursor = settingsCancel;
            } 
			cursorToUpdate_ = true;
            break;
        }
        break;
    case MBUTT: 
        switch (game_.currentScreen)
        {
        case mainScreen:
            switch (game_.mainCursor)
            {
            case none:
                break;
            case food:
                game_.currentScreen = foodScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case play:
                game_.currentScreen = playScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case wash:
                game_.currentScreen = washScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case heal:
                game_.currentScreen = healScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case comm:
                game_.currentScreen = commScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case bedtime:
                game_.currentScreen = bedtimeScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case infos:
                game_.currentScreen = infosScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            case settings:
                game_.currentScreen = settingsScreen;
                menuToUpdate_ = true; // do this ourselves instead of flagging!
                break;
            }
            break;
        case foodScreen:
            switch (game_.foodCursor)
            {
            case junk:
                // eating mechanic
                break;
            case drink:

                break;
            case foodCancel:
                game_.currentScreen = mainScreen;
				game_.foodCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case playScreen:
            switch (game_.playCursor)
            {
            case hop:
                /* code */
                break;
            case playCancel:
                game_.currentScreen = mainScreen;
				game_.playCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case washScreen:
            switch (game_.washCursor)
            {
            case shower:
                /* code */
                break;
            case washCancel:
                game_.currentScreen = mainScreen;
				game_.washCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case healScreen:
            switch (game_.healCursor)
            {
            case pill:
                /* code */
                break;
            case healCancel:
                game_.currentScreen = mainScreen;
				game_.healCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case commScreen:
            switch (game_.commCursor)
            {
            case search:
                break;
            case commCancel:
                game_.currentScreen = mainScreen;
				game_.commCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case bedtimeScreen:
            switch (game_.bedtimeCursor)
            {
            case light:
                /* code */
                break;
            case bedtimeCancel:
                game_.currentScreen = mainScreen;
				game_.bedtimeCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case infosScreen:
            switch (game_.infoCursor)
            {
            case name:
                /* code */
                break;
            case age:
                break;
            case hunger:
                break;
            case species:
                break;
            case happiness:
                break;
            case infosCancel:
                game_.currentScreen = mainScreen;
				game_.infoCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        case settingsScreen:
            switch (game_.settingsCursor)
            {
            case brightness:
                /* code */
                break;
            case settingsCancel:
                game_.currentScreen = mainScreen;
				game_.settingsCursor = 0;
                menuToUpdate_ = true;
                break;
            }
            break;
        }
        break;
    case RBUTT:
        switch (game_.currentScreen)
        {
        case mainScreen:
            if (game_.mainCursor < settings)
            {
                game_.mainCursor++;
            }else
            {
                game_.mainCursor = none;
            }
			iconsToUpdate_ = true;
            break;
        case foodScreen:
            if (game_.foodCursor < foodCancel)
            {
                game_.foodCursor++;
            }else
            {
                game_.foodCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case playScreen:
            if (game_.playCursor < playCancel)
            {
                game_.playCursor++;
            }else
            {
                game_.playCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case washScreen:
            if (game_.washCursor < washCancel)
            {
                game_.washCursor++;
            }else
            {
                game_.washCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case healScreen:
            if (game_.healCursor < healCancel)
            {
                game_.healCursor++;
            }else
            {
                game_.healCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case commScreen:
            if (game_.commCursor < commCancel)
            {
                game_.commCursor++;
            }else
            {
                game_.commCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case bedtimeScreen:
            if (game_.bedtimeCursor < bedtimeCancel)
            {
                game_.bedtimeCursor++;
            }else
            {
                game_.bedtimeCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case infosScreen:
            if (game_.infoCursor < infosCancel)
            {
                game_.infoCursor++;
            }else
            {
                game_.infoCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        case settingsScreen:
            if (game_.settingsCursor < settingsCancel)
            {
                game_.settingsCursor++;
            }else
            {
                game_.settingsCursor = 0;
            }
			cursorToUpdate_ = true;
            break;
        }
        break;
    } // end switch
	} // end if
}

// Add IR lib?
// Add sprites

/* Game logic: beginning
    instantiate tama (random or use internal clock idk)
    attributes: age, hunger, happiness, sickness (bool?), iq, weight
*/