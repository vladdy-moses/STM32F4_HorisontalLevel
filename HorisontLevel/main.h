//Includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"

//Defines
#define LED_BLUE_ON   GPIOD->BSRRL = GPIO_Pin_15;
#define LED_BLUE_OFF  GPIOD->BSRRH = GPIO_Pin_15;

#define LED_RED_ON   GPIOD->BSRRL = GPIO_Pin_14;
#define LED_RED_OFF  GPIOD->BSRRH = GPIO_Pin_14;

#define LED_ORANGE_ON   GPIOD->BSRRL = GPIO_Pin_13;
#define LED_ORANGE_OFF  GPIOD->BSRRH = GPIO_Pin_13;

#define LED_GREEN_ON   GPIOD->BSRRL = GPIO_Pin_12;
#define LED_GREEN_OFF  GPIOD->BSRRH = GPIO_Pin_12;
