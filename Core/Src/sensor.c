/*
 * sensor.c
 *
 *  Created on: Oct 27, 2026
 *      Author: tient
 */
#include "sensor.h"
uint8_t state = 0;

void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim3,0);
	while(__HAL_TIM_GET_COUNTER(&htim3) < us)
	{
	}
}

void DHT_SetOutPut(void)
{
	GPIO_InitTypeDef GPIO_Initstruct = {0};

	GPIO_Initstruct.Pin = DHT_PIN;
	GPIO_Initstruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Initstruct.Pull = GPIO_NOPULL;
	GPIO_Initstruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(DHT_PORT, &GPIO_Initstruct);
}
void DHT_SetInPut(void)
{
	GPIO_InitTypeDef GPIO_Initstruct = {0};

	GPIO_Initstruct.Pin = DHT_PIN;
	GPIO_Initstruct.Mode = GPIO_MODE_INPUT;
	GPIO_Initstruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(DHT_PORT, &GPIO_Initstruct);

}
uint8_t wait_level(GPIO_PinState level,uint16_t timeout_us)
{
	__HAL_TIM_SET_COUNTER(&htim3,0);
	uint16_t start = (uint16_t) __HAL_TIM_GET_COUNTER(&htim3);
	state = HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN);
	while(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) != level)
	{
		uint16_t now = (uint16_t) __HAL_TIM_GET_COUNTER(&htim3);

		if((uint16_t)(now - start) >= timeout_us)
		{
			return 0;
		}
	}
	return 1;
}
uint8_t DHT11_Read(float *t, float *h) {
  uint8_t d[5] = {0};
  DHT_SetOutPut(); HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 0);
  HAL_Delay(20);                       // start >18 ms
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 1); delay_us(30);
  DHT_SetInPut();
  if (!wait_level(0, 100) || !wait_level(1, 100) || !wait_level(0, 100)) return 0; // phản hồi 80us/80us
  for (int i = 0; i < 40; i++) {
    wait_level(1, 70);                 // hết 50us mức thấp
    delay_us(40);                      // bit 1 cao ~70us, bit 0 ~26us
    d[i/8] <<= 1;
    if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)) { d[i/8] |= 1; wait_level(0, 60); }
  }
  if ((uint8_t)(d[0]+d[1]+d[2]+d[3]) != d[4]) return 0;   // checksum
  *h = d[0]; *t = d[2] + d[3]*0.1f;
  return 1;
}



