#ifndef __OLED_H
#define __OLED_H 

#include "stdint.h"
#include "board.h"
#include "stdlib.h"	

//-----------------OLED端口定义---------------- 

// miniscope v1.0
#define OLED_SCL_Clr() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 24, 0)   	//0
#define OLED_SCL_Set() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 24, 1)		//1

#define OLED_SDA_Clr() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 15, 0)   	//0
#define OLED_SDA_Set() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 15, 1)   	//1

#define OLED_RES_Clr() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 20, 0)   	//0
#define OLED_RES_Set() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 20, 1)   	//1

#define OLED_DC_Clr()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 19, 0)        //0 pin25
#define OLED_DC_Set()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 19, 1)        //1 pin25
 		     
#define OLED_CS_Clr()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 27, 0)   	//0
#define OLED_CS_Set()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 27, 1)   	//1
/*
#define OLED_SCL_Clr() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 20, 0)   	//0
#define OLED_SCL_Set() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 20, 1)		//1

#define OLED_SDA_Clr() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 15, 0)   	//0
#define OLED_SDA_Set() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 15, 1)   	//1

#define OLED_RES_Clr() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 24, 0)   	//0
#define OLED_RES_Set() Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 24, 1)   	//1

#define OLED_DC_Clr()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 25, 0)        //0 pin25
#define OLED_DC_Set()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 25, 1)        //1 pin25
 		     
#define OLED_CS_Clr()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 27, 0)   	//0
#define OLED_CS_Set()  Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 27, 1)   	//1
*/

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DirectClear(void);
void OLED_ShowGrid(uint32_t waveOrFFt);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);
void OLED_DrawWave(uint8_t x,uint8_t y);
void Before_State_Update(uint8_t y);
void Current_State_Update(uint8_t y);

#endif

