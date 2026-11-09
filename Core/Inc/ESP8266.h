/*
 * ESP8266.h
 *
 *  Created on: Sep 27, 2026
 *      Author: hello
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "sensor.h"
#include "stdlib.h"

#define ESP_uart huart1
#define Log_uart huart2
int _log( char* data );


typedef enum {
	ESP_error = 0,
	ESP_OK,
	ESP_Not_feedback
}ESP_status;
ESP_status ESP8266_init(void);
ESP_status ESP8266connect_Wifi(char *ip_buffer, uint32_t bufflen, char* ssid, char*password);
ESP_status MQTT_Sub(char * subcribe);
ESP_status MQTT_Pub(char * publish_topic, char* publish_string);
STATE Subcribe_event_get_data(uint8_t *flag, uint8_t *flag1,uint8_t *flag2);
#endif /* INC_ESP8266_H_ */
