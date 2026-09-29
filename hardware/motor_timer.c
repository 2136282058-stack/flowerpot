#include "motor_timer.h"

/**********************************
变量定义
**********************************/
_Bool flag_step2_o = 0;										//2号步进电机打开标志位
_Bool flag_step2_c = 0;										//2号步进电机关闭标志位
_Bool flag_Step2 = 0;											//2号步进电机状态标志位
int target_steps2 = 0;								//目标步数
int current_steps2 = 0;							//当前步数
int remaining_steps2 = 0;						//剩余步数

/****
*******步进电机控制引脚初始化函数
*****/
static void Step_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(STEP2_GPIO_CLK,ENABLE);				//使能端口时钟

	GPIO_InitStructure.GPIO_Pin  = STEP2_GPIO_PIN_A|STEP2_GPIO_PIN_B|STEP2_GPIO_PIN_C|STEP2_GPIO_PIN_D;		//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//设置成推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO口速度为50MHz
 	GPIO_Init(STEP2_GPIO_PORT, &GPIO_InitStructure);				//根据设定参数初始化
	GPIO_ResetBits(STEP2_GPIO_PORT,STEP2_GPIO_PIN_A|STEP2_GPIO_PIN_B|STEP2_GPIO_PIN_C|STEP2_GPIO_PIN_D);	//输出低电平

}

/****
*******定时器初始化函数：
*****/
static void TIM2_Init(void)  // 函数名改为 TIM2_Init
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	STEP_TIM_APBxClock_FUN(STEP_TIM_CLK, ENABLE); 								//时钟使能
	
	//定时器初始化
	TIM_TimeBaseStructure.TIM_Period = 100; 											//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1; 									//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 			//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM向上计数模式
	TIM_TimeBaseInit(STEP_TIM, &TIM_TimeBaseStructure); 					//根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(STEP_TIM,TIM_IT_Update,ENABLE ); 								//使能指定的定时器中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = STEP_TIM_IRQ;  					//定时器中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  		//先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;  					//从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  															//初始化NVIC寄存器

	TIM_Cmd(STEP_TIM, ENABLE);  																	//使能定时器					 
}

/*********************************************************
 $ @description: 步进电机单步（4相8拍模式）
 $ @param {step_number:步进电机号，step：第几步 0-7}
 $ @return {*}
**********************************************************/
static void STEP_MOTOR_8A(u8 step_number, u8 step)
{
	if(step_number == 2)
	{
		// 4相8拍控制表
		// 顺序：A-AB-B-BC-C-CD-D-DA
		switch(step)
		{
			case 0:  // A
				STEP2_A = 1; STEP2_B = 0; STEP2_C = 0; STEP2_D = 0;
				break;
			case 1:  // AB
				STEP2_A = 1; STEP2_B = 1; STEP2_C = 0; STEP2_D = 0;
				break;
			case 2:  // B
				STEP2_A = 0; STEP2_B = 1; STEP2_C = 0; STEP2_D = 0;
				break;
			case 3:  // BC
				STEP2_A = 0; STEP2_B = 1; STEP2_C = 1; STEP2_D = 0;
				break;
			case 4:  // C
				STEP2_A = 0; STEP2_B = 0; STEP2_C = 1; STEP2_D = 0;
				break;
			case 5:  // CD
				STEP2_A = 0; STEP2_B = 0; STEP2_C = 1; STEP2_D = 1;
				break;
			case 6:  // D
				STEP2_A = 0; STEP2_B = 0; STEP2_C = 0; STEP2_D = 1;
				break;
			case 7:  // DA
				STEP2_A = 1; STEP2_B = 0; STEP2_C = 0; STEP2_D = 1;
				break;
			default:  // 停止
				STEP2_A = 0; STEP2_B = 0; STEP2_C = 0; STEP2_D = 0;
				break;
		}
	}
}

/****
*******定时器中断服务函数
*****/
void TIM2_IRQHandler(void)  // 修改中断服务函数名称
{
	static int current_step2 = 0;
	static int step2_delay_counter = 0;
	
	if(TIM_GetITStatus(STEP_TIM, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(STEP_TIM, TIM_IT_Update);
		
		// 步进电机控制
		if(remaining_steps2 != 0)
		{
			step2_delay_counter++;
			if(step2_delay_counter >= STEP_SPEED)
			{
				step2_delay_counter = 0;
				
				// 判断方向并计算下一步
				if(remaining_steps2 > 0)
				{
					// 正转
					current_step2 = (current_step2 + 1) % 8;
					remaining_steps2--;
					current_steps2++;
				}
				else
				{
					// 反转
					current_step2 = (current_step2 + 7) % 8;  // 8-1 实现反向
					remaining_steps2++;
					current_steps2--;
				}
				
				// 设置步进电机状态
				STEP_MOTOR_8A(2, current_step2);
				
				// 检查是否完成
				if(remaining_steps2 == 0)
				{
					flag_step2_o = 0;
					flag_step2_c = 0;
					flag_Step2 = (target_steps2 > 0) ? 1 : 0;
				}
			}
		}
	}
}

/*********************************************************
 * @description: 旋转指定角度
 * @param {float} degree 角度值，正数为正转，负数为反转
 * @return {void}
 *********************************************************/
void Step2_Rotate_Degree(float degree)
{
	// 28BYJ-48参数：
	// 减速比: 1:64 (实际约1:63.68395)
	// 步距角: 5.625° / 64 = 0.08789°
	// 在8拍模式下，每步角度: 5.625° / 8 = 0.703125°
	
	// 计算需要的步数 (每步0.703125度)
	float steps = degree / 0.703125f;
	
	Step2_Rotate_Steps((int)steps);
}

/*********************************************************
 * @description: 旋转指定步数
 * @param {int} steps 步数，正数为正转，负数为反转
 * @return {void}
 *********************************************************/
void Step2_Rotate_Steps(int steps)
{
	if(steps == 0) return;
	
	target_steps2 = steps;
	remaining_steps2 = steps;
	
	if(steps > 0)
	{
		flag_step2_o = 1;
		flag_step2_c = 0;
	}
	else
	{
		flag_step2_o = 0;
		flag_step2_c = 1;
	}
}

/*********************************************************
 * @description: 旋转指定圈数
 * @param {float} turns 圈数，正数为正转，负数为反转
 * @return {void}
 *********************************************************/
void Step2_Rotate_Turns(float turns)
{
	// 每圈512步 (64×8)
	int steps = (int)(turns * 512);
	Step2_Rotate_Steps(steps);
}

/*********************************************************
 * @description: 停止电机
 * @return {void}
 *********************************************************/
void Step2_Stop(void)
{
	remaining_steps2 = 0;
	flag_step2_o = 0;
	flag_step2_c = 0;
	STEP_MOTOR_8A(2, 8);  // 8表示停止
}

/*********************************************************
 * @description: 获取当前位置（步数）
 * @return {int} 当前步数
 *********************************************************/
int Step2_Get_Current_Steps(void)
{
	return current_steps2;
}

/*********************************************************
 * @description: 获取当前位置（角度）
 * @return {float} 当前角度
 *********************************************************/
float Step2_Get_Current_Degree(void)
{
	return current_steps2 * 0.703125f;
}

/*********************************************************
 * @description: 重置位置计数器
 * @return {void}
 *********************************************************/
void Step2_Reset_Position(void)
{
	current_steps2 = 0;
	target_steps2 = 0;
}

/****
*******步进电机初始化函数
*****/	
void Step_Motor_Init(void)
{
	TIM2_Init();  // 改为调用 TIM2_Init
	Step_GPIO_Init();
	
	// 初始位置为0
	current_steps2 = 0;
	target_steps2 = 0;
	remaining_steps2 = 0;
}

