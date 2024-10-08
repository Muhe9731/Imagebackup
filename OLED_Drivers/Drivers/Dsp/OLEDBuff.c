#include "OLEDBuff.h"
#include "i2c.h"
#include "oled.h"

/*
 * oled.c
 *
 *  Created on: 2022年7月24日
 *      Author: Muhe
 */

static uint8_t OLED_Buff[8 * 128] = {0}; // 屏幕缓冲区

/**********************************************************
 * 初始化命令,根据芯片手册书写，详细步骤见上图以及注意事项
 ***********************************************************/
uint8_t CMD_Data[] = {

	0xAE, // 关闭显示
	0xD5, // 设置时钟分频因子,震荡频率
	0x80, //[3:0],分频因子;[7:4],震荡频率
	0xA8, // 设置驱动路数
	0X3F, // 默认 0X3F(1/64)
	0xD3, // 设置显示偏移
	0X00, // 默认为 0
	0x40, // 设置显示开始行 [5:0],行数.
	0x8D, // 电荷泵设置
	0x14, // bit2，开启/关闭
	0x20, // 设置内存地址模式
	0x00, //[1:0],00列地址模式; 01行地址模式; 10页地址模式; 默认10;
	0xA1, // 段重定义设置,bit0:0,0->0; 1,0->127;
	0xC8, // 设置 COM 扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	0xDA, // 设置 COM 硬件引脚配置
	0x12, //[5:4]配置
	0x81, // 对比度设置
	0xEF, // 1~255;默认 0X7F (亮度设置,越大越亮)
	0xD9, // 设置预充电周期
	0xf1, //[3:0],PHASE 1;[7:4],PHASE 2;
	0xDB, // 设置 VCOMH 电压倍率
	0x30, //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	0xA4, // 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	0xA6, // 设置显示方式;bit0:1,反相显示;0,正常显示
	0xAF, // 开启显示

	0x21, // 设置列地址起始位置与终点位置
	0x00,
	0x7F, // 0x00~0x7F = 0~127
	0x22, // 设置页地址起始位置与终点位置
	0x00,
	0x07, // 0x00~0x07 = 0~7
};

/**
 * @function: void OLED_Init(void)
 * @description: OLED初始化
 * @return {*}
 */
void OLED_Init(void)
{
	//	OLED_Set_CS(GPIO_PIN_RESET);
	//	OLED_Set_DC(GPIO_PIN_RESET);
	OLED_Set_Res(GPIO_PIN_SET);
	HAL_Delay(100);
	OLED_Set_Res(GPIO_PIN_RESET);
	HAL_Delay(100);
	OLED_Set_Res(GPIO_PIN_SET);
	HAL_Delay(100);

	uint8_t i = 0;
	for (i = 0; i < sizeof(CMD_Data) / sizeof(CMD_Data[0]); i++)
	{
		OLED_WR_CMD(CMD_Data[i]);
	}
}

/**
 * @function: void OLED_WR_CMD(uint8_t cmd)
 * @description: 向设备写控制命令
 * @param {uint8_t} cmd 芯片手册规定的命令
 * @return {*}
 */
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0x100);
}

/**
 * @function: void OLED_WR_DATA(uint8_t data)
 * @description: 向设备写控制数据
 * @param {uint8_t} data 数据
 * @return {*}
 */
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
}

/**
 * @function: void OLED_On(void)
 * @description: 更新显示

 * @return {*}
 */
void OLED_On(void)
{
	uint8_t *OLED_Buffptr = OLED_Buff;
	for (uint16_t i = 0; i < 1024; i++)
	{
		*OLED_Buffptr++ = 0xff;
	}
	OLED_Refresh();
}

/**
 * @function: OLED_Clear(void)
 * @description: 清屏,整个屏幕是黑色的!和没点亮一样!!!
 * @return {*}
 */
void OLED_Clear(void)
{
	uint8_t *OLED_Buffptr = OLED_Buff;
	for (uint16_t i = 0; i < 1024; i++)
	{
		*OLED_Buffptr++ = 0;
	}
	OLED_Refresh();
}

/**
 * @function: OLED_Clear_Buff(void)
 * @description: 清除缓冲区 OLED_Buff
 * @return {*}
 */
void OLED_Clear_Buff(void)
{
	uint8_t *OLED_Buffptr = OLED_Buff;
	for (uint16_t i = 0; i < 1024; i++)
	{
		*OLED_Buffptr++ = 0;
	}
}

/**
 * @function: void OLED_Display_On(void)
 * @description: 开启OLED显示
 * @return {*}
 */
void OLED_Display_On(void)
{
	OLED_WR_CMD(0X8D); // SET DCDC命令
	OLED_WR_CMD(0X14); // DCDC ON
	OLED_WR_CMD(0XAF); // DISPLAY ON,打开显示
}

/**
 * @function: void OLED_Display_Off(void)
 * @description: 关闭OLED显示
 * @return {*}
 */
void OLED_Display_Off(void)
{
	OLED_WR_CMD(0X8D); // SET DCDC命令
	OLED_WR_CMD(0X10); // DCDC OFF
	OLED_WR_CMD(0XAE); // DISPLAY OFF，关闭显示
}

/**
 * @function: unsigned int oled_pow(uint8_t m,uint8_t n)
 * @description: m^n函数
 * @param {uint8_t} m,n
 * @return {unsigned int} result
 */
unsigned int oled_pow(uint8_t m, uint8_t n)
{
	unsigned int result = 1;
	while (n--)
		result *= m;
	return result;
}

/**
 * @function: void OLED_Refresh(void)
 * @description: 向设备写数据，将OLED_Buff缓冲区数据更新到OLED
 * @return {*}
 */
void OLED_Refresh(void)
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, OLED_Buff, 1024, 0x100); // 计算一下时间
	OLED_Clear_Buff();
}

/**
 * @function: void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size,uint8_t Color_Turn)
 * @description: 在OLED12864特定位置开始显示一个字符
 * @param {uint8_t} x字符开始显示的横坐标
 * @param {uint8_t} y字符开始显示的纵坐标
 * @param {uint8_t} chr待显示的字符
 * @param {uint8_t} Char_Size待显示字符的字体大小,选择字体 16/12
 * @param {uint8_t} Color_Turn是否反相显示(1反相、0不反相)
 * @return {*}
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size, uint8_t Color_Turn)
{
	unsigned char c = 0, i = 0;
	uint8_t *OLED_Buffptr = OLED_Buff;
	c = chr - ' '; // 得到偏移后的值
	if (Char_Size == 16)
	{
		OLED_Buffptr = OLED_Buff + y * 128 + x;
		for (i = 0; i < 8; i++)
		{
			if (Color_Turn)
			{
				*OLED_Buffptr = ~F8X16[c * 16 + i];
			}
			else
			{
				*OLED_Buffptr = F8X16[c * 16 + i];
			}
			OLED_Buffptr++;
		}
		OLED_Buffptr = OLED_Buff + (y + 1) * 128 + x;
		for (i = 0; i < 8; i++)
		{
			if (Color_Turn)
			{
				*OLED_Buffptr = ~F8X16[c * 16 + i + 8];
			}
			else
			{
				*OLED_Buffptr = F8X16[c * 16 + i + 8];
			}
			OLED_Buffptr++;
		}
	}
	else
	{
		OLED_Buffptr = OLED_Buff + y * 128 + x;
		for (i = 0; i < 6; i++)
		{
			if (Color_Turn)
				*OLED_Buffptr = ~F6x8[c][i];
			else
				*OLED_Buffptr = F6x8[c][i];
			OLED_Buffptr++;
		}
	}
}

/**
 * @function: void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_tChar_Size, uint8_t Color_Turn)
 * @description: 在OLED12864特定位置开始显示字符串
 * @param {uint8_t} x待显示字符串的开始横坐标x:0~127
 * @param {uint8_t} y待显示字符串的开始纵坐标 y:0~7，若选择字体大小为16，则两行数字之间需要间隔2，若选择字体大小为12，间隔1
 * @param {uint8_t} *chr待显示的字符串
 * @param {uint8_t} Char_Size待显示字符串的字体大小,选择字体 16/12，16为8X16，12为6x8
 * @param {uint8_t} Color_Turn是否反相显示(1反相、0不反相)
 * @return {*}
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size, uint8_t Color_Turn)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size, Color_Turn);
		if (Char_Size == 12) // 6X8的字体列加6，显示下一个字符
			x += 6;
		else // 8X16的字体列加8，显示下一个字符
			x += 8;

		if (x > 122 && Char_Size == 12) // TextSize6x8如果一行不够显示了，从下一行继续显示
		{
			x = 0;
			y++;
		}
		if (x > 120 && Char_Size == 16) // TextSize8x16如果一行不够显示了，从下一行继续显示
		{
			x = 0;
			y++;
		}
		j++;
	}
}

/**
 * @function: void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2, Color_Turn)
 * @description: 显示数字
 * @param {uint8_t} x待显示的数字起始横坐标,x:0~126
 * @param {uint8_t} y待显示的数字起始纵坐标, y:0~7，若选择字体大小为16，则两行数字之间需要间隔2，若选择字体大小为12，间隔1
 * @param {unsigned int} num:输入的数据
 * @param {uint8_t } len:输入的数据位数
 * @param {uint8_t} size2:输入的数据大小，选择 16/12，16为8X16，12为6x8
 * @param {uint8_t} Color_Turn是否反相显示(1反相、0不反相)
 * @return {*}
 */
void OLED_ShowNum(uint8_t x, uint8_t y, unsigned int num, uint8_t len, uint8_t size2, uint8_t Color_Turn)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2, Color_Turn);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2, Color_Turn);
	}
}

/**
 * @function: void OLED_Showdecimal(uint8_t x,uint8_t y,float num,uint8_t z_len,uint8_t f_len,uint8_t size2, uint8_t Color_Turn)
 * @description: 显示正负浮点数
 * @param {uint8_t} x待显示的数字起始横坐标,x:0~126
 * @param {uint8_t} y待显示的数字起始纵坐标, y:0~7，若选择字体大小为16，则两行数字之间需要间隔2，若选择字体大小为12，间隔1
 * @param {float} num:输入的浮点型数据
 * @param {uint8_t } z_ len:整数部分的位数
 * @param {uint8_t } f_len: 小数部分的位数
 * @param {uint8_t} size2:输入的数据大小，选择 16/12，16为8X16，12为6x8
 * @param {uint8_t} Color_Turn是否反相显示(1反相、0不反相)
 * @return {*}
 */
void OLED_Showdecimal(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size2, uint8_t Color_Turn)
{
	uint8_t t, temp, i = 0; // i为负数标志位
	uint8_t enshow;
	int z_temp, f_temp;
	if (num < 0)
	{
		z_len += 1;
		i = 1;
		num = -num;
	}
	z_temp = (int)num;
	// 整数部分
	for (t = 0; t < z_len; t++)
	{
		temp = (z_temp / oled_pow(10, z_len - t - 1)) % 10;
		if (enshow == 0 && t < (z_len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2, Color_Turn);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2, Color_Turn);
	}
	// 小数点
	OLED_ShowChar(x + (size2 / 2) * (z_len), y, '.', size2, Color_Turn);

	f_temp = (int)((num - z_temp) * (oled_pow(10, f_len)));
	// 小数部分
	for (t = 0; t < f_len; t++)
	{
		temp = (f_temp / oled_pow(10, f_len - t - 1)) % 10;
		OLED_ShowChar(x + (size2 / 2) * (t + z_len) + 5, y, temp + '0', size2, Color_Turn);
	}
	if (i == 1) // 如果为负，就将最前的一位赋值‘-’
	{
		OLED_ShowChar(x, y, '-', size2, Color_Turn);
		i = 0;
	}
}

/**
 * @function: void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no, uint8_t Color_Turn)
 * @description: 在OLED特定位置开始显示16X16汉字
 * @param {uint8_t} x待显示的汉字起始横坐标x: 0~112，两列汉字之间需要间隔16
 * @param {uint8_t} y待显示的汉字起始纵坐标 y: 0~6 , 两行汉字之间需要间隔2
 * @param {uint8_t} no待显示的汉字编号
 * @param {uint8_t} Color_Turn是否反相显示(1反相、0不反相)
 * @return {*}
 */
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no, uint8_t Color_Turn)
{
	uint8_t t = 0;
	uint8_t *OLED_Buffptr = OLED_Buff;
	OLED_Buffptr = OLED_Buff + y * 128 + x;
	for (t = 0; t < 16; t++)
	{
		if (Color_Turn)
			*OLED_Buffptr = ~Hzk[2 * no][t]; // 显示汉字的上半部分
		else
			*OLED_Buffptr = Hzk[2 * no][t]; // 显示汉字的上半部分
		OLED_Buffptr++;
	}
	OLED_Buffptr = OLED_Buff + (y + 1) * 128 + x;
	for (t = 0; t < 16; t++)
	{
		if (Color_Turn)
			*OLED_Buffptr = ~Hzk[2 * no + 1][t]; // 显示汉字的上半部分
		else
			*OLED_Buffptr = Hzk[2 * no + 1][t]; // 显示汉字的上半部分
		OLED_Buffptr++;
	}
}
/**
 * @function: void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *  BMP,uint8_t Color_Turn)
 * @description: 在OLED特定区域显示BMP图片
 * @param {uint8_t} x0图像开始显示横坐标  x0:0~127
 * @param {uint8_t} y0图像开始显示纵坐标  y0:0~7
 * @param {uint8_t} x1图像结束显示横坐标  x1:1~128
 * @param {uint8_t} y1图像结束显示纵坐标  y1:1~8
 * @param {uint8_t} *BMP待显示的图像数据
 * @param {uint8_t} Color_Turn是否反相显示(1反相、0不反相)
 * @return {*}
 */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *BMP, uint8_t Color_Turn)
{
	uint32_t j = 0;
	uint8_t x = 0, y = 0;
	uint8_t *OLED_Buffptr = OLED_Buff;
	for (y = y0; y < y1; y++)
	{
		OLED_Buffptr = OLED_Buff + y * 128 + x0;
		for (x = x0; x < x1; x++)
		{
			if (Color_Turn)
				*OLED_Buffptr = ~BMP[j++]; // 显示反相图片
			else
				*OLED_Buffptr = BMP[j++]; // 显示图片
			OLED_Buffptr++;
		}
	}
}
