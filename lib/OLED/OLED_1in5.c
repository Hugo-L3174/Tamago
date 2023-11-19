#include "OLED_1in5.h"
#include "stdio.h"

/*******************************************************************************
function:
            Hardware reset
*******************************************************************************/
static void OLED_Reset(void)
{
    OLED_RST_1;
    DEV_Delay_ms(100);
    OLED_RST_0;
    DEV_Delay_ms(100);
    OLED_RST_1;
    DEV_Delay_ms(100);
}

/*******************************************************************************
function:
            Write register address and data
*******************************************************************************/
static void OLED_WriteReg(uint8_t Reg)
{
#if USE_SPI
    OLED_DC_0;
    OLED_CS_0;
    DEV_SPI_WriteByte(Reg);
#elif USE_IIC
    I2C_Write_Byte(Reg,IIC_CMD);
#endif
}

static void OLED_WriteData(uint8_t Data)
{   
#if USE_SPI
    OLED_DC_1;
    OLED_CS_0;
    DEV_SPI_WriteByte(Data);
    OLED_CS_1;
#elif USE_IIC
    I2C_Write_Byte(Data,IIC_RAM);
#endif
}

/*******************************************************************************
function:
        Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{
    OLED_WriteReg(0xae);  // turn off oled panel

    #if BREADBOARD
    OLED_WriteReg(0x15);  // set column address
    OLED_WriteReg(0x00);  // start column   0
    OLED_WriteReg(0x7f);  // end column   127

    OLED_WriteReg(0x75);  // set row address
    OLED_WriteReg(0x00);  // start row   0
    OLED_WriteReg(0x7f);  // end row   127

    OLED_WriteReg(0xa0);  // segment remap
    OLED_WriteReg(0x51);  // 01010001 
                          // bits 0 and 1 define column remapping
                          // bit 2 defines adress increment mode (horizontal/vertical)
                          // bits 4 and 6 define com remapping
    #endif

    #if TAMA_BOARD
    // screen is flipped so row and columns + iterations need to be flipped
    OLED_WriteReg(0x15);  // set column address
    OLED_WriteReg(0x00);  // start column   0
    OLED_WriteReg(0x7f);  // end column   127

    OLED_WriteReg(0x75);  // set row address
    OLED_WriteReg(0x00);  // start row   0
    OLED_WriteReg(0x7f);  // end row   127

    OLED_WriteReg(0xa0);  // segment remap
    OLED_WriteReg(0x42);  // 01000010
    // flipping bits 1 and 2 (adress remap set to 0 and nibble remap set to 1) mirrors vertically
    // flipping bit 4 to 0 disables com remap
    #endif

    OLED_WriteReg(0x81);  // set contrast control
    OLED_WriteReg(0x80);  // 0x80 by default, the higher the contrast val the lower the brightness and saturation

    OLED_WriteReg(0xa1);  // start line
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa2);  // display offset
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa4);  // normal display mode (options are normal: a4, all on: a5, all off: a6, negative/inverted: a7)

    OLED_WriteReg(0xa8);  // set multiplex ratio
    OLED_WriteReg(0x7f);

    OLED_WriteReg(0xb1);  // set phase length of display phase 1 (discharge of capacitance to reset pixel) and phase 2 (pre charge of capacitors)
    OLED_WriteReg(0x21);  // first 4 bits are display phase 1, last 4 are display phase 2

    OLED_WriteReg(0xb6);  // set phase length of display phase 3 (second pre charge of pixel: speed of charging process)
    OLED_WriteReg(0x0f);  

    // here we could set reg 0xb8/b9 to set gamma setting (pulse width)
    // higher value means wider pulse width so brighter pixels
    // there are 16 presets of grayscale level
    // b8 is to make custom presets (16 next commands sent), b9 is to set to default presets
    OLED_WriteReg(0xb9);

    OLED_WriteReg(0xb3);  // set dclk :  low freq: may flicker but lower energy consumption
    OLED_WriteReg(0xc1);  // first 4 bits are clock divide ratio , last 4 bits are oscillator frequency
                          // 80Hz:0xc1 90Hz:0xe1 100Hz:0x00  110Hz:0x30 120Hz:0x50 130Hz:0x70 

    OLED_WriteReg(0xab);  // function selection A: bit 0 is use of internal voltage regulator
    OLED_WriteReg(0x01);  // 1 -> enable internal (0 would be external)

    OLED_WriteReg(0xbe);  // set COM deselect voltage level
    OLED_WriteReg(0x0f);  // max value should be 0x07 = 0.86*Vcc

    OLED_WriteReg(0xbc);  // set precharge voltage level
    OLED_WriteReg(0x08);  // set to Vcomh (below are multiples of Vcc)

    OLED_WriteReg(0xd5);  // function selection B
    OLED_WriteReg(0x62);  // only first 2 bits should have an effect? enable or not second precharge, and internal or external VSL

    OLED_WriteReg(0xfd);  // MCU protection
    OLED_WriteReg(0x12);  // only 3rd bit has an effect, lock or unlock memory commands

}

/********************************************************************************
function:
            initialization
********************************************************************************/
void OLED_1in5_Init(void)
{
    //Hardware reset
    OLED_Reset();

    //Set the initialization register
    OLED_InitReg();
    DEV_Delay_ms(200);

    //Turn on the OLED display
    OLED_WriteReg(0xaf);
    
}

/********************************************************************************
function:   Set the display Window(Xstart, Ystart, Xend, Yend)
parameter:
        xStart :   X direction Start coordinates
        Ystart :   Y direction Start coordinates
        Xend   :   X direction end coordinates
        Yend   :   Y direction end coordinates
********************************************************************************/
static void OLED_SetWindow(UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend)
{
    if((Xstart > OLED_1in5_WIDTH) || (Ystart > OLED_1in5_HEIGHT) ||
       (Xend > OLED_1in5_WIDTH) || (Yend > OLED_1in5_HEIGHT))
        return;

    OLED_WriteReg(0x15);
    OLED_WriteReg(Xstart/2);
    OLED_WriteReg(Xend/2 - 1);

    OLED_WriteReg(0x75);
    OLED_WriteReg(Ystart);
    OLED_WriteReg(Yend - 1);
}

/********************************************************************************
function:
            Clear screen
********************************************************************************/
void OLED_1in5_Clear(void)
{
    UWORD i;
    OLED_SetWindow(0, 0, 128, 128);
    for(i=0; i<OLED_1in5_WIDTH*OLED_1in5_HEIGHT/2; i++){
        OLED_WriteData(0x00);
    }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_1in5_Display(UBYTE *Image)
{
    UWORD x, y, temp;
    OLED_SetWindow(0, 0, 128, 128);
    for(y=0; y<OLED_1in5_HEIGHT; y++)
        for(x=0; x<OLED_1in5_WIDTH/2; x++)
        {
            // Does this interfere with 8bit display?
            temp = Image[x + y*64]; // writing data for desired pixel, adress is bc Xcoordinate + Ycoord*screen width (rows completed)
            OLED_WriteData(temp);
        }
}


void OLED_1in5_Display_Part(UBYTE *Image, UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend)
{
    //this function is still not working for xstart /=0
    UWORD x, y, temp;
    UWORD imgWidth = Xend-Xstart;
    // screen is addressed using 1 byte for 1 pixel in height and 1 byte for 2 pixels in width
    // this means we need to divide width by 2 in address
    UWORD widthByte = ((Xend-Xstart)%2==0) ? ((Xend-Xstart)/2) : ((Xend-Xstart)/2+1); 
    UWORD imgHeight = Yend - Ystart;
    UWORD heightByte = Yend - Ystart;

    UWORD XstartOffset = (Xstart%2==0) ? (Xstart/2) : (Xstart/2+1); //this is a bug : if 
    UWORD YstartOffset = Ystart;
    
    OLED_SetWindow(Xstart, Ystart, Xend, Yend);
    for(y=0; y<Yend-Ystart; y++)
        for(x=0; x < widthByte; x++)
        {
            temp = Image[x + y*widthByte]; //(XstartOffset + YstartOffset*widthByte) +
            OLED_WriteData(temp);
        }
        // for(x=0; x < Xend-Xstart; x++)
        // {
        //     temp = Image[x + y*widthByte]; //(XstartOffset + YstartOffset*widthByte) +
        //     OLED_WriteData(temp);
        // }
}
