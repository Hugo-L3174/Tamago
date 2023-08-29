/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2020-06-17
* | Info        :   Basic version
*
******************************************************************************/
#include "DEV_Config.h"
#include <unistd.h>

uint32_t fd;
/*****************************************
                GPIO
*****************************************/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_put(Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    UBYTE Read_value = 0;
    Read_value = gpio_get(Pin);
    return Read_value;
}

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
#ifdef USE_BCM2835_LIB  
    if(Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT){
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
    }else {
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
    }
#elif USE_WIRINGPI_LIB
    if(Mode == 0 || Mode == INPUT){
        pinMode(Pin, INPUT);
        pullUpDnControl(Pin, PUD_UP);
    }else{ 
        pinMode(Pin, OUTPUT);
        // printf (" %d OUT \r\n",Pin);
    }
#elif USE_DEV_LIB
    SYSFS_GPIO_Export(Pin);
    if(Mode == 0 || Mode == SYSFS_GPIO_IN){
        SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_IN);
        // printf("IN Pin = %d\r\n",Pin);
    }else{
        SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_OUT);
        // printf("OUT Pin = %d\r\n",Pin);
    }
#endif
    gpio_init(Pin);
	if(Mode == 0 || Mode == GPIO_IN) {
		gpio_set_dir(Pin, GPIO_IN);
	} else {
		gpio_set_dir(Pin, GPIO_OUT);
	}
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
    sleep_ms(xms);
}

static void DEV_GPIO_Init(void)
{
    // Init oled pins
    DEV_GPIO_Mode(OLED_CS, GPIO_OUT);
    DEV_GPIO_Mode(OLED_RST, GPIO_OUT);
    DEV_GPIO_Mode(OLED_DC, GPIO_OUT);

	DEV_Digital_Write(OLED_CS, GPIO_OUT);
    DEV_Digital_Write(OLED_DC, GPIO_IN);

    // Init buttons pins
    DEV_GPIO_Mode(LBUTT, GPIO_IN);
    gpio_pull_down(LBUTT);
    DEV_GPIO_Mode(MBUTT, GPIO_IN);
    gpio_pull_down(MBUTT);
    DEV_GPIO_Mode(RBUTT, GPIO_IN);
    gpio_pull_down(RBUTT);

    // Init battery monitoring pins
    gpio_pull_up(BATT_SDA);
    gpio_pull_up(BATT_SDL);
    
}

UBYTE Init_OLED()
{
    // Init oled pins
    DEV_GPIO_Mode(OLED_CS, GPIO_OUT);
    DEV_GPIO_Mode(OLED_RST, GPIO_OUT);
    DEV_GPIO_Mode(OLED_DC, GPIO_OUT);

	DEV_Digital_Write(OLED_CS, GPIO_OUT);
    DEV_Digital_Write(OLED_DC, GPIO_IN);

    // Init spi
    spi_init(spi0, 10000*1000); // Baudrate set to 48kHz
    gpio_set_function(OLED_CLK, GPIO_FUNC_SPI);
    gpio_set_function(OLED_TX, GPIO_FUNC_SPI);
    return 0;
}

UBYTE Init_Buttons()
{
    // Init buttons pins
    DEV_GPIO_Mode(LBUTT, GPIO_IN);
    gpio_pull_down(LBUTT);
    DEV_GPIO_Mode(MBUTT, GPIO_IN);
    gpio_pull_down(MBUTT);
    DEV_GPIO_Mode(RBUTT, GPIO_IN);
    gpio_pull_down(RBUTT);
    return 0;
}

UBYTE Init_Buzzer()
{
    gpio_set_function(BUZZ, GPIO_FUNC_PWM);
    return 0;
}

UBYTE Init_Battery()
{
    // Init battery monitoring pins
    gpio_pull_up(BATT_SDA);
    gpio_pull_up(BATT_SDL);

    // Init i2c for it
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(BATT_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BATT_SDL, GPIO_FUNC_I2C);
    return 0;
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{
    stdio_init_all();
    DEV_GPIO_Init();
    spi_init(spi0, 10000*1000); // Baudrate set to 48kHz
    gpio_set_function(OLED_CLK, GPIO_FUNC_SPI);
    gpio_set_function(OLED_TX, GPIO_FUNC_SPI);
    gpio_set_function(BUZZ, GPIO_FUNC_PWM);

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(BATT_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BATT_SDL, GPIO_FUNC_I2C);
    return 0;
}

void DEV_SPI_WriteByte(uint8_t Value)
{
#ifdef USE_BCM2835_LIB
    bcm2835_spi_transfer(Value);
    
#elif USE_WIRINGPI_LIB
    wiringPiSPIDataRW(0,&Value,1);
    
#elif USE_DEV_LIB
	// printf("write data is %d\r\n", Value);
    DEV_HARDWARE_SPI_TransferByte(Value);
    
#endif
    spi_write_blocking(spi0, &Value, 1);
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
	//printf("data is %s", pData);
#ifdef USE_BCM2835_LIB
    char rData[Len];
    bcm2835_spi_transfernb(pData,rData,Len);
    
#elif USE_WIRINGPI_LIB
    wiringPiSPIDataRW(0, pData, Len);
    
#elif USE_DEV_LIB
    DEV_HARDWARE_SPI_Transfer(pData, Len);
    
#endif
    spi_write_blocking(spi0, pData, Len);
}

void I2C_Write_Byte(uint8_t value, uint8_t Cmd)
{
#ifdef USE_BCM2835_LIB
    char wbuf[2]={Cmd, value};
    bcm2835_i2c_write(wbuf, 2);
#elif USE_WIRINGPI_LIB
	int ref;
	//wiringPiI2CWrite(fd,Cmd);
    ref = wiringPiI2CWriteReg8(fd, (int)Cmd, (int)value);
    while(ref != 0) {
        ref = wiringPiI2CWriteReg8 (fd, (int)Cmd, (int)value);
        if(ref == 0)
            break;
    }
#elif USE_DEV_LIB
    char wbuf[2]={Cmd, value};
    DEV_HARDWARE_I2C_write(wbuf, 2);

#endif
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
#ifdef USE_BCM2835_LIB
    bcm2835_spi_end();
	bcm2835_i2c_end();
    bcm2835_close();


#elif USE_WIRINGPI_LIB
    OLED_CS_0;
	OLED_RST_1;
	OLED_DC_0;

#elif USE_DEV_LIB
    OLED_CS_0;
	OLED_RST_1;
	OLED_DC_0;
    DEV_HARDWARE_SPI_end();
    DEV_HARDWARE_I2C_end();
#endif
    spi_deinit(spi0);
}

