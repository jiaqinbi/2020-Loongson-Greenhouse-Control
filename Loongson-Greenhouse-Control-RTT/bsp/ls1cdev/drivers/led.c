// led接口
// 使用低电平点亮led，高电平熄灭led


#include "../libraries/ls1c_gpio.h"
#include "led.h"

/**
 * @brief  初始化led
 * @param  led_gpio led所在gpio引脚
 * @return None
 * @note   None
 */
void led_init(unsigned int led_gpio)
{
    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);        // 指示灯默认熄灭

    return ;
}

/**
 * @brief  点亮led
 * @param  led_gpio led所在gpio引脚
 * @return None
 * @note   None
 */
void led_on(unsigned int led_gpio)
{
    gpio_set(led_gpio, gpio_level_low);

    return ;
}

/**
 * @brief  熄灭led
 * @param  led_gpio led所在gpio引脚
 * @return None
 * @note   None
 */
void led_off(unsigned int led_gpio)
{
    gpio_set(led_gpio, gpio_level_high);

    return ;
}




