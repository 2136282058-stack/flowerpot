#include "gpio.h"


/****
*******	LED IO初始化
*****/    
void Gpio_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LEDS_GPIO_CLK|RELAY_FS_GPIO_CLK|RELAY_QG_GPIO_CLK,ENABLE);			    //使能端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	

	GPIO_InitStructure.GPIO_Pin  = LEDS_GPIO_PIN;  						//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					//设置成推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO口速度为50MHz
 	GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);						//根据设定参数初始化
	GPIO_SetBits(LEDS_GPIO_PORT,LEDS_GPIO_PIN);  							//IO口输出高

	//GPIO_InitStructure.GPIO_Pin  = LED_PIN;  								//引脚配置
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 				//设置成推挽输出
 	//GPIO_Init(LED_PORT, &GPIO_InitStructure);								//根据设定参数初始化
	//GPIO_SetBits(LED_PORT,LED_PIN);  												//IO口输出高

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					//设置成推挽输出
	GPIO_InitStructure.GPIO_Pin  = BEEP_PIN;									//引脚配置
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);								//根据设定参数初始化
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN); 								 			//IO口输出低

	GPIO_InitStructure.GPIO_Pin  = RELAY_FS_PIN|RELAY_SB_PIN|RELAY_BG_PIN|RELAY_JR_PIN;	//引脚配置
 	GPIO_Init(RELAY_FS_PORT, &GPIO_InitStructure);						//根据设定参数初始化
	GPIO_ResetBits(RELAY_FS_PORT,RELAY_FS_PIN|RELAY_SB_PIN|RELAY_BG_PIN|RELAY_JR_PIN);		//IO口输出高

	GPIO_InitStructure.GPIO_Pin  = RELAY_QG_PIN;									//引脚配置
 	GPIO_Init(RELAY_QG_PORT, &GPIO_InitStructure);								//根据设定参数初始化
	GPIO_ResetBits(RELAY_QG_PORT,RELAY_FS_PIN);										//IO口输出高

	GPIO_InitStructure.GPIO_Pin  = CHECK_R_GPIO_PIN;  						//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;					//设置成推挽输出
 	GPIO_Init(CHECK_R_GPIO_PORT, &GPIO_InitStructure);						//根据设定参数初始化

	GPIO_InitStructure.GPIO_Pin  = CHECK_H_GPIO_PIN;  						//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;					//设置成推挽输出
 	GPIO_Init(CHECK_H_GPIO_PORT, &GPIO_InitStructure);						//根据设定参数初始化

}

