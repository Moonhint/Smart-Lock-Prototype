// LCD = 128 x 64
// TFT = 320 x240
#include "stdio.h"
#include "string.h"
#include "NUC1xx.h"
#include "SYS.h"
#include "SPI.h"
#include "GPIO.h"	
#include "TFT.h"
#include "TFT8x16.h"

#define SPI_CS_CLR   DrvSPI_SetSS(eDRVSPI_PORT3, eDRVSPI_SS0)  
#define SPI_CS_SET   DrvSPI_ClrSS(eDRVSPI_PORT3, eDRVSPI_SS0) 
#define D_C_SET      DrvGPIO_SetBit(E_GPB, 7)
#define D_C_CLR      DrvGPIO_ClrBit(E_GPB, 7)
#define Reset_SET    DrvGPIO_SetBit(E_GPB, 10)
#define Reset_CLR    DrvGPIO_ClrBit(E_GPB, 10)
#define LED_SET      DrvGPIO_SetBit(E_GPB, 9)
#define LED_CLR      DrvGPIO_ClrBit(E_GPB, 9)
#define DEBUG_SET    DrvGPIO_SetBit(E_GPC, 15)
#define DEBUG_CLR    DrvGPIO_ClrBit(E_GPC, 15)

extern  SPI_T * SPI_PORT[4]={SPI0, SPI1, SPI2, SPI3};

void init_SPI3(void)
{
	DrvGPIO_Open(E_GPB, 9, E_IO_OUTPUT);
	DrvGPIO_Open(E_GPB, 7, E_IO_OUTPUT);
	DrvGPIO_Open(E_GPB, 10, E_IO_OUTPUT);
	DrvGPIO_Open(E_GPD, 14, E_IO_OUTPUT);
	DrvGPIO_ClrBit(E_GPD, 14);
	DrvGPIO_InitFunction(E_FUNC_SPI3);
	/* Configure SPI3 as a master, Type1 waveform, 8-bit transaction */
	DrvSPI_Open(eDRVSPI_PORT3, eDRVSPI_MASTER, eDRVSPI_TYPE1, 8);
	/* MSB first */
	DrvSPI_SetEndian(eDRVSPI_PORT3, eDRVSPI_MSB_FIRST);
	/* Disable the automatic slave select function of SS0. */
	DrvSPI_DisableAutoSS(eDRVSPI_PORT3);
	/* Set the active level of slave select. */
	DrvSPI_SetSlaveSelectActiveLevel(eDRVSPI_PORT3, eDRVSPI_ACTIVE_LOW_FALLING);
	/* SPI clock rate 20MHz */
	DrvSPI_SetClockFreq(eDRVSPI_PORT3, 25000000, 0);
  SPI_CS_SET; 
}

void lcdWriteCommand(unsigned char temp)
{
	D_C_CLR;
	SPI_CS_CLR;
  DrvSPI_SingleWrite_byte(eDRVSPI_PORT3,temp);                             
  while(DrvSPI_IsBusy(eDRVSPI_PORT3) != 0); 
	SPI_CS_SET;
}

void lcdWriteData(unsigned char temp)
{
	D_C_SET;
	SPI_CS_CLR;
  DrvSPI_SingleWrite_byte(eDRVSPI_PORT3,temp);                         
  while(DrvSPI_IsBusy(eDRVSPI_PORT3) != 0);
	SPI_CS_SET;	
}

void lcdSetAddr(unsigned int x1,unsigned int x2,unsigned int y1,unsigned int y2)
{
	lcdWriteCommand(0x2A); 	//column address set
	lcdWriteData(x1>>8);
	lcdWriteData(x1);
	lcdWriteData(x2>>8);
	lcdWriteData(x2);
	lcdWriteCommand(0x2B); 	//page(row) address set  
	lcdWriteData(y1>>8);
	lcdWriteData(y1);
	lcdWriteData(y2>>8);
	lcdWriteData(y2);
	lcdWriteCommand(0x2C); // RAM Write
}

void tftWriteData(uint16_t LCD_Data)
{
	lcdWriteData(LCD_Data>>8);
	lcdWriteData(LCD_Data);
}

void init_LCD(void)
{ 
	  init_SPI3();
    Reset_SET;
    Reset_CLR;
    DrvSYS_Delay(10000);
    Reset_SET;
    DrvSYS_Delay(120000);; 
	
        lcdWriteCommand(0xCB); 
        lcdWriteData(0x39); 
        lcdWriteData(0x2C); 
        lcdWriteData(0x00); 
        lcdWriteData(0x34); 
        lcdWriteData(0x02); 

        lcdWriteCommand(0xCF);  //?
        lcdWriteData(0x00); 
        lcdWriteData(0XC1); 
        lcdWriteData(0X30); 
 
        lcdWriteCommand(0xE8);  //?
        lcdWriteData(0x85); 
        lcdWriteData(0x00); 
        lcdWriteData(0x78); 
 
        lcdWriteCommand(0xEA);  //?
        lcdWriteData(0x00); 
        lcdWriteData(0x00); 
 
        lcdWriteCommand(0xED);  //?
        lcdWriteData(0x64); 
        lcdWriteData(0x03); 
        lcdWriteData(0X12); 
        lcdWriteData(0X81); 

        lcdWriteCommand(0xF7); //? 
        lcdWriteData(0x20); 
  
        lcdWriteCommand(0xC0);//Power control 
        lcdWriteData(0x23);   //VRH[5:0] = 3.8V 
 
        lcdWriteCommand(0xC1);//Power control 
        lcdWriteData(0x10);   //SAP[2:0];BT[3:0] 
 
        lcdWriteCommand(0xC5);//VCM control 
        lcdWriteData(0x3e);   //4.25V
        lcdWriteData(0x28);   //-1.5V
 
        lcdWriteCommand(0xC7);//VCM control2 
        lcdWriteData(0x86);   //VMH-58
 
        lcdWriteCommand(0x36);// Memory Access Control 
        //lcdWriteData(0x48); // PORTRAIT
        lcdWriteData(0xe8);   // LANDSCAPE

        lcdWriteCommand(0x3A);// pixel-format =16bit   
        lcdWriteData(0x55); 

        lcdWriteCommand(0xB1); //?   
        lcdWriteData(0x00);  
        lcdWriteData(0x18); 
 
        lcdWriteCommand(0xB6); // Display Function Control 
        lcdWriteData(0x08); 
        lcdWriteData(0x82);
        lcdWriteData(0x27);  

        lcdWriteCommand(0x11);  //Exit Sleep 
        DrvSYS_Delay(120000); 				
        lcdWriteCommand(0x29);  //Display on 
        lcdWriteCommand(0x2c);  // RAM Write
		LED_SET;
}

void printC(int16_t x, int16_t y, unsigned char ascii_code)
{
 	uint8_t i,j,m;
	
 	lcdSetAddr(x,x+8-1,y,y+16-1);	
 	for(i=0; i<16;i++) {		
		m=Font8x16[ascii_code*16+i];		
		for(j=0;j<8;j++) {
			if((m&0x80)==0x80) tftWriteData(FG_COLOR);				
			else	 				     tftWriteData(BG_COLOR);
			m<<=1;
		}	
	}
	
}

void printC_TFT(int16_t x, int16_t y, char ascii_code, uint16_t fgColor, uint16_t bgColor)
{
 	uint8_t i,j,m;
	
 	lcdSetAddr(x,x+8-1,y,y+16-1);	
 	for(i=0; i<16;i++) {		
		m=Font8x16[ascii_code*16+i];		
		for(j=0;j<8;j++) {
			if((m&0x80)==0x80) tftWriteData(fgColor);				
			else	 				     tftWriteData(bgColor);
			m<<=1;
		}	
	}
}

void print_Line(int8_t line, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(i*8,line*16,text[i]);
}

void printS(int16_t x, int16_t y, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(x+i*8, y,text[i]);
}

void printS_TFT(unsigned short x, unsigned short y, char text[], unsigned int fgColor, unsigned int bgColor) 
{ 
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC_TFT(x+i*8, y, text[i], fgColor, bgColor);
}

void clear_LCD(void)
{
 uint16_t i,j;
 lcdSetAddr(0,LCD_Xmax-1,0,LCD_Ymax-1);
 for (j=0; j<LCD_Ymax; j++)
    for (i=0; i<LCD_Xmax; i++) 
        tftWriteData(BG_COLOR);
}

void draw_Bmp8x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k, kx,ky;
	if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check		
		 for (i=0;i<8;i++){
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
				}
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
		 }
}

void draw_Bmp16x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k,kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-7)) // boundary check
		 for (i=0;i<16;i++)
	   {
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
					}
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
			   //x=x++;
		 }
}

void draw_Bmp16x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k, kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-15)) // boundary check
	   for (j=0;j<2; j++){		 
		     for (i=0;i<16;i++) {	
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-x-i));
	       //lcdWriteData(bitmap[i+j*16]);
		     }
		 }
}

void draw_Bmp16x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t, i,j,k, kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-31)) // boundary check
	   for (j=0;j<4; j++)	{			 
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}					 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i+j*16]);
			   //x=x++;
		     }		 
		 }
}

void draw_Bmp16x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k,kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-47)) // boundary check
	   for (j=0;j<6; j++)	{
         k=x;			 
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-k));
	       //lcdWriteData(bitmap[i+j*16]);
			   //k=k++;
		     }		 
		 }
}

void draw_Bmp16x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k,kx,ky;
	if (x<(LCD_Xmax-15) && y==0) // boundary check
	   for (j=0;j<8; j++) {
				 k=x;
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-k));
	       //lcdWriteData(bitmap[i+j*16]);
			   //k=k++;
		     }
		 }
}

void draw_Bmp32x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k,kx,ky;
	if (x<(LCD_Xmax-31) && y<(LCD_Ymax-15)) // boundary check
		for (jy=0;jy<2;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
            kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }
			}
}

void draw_Bmp32x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-31) && y<(LCD_Ymax-31)) // boundary check
		for (jy=0;jy<4;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
            kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }
			}
}

void draw_Bmp32x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-31) && y<(LCD_Ymax-47)) // boundary check
		for (jy=0;jy<6;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
					  kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}	
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }		 
		 }
}

void draw_Bmp32x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-31) && y==0) // boundary check
		for (jy=0;jy<8;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
					  kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }
			}				 
}

void draw_Bmp64x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k,kx,ky;
	if (x<(LCD_Xmax-63) && y==0) // boundary check
		for (jy=0;jy<8;jy++)
	   for (jx=0;jx<4;jx++)	{
	       k=x;
		     for (i=0;i<16;i++) {
					  kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*64];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}					 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*64]);
			   //k=k++;
		     }
			}
}

void draw_Pixel(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor)
{
	lcdSetAddr(x,x,y,y);
	if (fgColor!=0) tftWriteData(fgColor);
	else            tftWriteData(bgColor);
}

// Draw 128x64 
void draw_LCD(unsigned char *buffer)
{
	int16_t x, y;
	int j,m;
	for (x=0; x<128; x++) {
    	for (y=0; y<8; y++) {		
			m=buffer[x+y*128];
			lcdSetAddr(x,x,y*8,y*8+7);
			for(j=0;j<8;j++) {
				if((m&0x01)==0x01) tftWriteData(FG_COLOR);				
				else	 				     tftWriteData(BG_COLOR);
				m>>=1;
			}	
		}
	}			
}

void draw_TFT(unsigned char *buffer, uint16_t fgColor, uint16_t bgColor)
{
	int16_t x, y;
	int j,m;
	for (x=0; x<LCD_Xmax; x++) {
    	for (y=0; y<(LCD_Ymax/8); y++) {		
			m=buffer[x+y*LCD_Xmax];
			lcdSetAddr(x,x,y*8,y*8+7);
			for(j=0;j<8;j++) {
				if((m&0x01)==0x01) tftWriteData(fgColor);				
				else	 				     tftWriteData(bgColor);
				m>>=1;
			}	
		}
	}			
}
