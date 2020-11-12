/*
 * @filename: main.c
 * @Descripttion: 
 * @version: V1.0
 * @Author: Bi Jiaqin
 * @Date: 2020-10-10 08:45:20
 * @LastEditors: Bi Jiaqin
 * @LastEditTime: 2020-11-08 18:28:41
 */ 

#include <rtthread.h>
#include <string.h>
#include <rtdbg.h>
#include <drivers/pin.h>
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "ls1c_pin.h"
#include "modbus.h"
#include "onenet.h"
#include "../drivers/oled.h"

#define DBG_ENABLE
#define DBG_COLOR
#define DBG_SECTION_NAME "onenet.mqtt"
#if ONENET_DEBUG
#define DBG_LEVEL DBG_LOG
#else
#define DBG_LEVEL DBG_INFO
#endif /* ONENET_DEBUG */

//=============================================================//
int esp8266_conn_flag = 0;
static rt_sem_t rs485init_sem = RT_NULL;        /* rs485初始化信号量 */
static rt_sem_t mqttinit_sem = RT_NULL;         /* mqtt初始化信号量 */

// rt_mq_t air_temperate_mq = RT_NULL;          /* 数据采集成功的消息队列 */
// rt_mq_t air_humidity_mq = RT_NULL;           /* 数据采集成功的消息队列 */

static rt_thread_t rs485_init_thread;           /* rs485初始化的线程的句柄 */
static rt_thread_t onenet_mqtt_init_thread;     /* mqtt初始化的线程的句柄 */
static rt_thread_t onenet_send_thread;          /* post数据到onenet线程的句柄 */
static rt_thread_t oled_show_thread;            /* oled初始化以及数据显示线程的句柄 */
static rt_thread_t data_acquisition_thread;     /* 数据采集线程的句柄 */

/* 云平台、微信操作设备状态  */
gpio_level_t led_status = 0;
gpio_level_t light_status = 0;
gpio_level_t pump_status = 0;

static rt_uint32_t oled_show_count; /* OLED页面翻转时间计时 */

/* 定义在modbus.c中，用于OLED显示使用 */
extern char str_air_temperate[20];
extern char str_air_humidity[20];
extern char str_air_lux[20];
extern char str_air_co2[20];
extern char str_soil_temperate[20];
extern char str_soil_humidity[20];

rt_uint8_t receive[20]; /* 接收485返回的数据帧 */

//MODBUS数据帧格式 [地址码   功能码   寄存器起始地址（两字节分高位和低位）  寄存器长度  校验码低位   校验码高位]
rt_uint8_t air_temperate[8] = {0x01, 0x03, 0x01, 0xF5, 0x00, 0x01, 0x95, 0xC4};  //室内空气温度         应答帧为{01 03 02 00 E0 B9 CC}，其中00 E0为温度值，02为有效字节数,最后两位为校验码低位和高位
rt_uint8_t air_humidity[8] = {0x01, 0x03, 0x01, 0xF4, 0x00, 0x01, 0xC4, 0x04};   //室内空气湿度         应答帧为{01 03 02 01 F6 39 92}，其中00 E0为温度值，02为有效字节数,最后两位为校验码低位和高位
rt_uint8_t air_lux[8] = {0x01, 0x03, 0x01, 0xFA, 0x00, 0x02, 0xE5, 0xC6};        //室内光照度           应答帧为{01 03 04 00 00 00 40 FB C3}，其中，04为有效字节数，00 00 00 40为光照度，即56lux,FB C3为校验码分别低位和高位
rt_uint8_t air_co2[8] = {0x01, 0x03, 0x01, 0xF7, 0x00, 0x01, 0x34, 0x04};        //室内空气二氧化碳含量  应答帧为{01 03 02 02 FD 78 A5}，其中 02 FD为二氧化碳含量，即765ppm，每立方米的空气中含有765ml二氧化碳
rt_uint8_t soil_humidity[8] = {0x06, 0x03, 0x00, 0x00, 0x00, 0x01, 0x85, 0xBD};  //土壤湿度水分含量      应答帧为{02 03 02 00 4E 7C 70},其中 00 4E为土壤湿度值，扩大了十倍，即7.8%
rt_uint8_t soil_temperate[8] = {0x06, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD4, 0x7D}; //土壤温度             应答帧为{02 03 02 00 F7 BD C2}，其中 00F7为土壤温度值，扩大了十倍，即24.7℃
//=============================================================//


/* onenet mqtt command response callback function */
static void onenet_cmd_process(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size);


/**
 * @name: device_status_switch
 * @test: test font
 * @msg: 
 * @param {type} 
 * @return {type} 
 */
void device_status_switch(void)
{
    led_status = gpio_get(52);
    light_status = gpio_get(86);
    pump_status  = gpio_get(90);
    

    if (light_status == 1)  /* 照明灯 */
    {
        onenet_mqtt_upload_digit("light_status", 2);
        rt_kprintf("buffer : {\"light_status\":%d}\r\n", light_status);
    }
    else
    {
        onenet_mqtt_upload_digit("light_status", 3);
        rt_kprintf("buffer : {\"light_status\":%d}\r\n", light_status);
    }

    if (pump_status == 1)  /* 水泵 */
    {
        onenet_mqtt_upload_digit("pump_status", 4);
        rt_kprintf("buffer : {\"pump_status\":%d}\r\n", pump_status);
    }
    else
    {
        onenet_mqtt_upload_digit("pump_status", 5);
        rt_kprintf("buffer : {\"pump_status\":%d}\r\n", light_status);
    }

    if (led_status == 0)  /* 底板灯 */
    {
        onenet_mqtt_upload_digit("led_status", 0);
        rt_kprintf("buffer : {\"led_status\":%d}\r\n", led_status);
    }
    else
    {
        onenet_mqtt_upload_digit("led_status", 1);
        rt_kprintf("buffer : {\"led_status\":%d}\r\n", led_status);
    }
}
/**
 * @name: air_temperate_cal
 * @test: 室内空气温度计算
 * @msg: 
 * @param  none 
 * @return none 
 */
void air_temperate_cal(void)
{
    rt_uint16_t i = 0, p = 0;

    RS485_Send(air_temperate, 8);
    get_rs485buff(receive, &i);
    clean_buff();
    air_temperate_receive(receive, RS485_temperature, &p);
}

/**
 * @name: air_humidity_cal
 * @test: 室内空气湿度计算
 * @msg: 
 * @param  none 
 * @return none 
 */
void air_humidity_cal(void)
{
    rt_uint16_t i = 0, p = 0;

    RS485_Send(air_humidity, 8);
    get_rs485buff(receive, &i);
    clean_buff();
    air_humidity_receive(receive, RS485_humidity, &p);
}

/**
 * @name: air_lux_cal
 * @test: 室内光照度计算
 * @msg: 
 * @param  none 
 * @return none 
 */
void air_lux_cal(void)
{
    rt_uint16_t i = 0, p = 0;

    RS485_Send(air_lux, 8);
    get_rs485buff(receive, &i);
    clean_buff();
    air_lux_receive(receive, RS485_lux, &p);
}

/**
 * @name: air_co2_cal
 * @test: 室内二氧化碳含量计算
 * @msg: 
 * @param  none 
 * @return none 
 */
void air_co2_cal(void)
{
    rt_uint16_t i = 0, p = 0;

    RS485_Send(air_co2, 8);
    get_rs485buff(receive, &i);
    clean_buff();
    air_co2_receive(receive, RS485_co2, &p);
}

/**
 * @name: soil_humidity_cal
 * @test: 土壤湿度即水分含量计算
 * @msg: 
 * @param  none 
 * @return none 
 */
void soil_humidity_cal(void)
{
    rt_uint16_t i = 0, p = 0;

    RS485_Send(soil_humidity, 8);
    get_rs485buff(receive, &i);
    clean_buff();
    soil_humidity_receive(receive, RS485_soil_humidity, &p);
}

/**
 * @name: soil_temperate_cal
 * @test: 土壤温度计算
 * @msg: 
 * @param  none 
 * @return none 
 */
void soil_temperate_cal(void)
{
    rt_uint16_t i = 0, p = 0;

    RS485_Send(soil_temperate, 8);
    get_rs485buff(receive, &i);
    clean_buff();
    soil_temperate_receive(receive, RS485_soil_temperate, &p);
}

/**
 * @name:  data_acquisition_entry
 * @test:  接收485数据入口函数,发送至OLED并上传到ONENET和微信
 * @msg:   接收485数据入口函数,发送至OLED并上传到ONENET和微信
 * @param  none
 * @return none
 */
void data_acquisition_entry(void *parameter)
{
    // /* 永久等待mqttinit_sem信号量，收不到则该线程一直挂起 */
    // rt_sem_take(mqttinit_sem, RT_WAITING_FOREVER);
    // /* 用完则删除mqttinit_sem信号量 */
    // rt_sem_delete(mqttinit_sem);

    while (1)
    {
        onenet_set_cmd_rsp_cb(onenet_cmd_process);
        air_temperate_cal();
        air_humidity_cal();
        air_lux_cal();
        air_co2_cal();
        soil_humidity_cal();
        soil_temperate_cal();
        device_status_switch();
    }
}

static void rs485_init_entry(void *parameter)
{
    RS485_Init();
    rt_sem_release(rs485init_sem);
}

static void onenet_mqtt_init_entry(void *parameter)
{
    rt_uint8_t onenet_mqtt_init_failed_times;
    rt_err_t uwRet = RT_EOK;

    /* 永久等待rs485init_sem信号量，收不到则该线程一直挂起 */
    rt_sem_take(rs485init_sem, RT_WAITING_FOREVER);
    /* 用完则删除rs485init_sem信号量 */
    rt_sem_delete(rs485init_sem);

    /* mqtt初始化 */
    while (1)
    {
        if (!onenet_mqtt_init())
        {
            /* mqtt初始化成功之后，释放信号量告知onenet_upload_data_thread线程可以上传数据了 */
            rt_sem_release(mqttinit_sem);
            /* 同时删除485初始化线程 */
            uwRet = rt_thread_delete(rs485_init_thread);
            if (uwRet == RT_EOK)
            {
                rt_kprintf("[%s] Info->rs485_init_thread delete SUCC! %s, line:%d \r\n", __FILE__, __FUNCTION__, __LINE__);
            }
            else
            {
                rt_kprintf("[%s] Info->rs485_init_thread delete FAILED! %s, line:%d \r\n", __FILE__, __FUNCTION__, __LINE__);
            }
            return;
        }
        rt_thread_mdelay(100);
        LOG_E("onenet mqtt init failed %d times \r\n", onenet_mqtt_init_failed_times++);
    }
}

/**
 * @name: oled_show_thread_entry
 * @test: OLED数据显示函数
 * @msg: 
 * @param  {NULL}
 * @return {none}
 */
void oled_show_thread_entry(void *parameter)
{
    int i = 0;
    OLED_Init();
    while (1)
    {
        if (oled_show_count < 30)
        {
            // OLED_Clear();

            OLED_ShowString(70, 0, "    ");
            OLED_ShowString(70, 2, "    ");

            OLED_ShowCHinese(0, 0, 0);   //室
            OLED_ShowCHinese(16, 0, 1);  //内
            OLED_ShowCHinese(32, 0, 2);  //温
            OLED_ShowCHinese(48, 0, 3);  //度
            OLED_ShowChar(62, 0, ':');   //：
            OLED_ShowCHinese(110, 0, 5); //℃

            OLED_ShowCHinese(0, 2, 0);  //室
            OLED_ShowCHinese(16, 2, 1); //内
            OLED_ShowCHinese(32, 2, 4); //湿
            OLED_ShowCHinese(48, 2, 3); //度
            OLED_ShowChar(62, 2, ':');  //：
            OLED_ShowChar(105, 2, '%'); //%

            OLED_ShowCHinese(0, 4, 0);  //室
            OLED_ShowCHinese(16, 4, 1); //内
            OLED_ShowCHinese(32, 4, 6); //光
            OLED_ShowCHinese(48, 4, 7); //照
            OLED_ShowChar(62, 4, ':');  //：

            OLED_ShowCHinese(0, 6, 10);  //二
            OLED_ShowCHinese(16, 6, 11); //氧
            OLED_ShowCHinese(32, 6, 12); //化
            OLED_ShowCHinese(48, 6, 13); //碳
            OLED_ShowChar(62, 6, ':');   //：
            
            OLED_ClearPoint(70, 0);
            OLED_ClearPoint(70, 2);

            OLED_ShowString(70, 0, str_air_temperate);
            OLED_ShowString(70, 2, str_air_humidity);
            OLED_ShowString(70, 4, str_air_lux);
            OLED_ShowString(70, 6, str_air_co2);
        }
        else
        {
            OLED_ShowString(0, 4, "    ");  
            OLED_ShowString(16, 4, "    "); 
            OLED_ShowString(32, 4, "    "); 
            OLED_ShowString(48, 4, "    "); 
            OLED_ShowString(62, 4, "    "); 
            OLED_ShowString(0, 6, "    ");  
            OLED_ShowString(16, 6, "    "); 
            OLED_ShowString(32, 6, "    "); 
            OLED_ShowString(48, 6, "    "); 
            OLED_ShowString(62, 6, "    ");  
            OLED_ShowString(70, 0, "    ");
            OLED_ShowString(70, 2, "    ");
            OLED_ShowString(70, 4, "       ");
            OLED_ShowString(70, 6, "       ");
            // OLED_Clear();
            OLED_ShowCHinese(0, 0, 8);   //土
            OLED_ShowCHinese(16, 0, 9);  //壤
            OLED_ShowCHinese(32, 0, 2);  //温
            OLED_ShowCHinese(48, 0, 3);  //度
            OLED_ShowChar(62, 0, ':');   //：
            OLED_ShowCHinese(110, 0, 5); //℃

            OLED_ShowCHinese(0, 2, 8);  //土
            OLED_ShowCHinese(16, 2, 9); //壤
            OLED_ShowCHinese(32, 2, 4); //湿
            OLED_ShowCHinese(48, 2, 3); //度
            OLED_ShowChar(62, 2, ':');  //：
            OLED_ShowChar(105, 2, '%'); //%

            OLED_ClearPoint(70, 0);
            OLED_ClearPoint(70, 2);
            OLED_ShowString(70, 0, str_soil_temperate);
            OLED_ShowString(70, 2, str_soil_humidity);

            
        }
        oled_show_count++;
        if (oled_show_count == 60)
        {
            oled_show_count = 0;
        }

        rt_thread_delay(10);
    }
}

int main(int argc, char **argv)
{

    gpio_init(52, gpio_mode_output);  //底板LED测试
    gpio_init(88, gpio_mode_output);  //LED
    gpio_init(86, gpio_mode_output);  //照明灯
    gpio_init(90, gpio_mode_output);  //水泵
    gpio_set(86, gpio_level_high);    
    gpio_set(90, gpio_level_high);
    gpio_set(88, gpio_level_high);
    gpio_set(52, gpio_level_low);

    rs485init_sem = rt_sem_create("rs485_sem", RT_NULL, RT_IPC_FLAG_FIFO);
    RT_ASSERT(rs485init_sem);
    mqttinit_sem = rt_sem_create("mqtt_sem", RT_NULL, RT_IPC_FLAG_FIFO);
    RT_ASSERT(mqttinit_sem);

    /* 485初始化线程 */
    rs485_init_thread = rt_thread_create("rs485init", rs485_init_entry, RT_NULL,
                                         1024, RT_THREAD_PRIORITY_MAX / 2 - 2, 20);
    if (rs485_init_thread != RT_NULL)
    {
        rt_thread_startup(rs485_init_thread);
    }

    /* MQTT初始化线程 */
    onenet_mqtt_init_thread = rt_thread_create("mqttinit", onenet_mqtt_init_entry, RT_NULL,
                                               1024, RT_THREAD_PRIORITY_MAX / 2 - 2, 20);
    if (onenet_mqtt_init_thread != RT_NULL)
    {
        rt_thread_startup(onenet_mqtt_init_thread);
    }

    /* OLED显示线程 */
    oled_show_thread = rt_thread_create("oled_show", oled_show_thread_entry, RT_NULL,
                                        1024, RT_THREAD_PRIORITY_MAX / 2, 20);
    if (oled_show_thread != RT_NULL)
    {
        rt_thread_startup(oled_show_thread);
    }

    /* 数据采集线程 */
    data_acquisition_thread = rt_thread_create("data_acquisition", data_acquisition_entry,
                                               RT_NULL, 4096,
                                               RT_THREAD_PRIORITY_MAX / 2 - 2, 20);
    if (data_acquisition_thread != RT_NULL)
    {
        rt_thread_startup(data_acquisition_thread);
    }

    return 0;
}

/* onenet mqtt command response callback function */
static void onenet_cmd_process(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    char res_buf[20] = {0};
    char recv_buf[5] = {0};
    /**********接收对应字符**************/
    /*  “0”  <--->  底板P52引脚LED点亮  */
    /*  “1”  <--->  底板P52引脚LED熄灭  */
    /*  “2”  <--->  P86引脚控制照明灯亮 */
    /*  “3”  <--->  P86引脚控制照明灯灭 */
    /*  “4”  <--->  P90引脚水泵运行     */
    /*  “5”  <--->  P90引脚水泵不运行   */
    /***********************************/
    rt_kprintf("receive data %s\r\n", recv_data);
    if (!strncmp((char *)recv_data, "0", 1))
    {
        rt_kprintf("led is on\n");

        gpio_set(52, gpio_level_low);
    }
    if (!strncmp((char *)recv_data, "1", 1))
    {
        gpio_set(52, gpio_level_high);
        rt_kprintf("led is off\n");
    }

    //照明灯状态命令接收以及处理
    if (!strncmp((char *)recv_data, "2", 1))
    {
        gpio_set(86, gpio_level_high);
        rt_kprintf("light is on\n");
    }
    if (!strncmp((char *)recv_data, "3", 1))
    {
        gpio_set(86, gpio_level_low);
        rt_kprintf("light is off\n");
    }

    //水泵状态命令接收以及处理
    if (!strncmp((char *)recv_data, "4", 1))
    {
        gpio_set(90, gpio_level_high);
        rt_kprintf("pump is on\n");
    }
    if (!strncmp((char *)recv_data, "5", 1))
    {
        gpio_set(90, gpio_level_low);
        rt_kprintf("pump is off\n");
    }
}
