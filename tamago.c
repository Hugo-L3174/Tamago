#include "tamago.h"

int main() {
	/*stdio_init_all();
	spi_init(spi0, 10 * 1000 * 1000);
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

	gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

	//Hardware reset
    OLED_RST_1;
    sleep_ms(100);
    OLED_RST_0;
    sleep_ms(100);
    OLED_RST_1;
    sleep_ms(100);

    //Set the initialization register
    //OLED_InitReg();
	DEV_SPI_WriteByte(0xae);//--turn off oled panel

    DEV_SPI_WriteByte(0x15);    //   set column address
    DEV_SPI_WriteByte(0x00);    //  start column   0
    DEV_SPI_WriteByte(0x7f);    //  end column   127

    DEV_SPI_WriteByte(0x75);    //   set row address
    DEV_SPI_WriteByte(0x00);    //  start row   0
    DEV_SPI_WriteByte(0x7f);    //  end row   127

    DEV_SPI_WriteByte(0x81);  // set contrast control
    DEV_SPI_WriteByte(0x80);

    DEV_SPI_WriteByte(0xa0);    // gment remap
    DEV_SPI_WriteByte(0x51);   //51

    DEV_SPI_WriteByte(0xa1);  // start line
    DEV_SPI_WriteByte(0x00);

    DEV_SPI_WriteByte(0xa2);  // display offset
    DEV_SPI_WriteByte(0x00);

    DEV_SPI_WriteByte(0xa4);    // rmal display
    DEV_SPI_WriteByte(0xa8);    // set multiplex ratio
    DEV_SPI_WriteByte(0x7f);

    DEV_SPI_WriteByte(0xb1);  // set phase leghth
    DEV_SPI_WriteByte(0xf1);

    DEV_SPI_WriteByte(0xb3);  // set dclk
    DEV_SPI_WriteByte(0x00);  //80Hz:0xc1 90Hz:0xe1   100Hz:0x00   110Hz:0x30 120Hz:0x50   130Hz:0x70     01

    DEV_SPI_WriteByte(0xab);  //
    DEV_SPI_WriteByte(0x01);  //

    DEV_SPI_WriteByte(0xb6);  // set phase leghth
    DEV_SPI_WriteByte(0x0f);

    DEV_SPI_WriteByte(0xbe);
    DEV_SPI_WriteByte(0x0f);

    DEV_SPI_WriteByte(0xbc);
    DEV_SPI_WriteByte(0x08);

    DEV_SPI_WriteByte(0xd5);
    DEV_SPI_WriteByte(0x62);

    DEV_SPI_WriteByte(0xfd);
    DEV_SPI_WriteByte(0x12);
    sleep_ms(200);

    //Turn on the OLED display
	DEV_SPI_WriteByte(0xAF);
	sleep_ms(1000*60);*/

    /*const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        // Blink faster! (this is the only line that's modified)
        sleep_ms(25);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }*/
    OLED_1in5_test();
}

int OLED_1in5_test(void)
{
	printf("1.5inch OLED test demo\n");
	if(DEV_ModuleInit() != 0) {
		return -1;
	}
	  
	printf("OLED Init...\r\n");
	OLED_1in5_Init();
	DEV_Delay_ms(500);	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);	
	Paint_SetScale(16);
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		
		// 2.Drawing on the image		
		printf("Drawing:page 1\r\n");
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
		printf("Drawing:page 2\r\n");
		for(UBYTE i=0; i<16; i++){
			Paint_DrawRectangle(0, 8*i, 127, 8*(i+1), i, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		}			
		// Show image on page2
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(2000);	
		Paint_Clear(BLACK);	
		
		// Drawing on the image
		printf("Drawing:page 3\r\n");			
		Paint_DrawString_EN(10, 0, "waveshare", &Font16, 0x1, 0xb);
		Paint_DrawString_EN(10, 17, "hello world", &Font8, 0x2, 0xc);
		Paint_DrawNum(10, 30, 123.456789, &Font8, 4, 0x3, 0xd);
		Paint_DrawNum(10, 43, 987654, &Font12, 5, 0x4, 0xe);
		// Show image on page2
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(2000);	
		Paint_Clear(BLACK);		
			

		// Drawing on the image
		printf("Drawing:page 5\r\n");
		GUI_ReadBmp_16Gray("./pic/1in5.bmp", 0, 0);
		// Show image on page4
		OLED_1in5_Display(BlackImage);
		DEV_Delay_ms(2000);		
		Paint_Clear(BLACK);	

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