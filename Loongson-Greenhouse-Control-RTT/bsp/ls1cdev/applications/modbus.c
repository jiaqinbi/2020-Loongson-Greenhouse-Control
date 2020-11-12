/*
 * @filename: modbus.c
 * @Descripttion: 
 * @version: V1.0
 * @Author: Bi Jiaqin
 * @Date: 2020-10-10 08:46:20
 * @LastEditors: Bi Jiaqin
 * @LastEditTime: 2020-11-08 13:26:18
 */ 

#include <rtthread.h>
#include <stdlib.h>  
#include "ls1c.h"
#include <drivers/pin.h>
#include "ls1c_public.h"
#include "ls1c_uart.h"
#include "ls1c_pin.h"  
#include "../drivers/drv_uart.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_gpio.h"
#include "string.h"
#include "modbus.h"
#include "onenet.h" 
#include <rthw.h>
#include <rtdbg.h>

#define RS485_BUFF_SIZE 1024
#define RD_GPIO         57

volatile rt_uint16_t uart_p=0;
rt_uint8_t RS485_BUFF[RS485_BUFF_SIZE];

char str_air_temperate[20];
char str_air_humidity[20];
char str_air_lux[20];
char str_air_co2[20];
char str_soil_temperate[20];
char str_soil_humidity[20];

extern rt_mq_t      air_temperate_mq;
extern rt_mq_t      air_humidity_mq;


struct recvdata rs485_recedata;

short int temperature_first;
float temperature_H;
float temperature_L;
float temperature_end;
rt_uint16_t  humidity_end;
rt_uint32_t  lux_high;
rt_uint32_t  lux_low;
rt_uint32_t  lux_end;
rt_uint16_t  co2_end;
rt_uint16_t  soil_humidty_end;
rt_uint16_t  soil_temperate_end;
/*crc校验*/
static const rt_uint8_t auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};
 
static const rt_uint8_t auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

/**
 * @name: CRC16
 * @test: 获取CRC16校验值
 * @msg: 
 * @param {type} -- {puchMsg:要校验的数组  usDataLen:校验的长度}
 * @return {rt_uint16_t} -- {CRC16校验码}
 */
rt_uint16_t CRC16(rt_uint8_t *puchMsg, rt_uint16_t usDataLen)
{
  rt_uint8_t uchCRCHi = 0xFF; // 高CRC字节初始化
  rt_uint8_t uchCRCLo = 0xFF; // 低CRC 字节初始化
  rt_uint32_t uIndex; // CRC循环中的索引
  while (usDataLen--) // 传输消息缓冲区
  {
    uIndex = uchCRCHi ^ *puchMsg++; // 计算CRC
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex];
  }
  return (((rt_uint16_t)uchCRCLo << 8u) | uchCRCHi);
}

/**
 * @name: RS485_uart_irqhandler
 * @test: RS485中断函数
 * @msg: 
 * @param {int, void *} --- {中断号, void指针}
 * @return none
 */
void RS485_uart_irqhandler(int IRQn, void *param)
{
	rt_uint8_t res;
	rt_uint32_t tick;
    ls1c_uart_t uartx = uart_irqn_to_uartx(IRQn);
    void *uart_base = uart_get_base(uartx);
    unsigned char iir = reg_read_8(uart_base + LS1C_UART_IIR_OFFSET); //接收8位数据
    // 判断是否为接收超时或接收到有效数据
    if ((IIR_RXTOUT & iir) || (IIR_RXRDY & iir))
    {
         while (LSR_RXRDY & reg_read_8(uart_base + LS1C_UART_LSR_OFFSET))
        {
		    RS485_BUFF[uart_p]=reg_read_8(uart_base + LS1C_UART_DAT_OFFSET);
           // rt_kprintf("\r\nRS485BUFF[%d]=%x\r\n",uart_p,RS485_BUFF[uart_p]);
            uart_p++;
        }
	}
}

/**
 * @name: get_rs485buff
 * @test: 获取数据和长度
 * @msg: 
 * @param {type} 
 * @return {type} 
 */
rt_uint8_t get_rs485buff(rt_uint8_t rebuff[],rt_uint16_t *p)
{
    *p= uart_p; 
	int i=0;
    // if(RS485_BUFF[0] != 0)
    // {
        for(i=0; i<20; i++)
        {
            //rt_kprintf("\r\nRS485_BUFF[%d]=0%X\r\n",i,RS485_BUFF[i]);
            rebuff[i] = RS485_BUFF[i];
        }
        // rt_kprintf("\r\n接收： %X  %X  %X  %X  %X  %X  %X\r\n",RS485_BUFF[0], RS485_BUFF[1], RS485_BUFF[2], RS485_BUFF[3], RS485_BUFF[4], RS485_BUFF[5], RS485_BUFF[6]);
    // }
    
    return 1;
}



/**
 * @name: air_temperate_receive
 * @test: 室内空气温度处理接收函数
 * @msg: 
 * @param {int [], int [], long *} -- {接收到的485数据帧， 用于保存温度字节的数组， 过渡中间值的指针} 
 * @return none
 */
void air_temperate_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t air_temperature[],rt_int16_t *p)
{
    int i=0, j;
    rt_uint32_t CRC=0;
    rt_err_t uwRet = RT_EOK;
    for(i=0; i<20; i++)
    {
        if((Pro_BUFF[i]==1) && (Pro_BUFF[i+1]==3))      //地址码和功能码的匹配
        {
            *p = Pro_BUFF[i+2];
            rt_uint8_t CRC_BUFF[*p+3];
            for(j=0; j<*p; j++)
            {
                air_temperature[j]=Pro_BUFF[i+3+j];
            }
            for(j=0; j<*p+3; j++)
            {
                CRC_BUFF[j]=Pro_BUFF[i+j];
            }
            CRC=CRC16(CRC_BUFF, 7); 
            temperature_first=((air_temperature[0]<<8) | air_temperature[1]);
            temperature_first=change(temperature_first);
            rs485_recedata.air_temperate = temperature_first;
            
            sprintf(str_air_temperate, "%d.%d", temperature_first/10, temperature_first%10);
            onenet_mqtt_upload_string("air_temperate", str_air_temperate);
            rt_kprintf("buffer : {\"air_temperate\":%s}\r\n",str_air_temperate);
        }
    }
}

/**
 * @name: air_humidity_receive
 * @test: 室内空气湿度处理接收函数
 * @msg: 
 * @param {int [], int [], long *} -- {接收到的485数据帧， 用于保存温度字节的数组， 过渡中间值的指针} 
 * @return none
 */
void air_humidity_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t air_humidity[],rt_int16_t *p)
{
    int i=0, j;
    rt_uint32_t CRC=0;
    rt_err_t uwRet = RT_EOK;
    for(i=0; i<20; i++)
    {
        if((Pro_BUFF[i]==1) && (Pro_BUFF[i+1]==3))      //地址码和功能码的匹配
        {
            *p = Pro_BUFF[i+2];
            rt_uint8_t CRC_BUFF[*p+3];
            for(j=0; j<*p; j++)
            {
                air_humidity[j]=Pro_BUFF[i+3+j];
            }
            for(j=0; j<*p+3; j++)
            {
                CRC_BUFF[j]=Pro_BUFF[i+j];
            }
            CRC=CRC16(CRC_BUFF, 7); 

            humidity_end=((air_humidity[0]<<8) | air_humidity[1]);
            sprintf(str_air_humidity, "%d.%d", humidity_end/10, humidity_end%10);
            onenet_mqtt_upload_string("air_humidity", str_air_humidity);
            rt_kprintf("buffer : {\"air_humidity\":%s}\r\n",str_air_humidity);
        }
    }
}


/**
 * @name: air_lux_receive
 * @test: 室内光照度处理接收函数
 * @msg: 
 * @param {int [], int [], long *} -- {接收到的485数据帧， 用于保存温度字节的数组， 过渡中间值的指针} 
 * @return none
 */
void air_lux_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t air_lux[],rt_int16_t *p)
{
    int i=0, j;
    rt_uint32_t CRC=0;

    for(i=0; i<20; i++)
    {
        if((Pro_BUFF[i]==1) && (Pro_BUFF[i+1]==3))      //地址码和功能码的匹配
        {
            *p = Pro_BUFF[i+2];
            rt_uint8_t CRC_BUFF[*p+3];
            for(j=0; j<*p; j++)
            {
                air_lux[j]=Pro_BUFF[i+3+j];
            }
            for(j=0; j<*p+3; j++)
            {
                CRC_BUFF[j]=Pro_BUFF[i+j];
            }
            CRC=CRC16(CRC_BUFF, 7); 

            lux_high=((air_lux[0]<<8) | air_lux[1]);
            lux_low=((air_lux[2]<<8) | air_lux[3]);
            lux_end=100*lux_high + lux_low;
            sprintf(str_air_lux, "%dlux", lux_end);
            onenet_mqtt_upload_digit("air_lux", lux_end);
            rt_kprintf("buffer : {\"air_lux\":%s}\r\n",str_air_lux);
        }
    }
}

/**
 * @name: air_co2_receive
 * @test: 室内二氧化碳含量处理接收函数
 * @msg: 
 * @param {int [], int [], long *} -- {接收到的485数据帧， 用于保存温度字节的数组， 过渡中间值的指针} 
 * @return none
 */
void air_co2_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t air_co2[],rt_int16_t *p)
{
    int i=0, j;
    rt_uint32_t CRC=0;

    for(i=0; i<20; i++)
    {
        if((Pro_BUFF[i]==1) && (Pro_BUFF[i+1]==3) && (Pro_BUFF[i+4]!=0))      //地址码和功能码的匹配
        {
            *p = Pro_BUFF[i+2];
            rt_uint8_t CRC_BUFF[*p+3];
            for(j=0; j<*p; j++)
            {
                air_co2[j]=Pro_BUFF[i+3+j];
            }
            for(j=0; j<*p+3; j++)
            {
                CRC_BUFF[j]=Pro_BUFF[i+j];
            }
            CRC=CRC16(CRC_BUFF, 7); 

            co2_end=((air_co2[0]<<8) | air_co2[1]);
            sprintf(str_air_co2, "%dppm", co2_end);
            onenet_mqtt_upload_digit("air_co2", co2_end);
            rt_kprintf("buffer : {\"air_co2\":%s}\r\n",str_air_co2);
        }
    }
}

/**
 * @name: soil_humidity_receive
 * @test: 土壤湿度即水分含量处理接收函数
 * @msg: 
 * @param {int [], int [], long *} -- {接收到的485数据帧， 用于保存温度字节的数组， 过渡中间值的指针} 
 * @return none
 */
void soil_humidity_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t soil_humidity[],rt_int16_t *p)
{
    int i=0, j;
    rt_uint32_t CRC=0;

    for(i=0; i<20; i++)
    {
        if((Pro_BUFF[i]==6) && (Pro_BUFF[i+1]==3) && (Pro_BUFF[i+2]==2))      //地址码和功能码的匹配
        {
            *p = Pro_BUFF[i+2];
            rt_uint8_t CRC_BUFF[*p+3];
            for(j=0; j<*p; j++)
            {
                soil_humidity[j]=Pro_BUFF[i+3+j];
            }
            for(j=0; j<*p+3; j++)
            {
                CRC_BUFF[j]=Pro_BUFF[i+j];
            }
            CRC=CRC16(CRC_BUFF, 7); 

            soil_humidty_end=((soil_humidity[0]<<8) | soil_humidity[1]);
            sprintf(str_soil_humidity, "%d.%d", soil_humidty_end/10, soil_humidty_end%10);
            onenet_mqtt_upload_string("soil_humidity", str_soil_humidity);
            rt_kprintf("buffer : {\"soil_humidity\":%s}\r\n",str_soil_humidity);
        }
    }
}

/**
 * @name: soil_temperate_receive
 * @test: 土壤温度处理接收函数
 * @msg: 
 * @param {int [], int [], long *} -- {接收到的485数据帧， 用于保存温度字节的数组， 过渡中间值的指针} 
 * @return none
 */
void soil_temperate_receive(rt_uint8_t Pro_BUFF[],rt_uint8_t soil_temperate[],rt_int16_t *p)
{
    int i=0, j;
    rt_uint32_t CRC=0;

    for(i=0; i<20; i++)
    {
        if((Pro_BUFF[i]==6) && (Pro_BUFF[i+1]==3))      //地址码和功能码的匹配
        {
            *p = Pro_BUFF[i+2];
            rt_uint8_t CRC_BUFF[*p+3];
            for(j=0; j<*p; j++)
            {
                soil_temperate[j]=Pro_BUFF[i+3+j];
            }
            for(j=0; j<*p+3; j++)
            {
                CRC_BUFF[j]=Pro_BUFF[i+j];
            }
            CRC=CRC16(CRC_BUFF, 7); 

            soil_temperate_end=((soil_temperate[0]<<8) | soil_temperate[1]);
            sprintf(str_soil_temperate, "%d.%d", soil_temperate_end/10, soil_temperate_end%10);
            onenet_mqtt_upload_string("soil_temperate", str_soil_temperate);
            rt_kprintf("buffer : {\"soil_temperate\":%s}\r\n\n\n",str_soil_temperate);
            rt_thread_delay(200);
        }
    }
}


/**
 * @name:  clean_buff
 * @test:  字符串清空
 * @msg:   
 * @param  none 
 * @return none
 */
void clean_buff(void)
{
    rt_uint16_t i = RS485_BUFF_SIZE+1;
    uart_p=0;

    while(i)
    {
        RS485_BUFF[i--]=0;
    }
}

/**
 * @name: RS485_Send
 * @test: 485发送问询数据帧
 * @msg: 
 * @param {char *, int} -- {字符指针, 整型} 
 * @return 0 
 */
int RS485_Send(char *p, int len)
{
    //rt_kprintf("\r\n发送： %X  %X  %X  %X  %X  %X  %X\r\n",p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
    int i=0;

    gpio_set(RD_GPIO, gpio_level_high); //485进入发送状态
    rt_thread_delay(10);
    for(i=0; i<len; i++)
    {
        uart_putc(4, *p);
        p++;
    }
    rt_thread_delay(1);
    gpio_set(RD_GPIO, gpio_level_low); //485进入接收状态
    rt_thread_delay(10);
    gpio_set(RD_GPIO, gpio_level_high); //485进入发送状态
    return 0;
}

short int change(short int temperature)
{
	short int temperature_value;
	if(temperature&0x8000) temperature_value=(~(temperature_value&0x7FFF)+1)+1;
	if((temperature&0x8000)==0)
        temperature_value=temperature;
	return temperature_value;
}


/**
 * @name: RS485_Init
 * @test: 485串口初始化
 * @msg: 波特率   RX     TX    UART
 *       4800   GPIO0  GPIO1  UART3
 * @param  none
 * @return none
 */
void  RS485_Init(void)
{
	// 调试串口信息
	ls1c_uart_info_t RS485_uart_info = {0};	

    unsigned int tx_gpio = 1;
    unsigned int rx_gpio = 0;
    // 初始化RE/DE_GPIO  
    gpio_init(RD_GPIO, gpio_mode_output);
    gpio_set(RD_GPIO, gpio_level_low);       // 进入接收状态
    // 设置复用
    pin_set_remap(tx_gpio, PIN_REMAP_FOURTH);
    pin_set_remap(rx_gpio, PIN_REMAP_FOURTH);
    
    // 初始化相关寄存器
    RS485_uart_info.UARTx = LS1C_UART3;
    RS485_uart_info.baudrate = 4800;
    RS485_uart_info.rx_enable = TRUE;  
    uart_init(&RS485_uart_info);
	rt_hw_interrupt_umask(LS1C_UART3_IRQ);		
    rt_hw_interrupt_install(LS1C_UART3_IRQ, RS485_uart_irqhandler, RT_NULL, "UART3");
    rt_kprintf("RS485_Init done!\r\n");
}

