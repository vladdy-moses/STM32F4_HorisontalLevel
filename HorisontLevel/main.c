#include "main.h"

const double PI = 3.1415926;
const double HALF_PI = 1.57079632675;

void timer_Init(uint16_t frequency);
void mySPI_Init(void);
void mySPI_SendData(uint8_t adress, uint8_t data);
uint8_t mySPI_GetData(uint8_t adress);
void lal();

int main(void) {
	SystemInit();

	GPIO_InitTypeDef GPIO_InitStructure;

	//enables GPIO clock for PortD
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13
			| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	mySPI_Init();
	mySPI_SendData(0x20, 0x47); //LIS302D Config

	LED_BLUE_ON;
	LED_RED_ON;
	LED_GREEN_ON;
	LED_ORANGE_ON;
	while (1) {
		lal();
	}
}

void mySPI_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_InitTypeDef SPI_InitTypeDefStruct;

	SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI1, &SPI_InitTypeDefStruct);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitTypeDef GPIO_InitTypeDefStruct;

	GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);

	GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_InitTypeDefStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	GPIO_SetBits(GPIOE, GPIO_Pin_3);

	SPI_Cmd(SPI1, ENABLE);
}

void mySPI_SendData(uint8_t adress, uint8_t data) {
	GPIO_ResetBits(GPIOE, GPIO_Pin_3);

	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		;
	SPI_I2S_SendData(SPI1, adress);
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
		;
	SPI_I2S_ReceiveData(SPI1);

	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		;
	SPI_I2S_SendData(SPI1, data);
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
		;
	SPI_I2S_ReceiveData(SPI1);

	GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

uint8_t mySPI_GetData(uint8_t adress) {

	GPIO_ResetBits(GPIOE, GPIO_Pin_3);

	adress = 0x80 | adress;

	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		;
	SPI_I2S_SendData(SPI1, adress);
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
		;
	SPI_I2S_ReceiveData(SPI1); //Clear RXNE bit

	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		;
	SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
		;

	GPIO_SetBits(GPIOE, GPIO_Pin_3);

	return SPI_I2S_ReceiveData(SPI1);
}

int8_t x_data, y_data, z_data;
double angle;

void lal(void) {
	x_data = mySPI_GetData(0x29);
	y_data = mySPI_GetData(0x2B);
	z_data = mySPI_GetData(0x2D);

	if ((x_data > 0) && (y_data >= 0))
		angle = atan((double) y_data / x_data);
	if ((x_data > 0) && (y_data < 0))
		angle = atan((double) y_data / x_data) + 2 * PI;
	if ((x_data < 0))
		angle = atan((double) y_data / x_data) + PI;
	if ((x_data == 0) && (y_data > 0))
		angle = HALF_PI;
	if ((x_data == 0) && (y_data < 0))
		angle = 3 * HALF_PI;
	if ((x_data == 0) && (y_data == 0))
		angle = 0;

	LED_BLUE_OFF;
	LED_RED_OFF;
	LED_GREEN_OFF;
	LED_ORANGE_OFF;
	if ((angle > 7 * HALF_PI / 2) || (angle < HALF_PI / 2)) {
		LED_GREEN_ON;
	} else if ((angle > HALF_PI / 2) && (angle < 3 * HALF_PI / 2)) {
		LED_BLUE_ON;
	} else if ((angle > 3 * HALF_PI / 2) && (angle < 5 * HALF_PI / 2)) {
		LED_RED_ON;
	} else {
		LED_ORANGE_ON;
	}
}
