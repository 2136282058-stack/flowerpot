#include "usart.h"
#include "system.h"
//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/*
extern uint8_t usart1_buf[256];				//串口1接收数组
extern uint8_t usart2_buf[256];				//串口2接收数组
extern uint8_t usart3_buf[256];				//串口3接收数组

	if(USART1_WaitRecive() == 0)
	{
		if(strstr((char*)usart1_buf,"+CMT:") != NULL)
		{

		}

		USART1_Clear();
	}
*/

/**********************************
变量定义
**********************************/
uint8_t usart1_buf[256];											//串口1接收数组
uint16_t usart1_cnt = 0, usart1_cntPre = 0;
//uint8_t usart2_buf[256];											//串口2接收数组
//uint16_t usart2_cnt = 0, usart2_cntPre = 0;

/*
************************************************************
*	函数名称：	Usart1_Init
*
*	函数功能：	串口1初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：			TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);													//使能USART1时钟
	
	//USART1 PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//复用推挽输出
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART1 PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//浮空输入
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart1 NVIC 配置
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;																		//串口1中断
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQ通道使能
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 3;														//抢占优先级0
	nvicInitStruct.NVIC_IRQChannelSubPriority = 3;																	//子优先级3
	NVIC_Init(&nvicInitStruct);																											//根据指定的参数初始化VIC寄存器
	
	//USART1 初始化设置
	usartInitStruct.USART_BaudRate = baud;																					//串口波特率
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//接收和发送模式
	usartInitStruct.USART_Parity = USART_Parity_No;																	//无奇偶校验位
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8位数据位
	
	USART_Init(USART1, &usartInitStruct); 																					//初始化串口1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);																	//使能接收中断
	
	USART_Cmd(USART1, ENABLE);																											//使能串口
}

/*
************************************************************
*	函数名称：	Usart2_Init
*
*	函数功能：	串口2初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：			TX-PA2		RX-PA3
************************************************************
*/
//void Usart2_Init(unsigned int baud)
//{
//	GPIO_InitTypeDef gpioInitStruct;
//	USART_InitTypeDef usartInitStruct;
//	NVIC_InitTypeDef nvicInitStruct;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOA时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);													//使能USART2时钟
//	
//	//USART2 PA2	TXD
//	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//复用推挽输出
//	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
//	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &gpioInitStruct);
//	
//	//USART2 PA3	RXD
//	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//浮空输入
//	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
//	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &gpioInitStruct);
//	
//	//Usart2 NVIC 配置
//	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;																		//串口2中断
//	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQ通道使能
//	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//抢占优先级1
//	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;																	//子优先级3
//	NVIC_Init(&nvicInitStruct);																											//根据指定的参数初始化VIC寄存器
//	
//	//USART1 初始化设置
//	usartInitStruct.USART_BaudRate = baud;																					//串口波特率
//	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
//	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//接收和发送模式
//	usartInitStruct.USART_Parity = USART_Parity_No;																	//无奇偶校验位
//	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1位停止位
//	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8位数据位
//	
//	USART_Init(USART2, &usartInitStruct); 																					//初始化串口2
//	
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);																	//使能串口2接收中断
//	
//	USART_Cmd(USART2, ENABLE);																											//使能串口2
//}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

/*
************************************************************
*	函数名称：	USART1_Clear
//
*	函数功能：	清空缓存
//
*	入口参数：	无
//
*	返回参数：	无
//
*	说明：		
************************************************************
*/
void USART1_Clear(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
}

/*
************************************************************
*	函数名称：	USART1_WaitRecive
//
*	函数功能：	等待接收完成
//
*	入口参数：	无
//
*	返回参数：	0-接收完成		1-接收超时未完成
//
*	说明：		循环调用检测是否接收完成
************************************************************
*/
_Bool USART1_WaitRecive(void)
{
	if(usart1_cnt == 0) 								//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return 1;
		
	if(usart1_cnt == usart1_cntPre)			//如果上一次的值和这次相同，则说明接收完毕
	{
		usart1_cnt = 0;										//清0接收计数
			
		return 0;													//返回接收完成标志
	}
		
	usart1_cntPre = usart1_cnt;					//置为相同
	
	return 1;														//返回接收未完成标志
}

/*
************************************************************
*	函数名称：	USART2_Clear
//
*	函数功能：	清空缓存
//
*	入口参数：	无
//
*	返回参数：	无
//
*	说明：		
************************************************************
*/
//void USART2_Clear(void)
//{
//	memset(usart2_buf, 0, sizeof(usart2_buf));
//	usart2_cnt = 0;
//}

/*
************************************************************
*	函数名称：	USART2_WaitRecive
//
*	函数功能：	等待接收完成
//
*	入口参数：	无
//
*	返回参数：	0-接收完成		1-接收超时未完成
//
*	说明：		循环调用检测是否接收完成
************************************************************
*/
//_Bool USART2_WaitRecive(void)
//{

//	if(usart2_cnt == 0) 								//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
//		return 1;
//		
//	if(usart2_cnt == usart2_cntPre)			//如果上一次的值和这次相同，则说明接收完毕
//	{
//		usart2_cnt = 0;										//清0接收计数
//			
//		return 0;													//返回接收完成标志
//	}
//		
//	usart2_cntPre = usart2_cnt;					//置为相同
//	
//	return 1;														//返回接收未完成标志
//}


/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
//void USART1_IRQHandler(void)//首先定义中断服务函数，函数名必须是与STM32的启动文件里面的中断向量表一致
//{
//	char data;    //声明局部变量data，存储在栈上，避免多个中断嵌套时竞争问题
//	
//	BaseType_t Task_Flag = pdFALSE; //声明FreeRTOS专用变量，它的作用是记录是否需要切换到更高级任务
//	
//	                                //空行分隔，让代码结构更清晰，提高代码可读性
//	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)//这是中断服务函数的第一到检查，USART可能有多个中断源，我们只处理数据接收寄存器的非空判断
//	{
//		data = USART1->DR;//读取数据寄存器DR，这是原子操作，同时会自动清除DR寄存器的标志位，这是STM32的硬件特性
//		                            //空行分隔不同的逻辑步骤
//		//将数据发送到队列，让任务处理
//		xQueueSendFromISR(BT,&data,&Task_Flag计：使用队列传递数据。三个参数分别是队列句柄，数据地址，调度标志。注意必须使用中断版本
//		                            //继续分隔保持代码清晰
//		USART_ClearFlag(USART1,USART_FLAG_RXNE);  //显示清除标志位，这是防御性编程，虽然DR寄存器的标志位已经清除，但是双重保险更可靠
//		                            //在空一行，准备最后任务决策
//		 // 若唤醒更高优先级任务，则触发任务切换
//		portYIELD_FROM_ISR(Task_Flag);//"智能任务调度决策。如果Task_Flag为pdTRUE，立即切换到高优先级任务；否则保持当前任务"
//		
//	}  //关闭if语句，注意缩进对齐
//}//"结束中断服务函数。整个设计体现了中断最小化原则，处理时间在微秒级"


void USART1_IRQHandler(void)
{
	char data;//我们发送的数据
	BaseType_t Task_Flag = pdFALSE;//判断是否想要切换到高级任务
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)//判断是否有数据来
	{
		data = USART1->DR;
		xQueueSendFromISR(BT,&data,&Task_Flag);
		USART_ClearFlag(USART1,USART_FLAG_RXNE);//清除标志位
		portYIELD_FROM_ISR(Task_Flag);
	}
}


/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口2收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
//void USART2_IRQHandler(void)
//{
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 		//接收中断
//	{
//		if(usart2_cnt >= sizeof(usart2_buf))	usart2_cnt = 0; 	//防止串口被刷爆
//		usart2_buf[usart2_cnt++] = USART2->DR;
//		
//		USART_ClearFlag(USART2, USART_FLAG_RXNE);
//	}
//}


