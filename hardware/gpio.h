#ifndef __GPIO_H
#define __GPIO_H


/**********************************
包含头文件
**********************************/
#include "sys.h"


/**********************************
重定义关键词
**********************************/
#define LEDS_GPIO_CLK                 RCC_APB2Periph_GPIOC        //最小系统LED
#define LEDS_GPIO_PORT                GPIOC
#define LEDS_GPIO_PIN                 GPIO_Pin_13
#define LED_SYS                       PCout(13)

//#define LED_GPIO_CLK                  RCC_APB2Periph_GPIOB				//LED引脚
//#define LED_PORT                      GPIOB
//#define LED_PIN                       GPIO_Pin_0
//#define LED 									    		PBout(0)

#define BEEP_GPIO_CLK                 RCC_APB2Periph_GPIOB				//蜂鸣器引脚
#define BEEP_PORT                     GPIOB
#define BEEP_PIN                      GPIO_Pin_1
#define BEEP 									    		PBout(1)

#define CHECK_R_GPIO_CLK              RCC_APB2Periph_GPIOB        //雨水检测引脚
#define CHECK_R_GPIO_PORT             GPIOB
#define CHECK_R_GPIO_PIN              GPIO_Pin_11
#define CHECK_R                       PBin(11)

#define CHECK_H_GPIO_CLK              RCC_APB2Periph_GPIOA        //人体检测引脚
#define CHECK_H_GPIO_PORT             GPIOA
#define CHECK_H_GPIO_PIN              GPIO_Pin_11
#define CHECK_H                       PAin(11)

#define RELAY_FS_GPIO_CLK             RCC_APB2Periph_GPIOA				//风扇继电器引脚
#define RELAY_FS_PORT                 GPIOA
#define RELAY_FS_PIN                  GPIO_Pin_4
#define RELAY_FS 											PAout(4)

#define RELAY_JR_GPIO_CLK             RCC_APB2Periph_GPIOA				//加热继电器引脚
#define RELAY_JR_PORT                 GPIOA
#define RELAY_JR_PIN                  GPIO_Pin_5
#define RELAY_JR 											PAout(5)

#define RELAY_SB_GPIO_CLK             RCC_APB2Periph_GPIOA				//水泵继电器引脚
#define RELAY_SB_PORT                 GPIOA
#define RELAY_SB_PIN                  GPIO_Pin_6
#define RELAY_SB 											PAout(6)

#define RELAY_BG_GPIO_CLK             RCC_APB2Periph_GPIOA				//补光继电器引脚
#define RELAY_BG_PORT                 GPIOA
#define RELAY_BG_PIN                  GPIO_Pin_7
#define RELAY_BG 											PAout(7)

#define RELAY_QG_GPIO_CLK             RCC_APB2Periph_GPIOB				//驱赶继电器引脚
#define RELAY_QG_PORT                 GPIOB
#define RELAY_QG_PIN                  GPIO_Pin_10
#define RELAY_QG 											PBout(10)


/**********************************
函数声明
**********************************/
void Gpio_Init(void);													//GPIO初始化


#endif
