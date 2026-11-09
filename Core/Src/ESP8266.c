/*
 * ESP8266.c
 *
 *  Created on: Sep 27, 2026
 *      Author: hello
 */
#include "ESP8266.h"
#include "usart.h"

static ESP_status ESP8266_WaitACK(char* out, uint16_t length, char* ack, uint32_t timeout)
{
	char temp[256] = {0};

	char* buff = (out!= NULL) ? out : temp;
	if(length == 0)
		length = 256;

	int stick = HAL_GetTick();
	while(HAL_GetTick()-stick < timeout)
	{
		_wait_usart(1000);
		Get_uart_buff(buff, length - strlen(buff));

		if(strstr(buff, ack))
		{
			return ESP_OK;
		}
		else if( strstr(buff, "ERROR"))
		{
			return ESP_error;
		}

	}
	return ESP_Not_feedback;
}

static ESP_status  ESP8266_SendCommand(char * data, char* ack, char* out, uint16_t length, uint32_t timeout)
{

	HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);

	return ESP8266_WaitACK(out, length , ack , timeout);
}

ESP_status ESP8266_init(void)
{
	char rxbufflog[256] ={0};
	ESP_status res;
	HAL_Delay(1000);
//	res = ESP8266_SendCommand("AT+RESTORE\r\n", "OK", rxbufflog, sizeof(rxbufflog), 15000);
//	_log(rxbufflog);
//	HAL_Delay(5000);
	memset(rxbufflog, 0 , 256);
	res = ESP8266_SendCommand("AT\r\n", "OK", rxbufflog, sizeof(rxbufflog), 15000);
	_log(rxbufflog);
	if(res != ESP_OK)
	{
		_log("ESP not responding\r\n");
		return res;
	}
	memset(rxbufflog, 0 , 256);

	res = ESP8266_SendCommand("ATE0\r\n", "OK", rxbufflog, sizeof(rxbufflog), 2000);
	_wait_usart(1000);
	_log(rxbufflog);
	if(res != ESP_OK)
	{
		_log("disable echo command fail\r\n");
		return res;
	}
	memset(rxbufflog, 0 , 256);
	res = ESP8266_SendCommand("AT+GMR\r\n", "OK", rxbufflog, sizeof(rxbufflog), 2000);
	_log(rxbufflog);
	if(res != ESP_OK)
	{
		_log("ESP not access GMR\r\n");
		return res;
	}

	_log("succesfull init esp8266\r\n");
	return ESP_OK;
}
ESP_status ESP_GetIp(char* ip_buffer, uint32_t bufflen)
{
	ESP_status res;
	/*
	char* last_ip;
	res = ESP8266_SendCommand("AT+CIFSR", "OK", 5000);

	if(res != ESP_OK)
	{
		_log("commant CIFSR fail\r\n");
		return res;
	}
	char* ip_start;
	char *search = Get_uart_buff();
	if(strstr(search,"STAIP,") != NULL)
	{
		_log("receive IP\r\n");
		ip_start = strstr(search,"STAIP,\"");
		ip_start+=7;
		char* end = strchr(ip_start,'"');
		if(end && end - ip_start < bufflen)
		{
			last_ip = ip_start;
		}
	}
	if(last_ip)
	{
		char* end = strchr(last_ip,'"');
		strncpy(ip_buffer,last_ip,end - last_ip);
		ip_buffer[end - last_ip] ='\0';
		_log("get IP to buffer\r\n");
		if(strcmp(ip_buffer, "0.0.0.0") == 0)
		{
			_log("IP don't ready yet\r\n");
			HAL_Delay(1000);
			return ESP_error;
		}
		else
		{
			char printf_buffer[256] ={0};
			snprintf(printf_buffer,sizeof(printf_buffer),"got IP:%s\r\n",ip_buffer);
			_log(printf_buffer);
			return ESP_OK;

		}
	}*/
	return res;
}
ESP_status ESP8266connect_Wifi(char *ip_buffer, uint32_t  bufflen, char* ssid, char*password)
{
	char* Client_id = "ESP01_7401";
	char* host = "broker.emqx.io";
	char* port = "1883";
	ESP_status res;
	char CWJAP_buffer[128]= {0};
	res = ESP8266_SendCommand("AT+CWMODE=1\r\n", "OK", NULL, 0, 2000);
	if(res == ESP_OK)
		{
			_log("Da bat WiFi mode Station");
		}
	if(res == ESP_error)
	{
		_log("Khong the bat CWMODE=1");
		return res;
	}

	char Print_buffer[256] = {0};
	snprintf(Print_buffer,sizeof(Print_buffer),"Dang ket noi voi WiFi ten la: %s",ssid);

	_log(Print_buffer);
	snprintf(CWJAP_buffer,sizeof(CWJAP_buffer),"AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
	char log[512] = {0};
	res =ESP8266_SendCommand(CWJAP_buffer, "OK", log, 512, 10000);
	_log(log);
	if(res == ESP_OK)
	{
		_log("Ket noi WiFi thanh cong");
	}
	if(res == ESP_error)
	{
		_log("Khong the ket noi WiFi");
		return res;
	}
	if(res == ESP_Not_feedback)
	{
		_log("Trong luc nhan du lieu da that bai bat duoc ack");
		return res;
	}

	memset(Print_buffer,0,sizeof(Print_buffer));
	snprintf(Print_buffer,sizeof(Print_buffer),"Dang tao client_id la: %s",Client_id);
	_log(Print_buffer);
	char MQTT_USER_CFG[256] = {0};
	snprintf(MQTT_USER_CFG,sizeof(MQTT_USER_CFG),"AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"\r\n",Client_id);
	memset(log,0,512);
	res = ESP8266_SendCommand(MQTT_USER_CFG, "OK", log, 512, 15000);
	_log(log);
	if(res == ESP_OK)
	{
		_log("Da tao client ten la ESP01_7400 ");
	}
	if(res == ESP_error)
	{
		_log("Khong the tao client");
		return res;
	}

	memset(Print_buffer,0,sizeof(Print_buffer));
	snprintf(Print_buffer,sizeof(Print_buffer),"Dang ket noi toi host la: %s",host);
	_log(Print_buffer);
	char MQTT_CONN[256] = {0};
	snprintf(MQTT_CONN,sizeof(MQTT_CONN),"AT+MQTTCONN=0,\"%s\",%s,1\r\n",host,port);
	memset(log,0,512);
	res = ESP8266_SendCommand(MQTT_CONN, "OK", log, 512, 15000);
	_log(log);
	if(res == ESP_OK)
	{
		_log("Da ket noi thanh cong voi host ");
	}
	if(res == ESP_error)
	{
		_log("Khong the tao ket noi toi host");
		return res;
	}

	return ESP_OK;
}
ESP_status MQTT_Sub(char * subcribe)
{
	ESP_status res;
	_log("Dang tien hanh subcribe");
	char MQTT_SUB[256] = {0};
	char Print_buffer[256] = {0};
	char log[512];
	snprintf(MQTT_SUB,sizeof(MQTT_SUB),"AT+MQTTSUB=0,\"%s\",0\r\n",subcribe);
	res = ESP8266_SendCommand(MQTT_SUB, "OK", log, 512, 1000);
	_log(log);
	if(res == ESP_OK)
	{
		snprintf(Print_buffer,sizeof(Print_buffer),"Da subcribe thanh cong voi kenh la: %s",subcribe);
		_log(Print_buffer);
	}
	if(res == ESP_error)
	{
		memset(Print_buffer,0,sizeof(Print_buffer));
		snprintf(Print_buffer,sizeof(Print_buffer),"Khong the subcribe voi kenh la: %s",subcribe);
		_log(Print_buffer);
		return res;
	}
	return res;
}
ESP_status MQTT_Pub(char * publish_topic, char* publish_string)

{
	ESP_status res;
	_log("Dang tien hanh publish");
	char MQTT_PUB[512] = {0};
	char Print_buffer[512] = {0};
	char log[512];
	snprintf(MQTT_PUB,sizeof(MQTT_PUB),"AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n",publish_topic,publish_string);
	res =ESP8266_SendCommand(MQTT_PUB, "OK", log, 512, 10000);
	_log(log);
	if(res == ESP_OK)
	{
		snprintf(Print_buffer,sizeof(Print_buffer),"Da publish thanh cong voi" \
				" topic la %s voi noi dung la: %s",publish_topic,publish_string);
		_log(Print_buffer);
	}
	if(res == ESP_error)
	{
		_log("That bai khi publish");
		return res;
	}
	return res;
}
STATE Subcribe_event_get_data(uint8_t *flag, uint8_t *flag1,uint8_t *flag2)
{
	char search[1024] = {0};
	char chuoi[256];
	char* ip_start;
	STATE data;
	Get_uart_subbuff(search, sizeof(search));

	if(strstr(search,"+MQTTSUBRECV:0,") != NULL)
	{
		_log("Vi dieu khien da nhan duoc chuoi:");
		_log(search);
		if(strstr(search,"\"led0") != NULL)
		{

			if(strstr(search,"\"on") != NULL)
			{
				ip_start = strstr(search,"\"on");
				ip_start +=5;
				data = atoi(ip_start);
				*flag = 1;
				return data;
			}
			else if(strstr(search,"\"off") != NULL)
			{
				ip_start = strstr(search,"\"off");
				ip_start +=6;
				data = atoi(ip_start);
				*flag = 1;
				return data;
			}
		}
		if(strstr(search,"\"led1") != NULL)
		{
			if(strstr(search,"\"on") != NULL)
			{
				ip_start = strstr(search,"\"on");
				ip_start +=5;
				data = atoi(ip_start);
				*flag1 = 1;
				return data;
			}
			else if(strstr(search,"\"off") != NULL)
			{
				ip_start = strstr(search,"\"off");
				ip_start +=6;
				data = atoi(ip_start);
				*flag1 = 1;
				return data;
			}
		}
		if(strstr(search,"\"fan") != NULL)
		{
			if(strstr(search,"\"on") != NULL)
			{
				ip_start = strstr(search,"\"on");
				ip_start +=5;
				data = atoi(ip_start);
				*flag2 = 1;
				return data;
			}
			else if(strstr(search,"\"off") != NULL)
			{
				ip_start = strstr(search,"\"off");
				ip_start +=6;
				data = atoi(ip_start);
				*flag2 = 1;
				return data;
			}
		}
//		char *adress_start = strstr(search,"\"LED_STATE");
//		adress_start += 16;
//
//		char *adress_end = strchr(adress_start,'"');
//		strncpy(chuoi, adress_start, adress_end-adress_start);
//		chuoi[adress_end-adress_start] = '\0';
//		_log(chuoi);

//		if(chuoi == "LIGHT")
//		{
//			return LIGHT;
//		}
//		else if(chuoi == "OFF")
//		{
//			return OFF;
//		}
	}
}
