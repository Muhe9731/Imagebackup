#ifndef __OLEDBUFF_H__
#define __OLEDBUFF_H__

#include "stm32h7xx_hal.h"
#include "oledfont.h"

//#define OLED_Set_CS(x)  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, x)     // CS 线
//#define OLED_Set_DC(x)  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, x)     // DC 线
#define OLED_Set_Res(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, x)     // Reset

void OLED_WR_CMD(uint8_t cmd);
void OLED_WR_DATA(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_On(void);

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,uint8_t Color_Turn);
void OLED_ShowString(uint8_t x,uint8_t y,char*chr,uint8_t Char_Size, uint8_t Color_Turn);
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2, uint8_t Color_Turn);
void OLED_Showdecimal(uint8_t x,uint8_t y,float num,uint8_t z_len,uint8_t f_len,uint8_t size2, uint8_t Color_Turn);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no, uint8_t Color_Turn);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *  BMP,uint8_t Color_Turn);

void OLED_Refresh(void);
void OLED_Clear_Buff(void);

#endif


