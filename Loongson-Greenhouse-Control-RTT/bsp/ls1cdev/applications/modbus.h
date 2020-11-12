/*
 * @filename: modbus.h
 * @Descripttion: 
 * @version: V1.0
 * @Author: Bi Jiaqin
 * @Date: 2020-10-10 08:46:10
 * @LastEditors: Bi Jiaqin
 * @LastEditTime: 2020-10-28 14:28:31
 */ 
#ifndef __OPENLOONGSON_MODBUS_H
#define __OPENLOONGSON_MODBUS_H
#include <rtthread.h>

struct recvdata                                      /* 定义一个结构体存放接收的数据 */
{
    int air_temperate_time;                           /* 时间戳 for nrf24l01 pipe0 data */
    int air_temperate;                                /* 室内空气温度值 for nrf24l01 pipe0 data */
    int air_humidity_time;                            /* 时间戳 for nrf24l01 pipe0 data */
    int air_humidity;                                 /* 室内空气湿度值 for nrf24l01 pipe0 data */
    int air_lux_time;                                 /* 时间戳 for nrf24l01 pipe0 data */
    int air_lux;                                      /* 室内光照度值 for nrf24l01 pipe0 data */
    int air_co2_time;                                 /* 时间戳 for nrf24l01 pipe0 data */
    int air_co2;                                      /* 室内二氧化碳浓度值 for nrf24l01 pipe0 data */
    int soil_humidity_time;                           /* 时间戳 for nrf24l01 pipe0 data */
    int soil_humidity;                                /* 室内土壤湿度值 for nrf24l01 pipe0 data */
    int soil_temperate_time;                          /* 时间戳 for nrf24l01 pipe0 data */
    int soil_temperate;                               /* 室内土壤温度值 for nrf24l01 pipe0 data */
};


rt_uint8_t RS485_temperature[10];
rt_uint8_t RS485_humidity[10];
rt_uint8_t RS485_lux[10];
rt_uint8_t RS485_co2[10];
rt_uint8_t RS485_soil_humidity[10];
rt_uint8_t RS485_soil_temperate[10];

rt_uint16_t CRC16(rt_uint8_t *puchMsg, rt_uint16_t usDataLen);     //CRC校验
void RS485_uart_irqhandler(int IRQn, void *param);                 //485串口初始化
rt_uint8_t get_rs485buff(rt_uint8_t rebuff[],rt_uint16_t *p);      //获取485数据帧


void air_temperate_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t AM_temperature[],rt_int16_t *p);
void air_humidity_receive (rt_uint8_t Pro_BUFF[],rt_uint8_t AM_temperature[],rt_int16_t *p);
void air_lux_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t air_lux[],rt_int16_t *p);
void air_co2_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t air_co2[],rt_int16_t *p);
void soil_humidity_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t soil_humidity[],rt_int16_t *p);
void soil_temperate_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t soil_temperate[],rt_int16_t *p);

short int change(short int temperature);






#define DEVICE_ID	           "637203510"	
#define API_KEY		           "m78I6ZbncO7rMRplwuBIr60tEFs="
#define ONENET_air_temperate   "air_temperate"
#define ONENET_air_humidity	   "air_humidity"
#define ONENET_air_lux	       "air_lux"
#define ONENET_air_co2	       "air_co2"
#define ONENET_soil_humidity   "soil_humidity"
#define ONENET_soil_temperate  "soil_temperate"



void clean_buff(void);
int RS485_Send(char *p,int len);
void RS485_Init(void);

#endif

