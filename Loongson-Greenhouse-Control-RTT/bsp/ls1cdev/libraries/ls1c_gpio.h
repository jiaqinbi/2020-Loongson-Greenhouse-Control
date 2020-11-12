/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-09-06     ��Ϊ��       first version
 */


#ifndef __OPENLOONGSON_GPIO_H
#define __OPENLOONGSON_GPIO_H



// ��о1c��gpio�ǰ���0,1,2,3,4...������˳���ŵģ�
// ���ڲ����Ĵ�����ʱ�����ǰ���ÿ32��һ�����ֵ�
// ��������������ԣ���ÿ���32��gpio����һ��"port"��ÿ��gpio��ÿ���е�������"pin"
// port = gpio / 32
// pin  = gpio % 32
// ����GPIO50��port=1,pin=18
#define GPIO_GET_PORT(gpio)                 ((gpio) / 32)
#define GPIO_GET_PIN(gpio)                  ((gpio) % 32)


// gpio�Ĺ���ģʽ--���롢���
typedef enum{
    gpio_mode_output = 0,       // ���
    gpio_mode_input = 1         // ����
}gpio_mode_t;


//  gpio�ߵ͵�ƽֵ
typedef enum{
    gpio_level_low = 0,         // �͵�ƽ
    gpio_level_high = 1         // �ߵ�ƽ
}gpio_level_t;


typedef enum {
    // �����ش���
	IRQ_TYPE_EDGE_RISING	= 0x00000001,
	// �½��ش���
	IRQ_TYPE_EDGE_FALLING	= 0x00000002,
	IRQ_TYPE_EDGE_BOTH	= (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING),
	// �ߵ�ƽ����
	IRQ_TYPE_LEVEL_HIGH	= 0x00000004,
	// �͵�ƽ����
	IRQ_TYPE_LEVEL_LOW	= 0x00000008,
	IRQ_TYPE_LEVEL_MASK	= (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH),	
}gpio_irq_type_t;



/*
 * ��ȡָ��gpio��CFG�Ĵ���
 * @gpio gpio���
 * @ret CFG�Ĵ���
 */
volatile unsigned int *gpio_get_cfg_reg(unsigned int gpio);

/*
 * gpio��ʼ��
 * @gpio gpio���ţ�ȡֵ��Χ[0, 127]
 * @mode gpio�Ĺ���ģʽ(���롢���)
 *
 * ��: ��gpio50��ʼ��Ϊ���
 * gpio_init(50, gpio_mode_output);
 */
void gpio_init(unsigned int gpio, gpio_mode_t mode);


/*
 * ��ָ��gpio����ߵ�ƽ��͵�ƽ
 * @gpio gpio���ţ�ȡֵ��Χ[0, 127]
 * @level ��ƽֵ
 *
 * ��: ��gpio50������͵�ƽ
 * gpio_set(50, gpio_level_low);
 */
void gpio_set(unsigned int gpio, gpio_level_t level);



/*
 * ��ȡָ��gpio���ŵ�ֵ
 * @gpio gpio���ţ�ȡֵ��Χ[0,127]
 *
 * ��: ��ȡgpio50�����ϵ�ֵ
 * gpio_level_t level;
 * level = gpio_get(50);
 */
unsigned int gpio_get(unsigned int gpio);



/**
 * �����ж�����
 * @gpio gpio����
 * @type �����жϵ��������ߵ�ƽ�������͵�ƽ�����������ش��� or �½��ش���
 */
void gpio_set_irq_type(unsigned int gpio, gpio_irq_type_t type);



#endif

