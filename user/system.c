#include "system.h"
#include "stm32f10x.h"

/*系统初始化*/
void systemInit(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置中断优先级分组
    SystemInit();
    delay_init();	
    Gpio_Init();		  									//IO初始化
    Key_Init();		  										//按键初始化
    Usart1_Init(9600);    							//蓝牙串口1初始化
    USART2_Init(115200);                              //语音串口2初始化
    Adc_Init();													//ADC初始化
    Step_Motor_Init();									//步进电机初始化
    DS18B20_Init();
    SGP30_Init();												//SGP30初始化
	Lcd_Init();
    Lcd_Clear(0xffff);
    
}
