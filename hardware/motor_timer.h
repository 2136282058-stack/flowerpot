#ifndef __MOTOR_TIMER_H
#define __MOTOR_TIMER_H

/**********************************
包含头文件
**********************************/
#include "sys.h" 

/**********************************
宏定义
**********************************/
#define           STEP_SPEED			            1					//电机速度，数值越大越慢（建议3-10）

// 定时器配置 - 改为 TIM2
#define           STEP_TIM                    TIM2
#define           STEP_TIM_APBxClock_FUN      RCC_APB1PeriphClockCmd  // TIM2 在 APB1 总线上
#define           STEP_TIM_CLK                RCC_APB1Periph_TIM2
#define           STEP_TIM_IRQ                TIM2_IRQn
#define           STEP_TIM_IRQHandler         TIM2_IRQHandler

// 步进电机2引脚配置
#define           STEP2_GPIO_CLK              RCC_APB2Periph_GPIOB
#define           STEP2_GPIO_PORT             GPIOB
#define           STEP2_GPIO_PIN_A            GPIO_Pin_15
#define           STEP2_GPIO_PIN_B            GPIO_Pin_14
#define           STEP2_GPIO_PIN_C            GPIO_Pin_13
#define           STEP2_GPIO_PIN_D            GPIO_Pin_12
#define           STEP2_A                     PBout(15)
#define           STEP2_B                     PBout(14)
#define           STEP2_C                     PBout(13)
#define           STEP2_D                     PBout(12)

// 28BYJ-48步进电机参数（8拍模式）
#define           STEP2_STEPS_PER_REV        512    // 每圈步数 (64×8)
#define           STEP2_DEGREE_PER_STEP      0.703125f  // 每步角度 (360/512)

/**********************************
外部变量声明
**********************************/
extern _Bool flag_step2_o;				//2号步进电机正转标志位
extern _Bool flag_step2_c;				//2号步进电机反转标志位
extern _Bool flag_Step2;					//2号步进电机状态标志位
extern int target_steps2;								//目标步数
extern int current_steps2;							//当前步数
extern int remaining_steps2;						//剩余步数

/**********************************
函数声明
**********************************/
void Step_Motor_Init(void);					//步进电机初始化
void Step2_Rotate_Degree(float degree);		//旋转指定角度
void Step2_Rotate_Steps(int steps);			//旋转指定步数
void Step2_Rotate_Turns(float turns);		//旋转指定圈数
void Step2_Stop(void);						//停止电机
int Step2_Get_Current_Steps(void);			//获取当前位置（步数）
float Step2_Get_Current_Degree(void);		//获取当前位置（角度）
void Step2_Reset_Position(void);			//重置位置计数器

#endif

