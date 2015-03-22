#include "accelerometer.h"

const double PI = 3.1415926;
const double HALF_PI = 1.57079632675;

void accelerometer_Init(void) {
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

void accelerometer_SendData(uint8_t adress, uint8_t data) {
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

uint8_t accelerometer_GetData(uint8_t adress) {
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

Accelerometer_Result accelerometer_Work(void) {

	/*uint8_t x_data = (accelerometer_GetData(0x29) << 1) ;
	 uint8_t y_data = (accelerometer_GetData(0x2B) << 1) ;
	 uint8_t z_data = (accelerometer_GetData(0x2D) << 1) ;*/

	x_data = accelerometer_GetData(0x29);
	y_data = accelerometer_GetData(0x2B);
	z_data = accelerometer_GetData(0x2D);

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
		angle = 0.;

	Accelerometer_Result result = { .angle = angle, .deviation = (x_data
			* x_data + y_data * y_data), .x = x_data, .y = y_data, .z = z_data };
	return result;
}
