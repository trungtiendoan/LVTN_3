/*
 * sensor.h
 *
 *  Created on: Oct 27, 2026
 *      Author: tient
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#define DHT_PORT GPIOB
#define DHT_PIN  GPIO_PIN_5

#define K_DIV 2
#define V_ZERO 2.5

extern TIM_HandleTypeDef htim3;
extern uint32_t adc[3];

uint8_t DHT11_Read(float *t, float *h);
float ACS_Irms(void);
void delay_us(uint32_t us);

typedef enum{
	LED0_OFF = 0,
	LED0_ON,
	LED1_OFF,
	LED1_ON,
	FAN_OFF,
	FAN_ON,
}STATE;
#endif /* INC_SENSOR_H_ */
