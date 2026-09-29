#include "sys.h"
#include "syn6288_uart.h"
#include "syn6288_time.h"

//#if 1
//#pragma import(__use_no_semihosting)
////标准库需要的支持函数
//struct __FILE
//{
//	int handle;
//};

//FILE __stdout;
////定义_sys_exit()以避免使用半主机模式
//void _sys_exit(int x)
//{
//	x = x;
//}
////重定义fputc函数
//int fputc(int ch, FILE *f)
//{
//	while((USART1->SR & 0X40) == 0); //循环发送,直到发送完毕
//	USART1->DR = (u8) ch;
//	return ch;
//}
//#endif

// 发送字符串
void USART2_SendString(char* str) {
    while(*str) {
        USART2_SendChar(*str++);
    }
}

void USART2_SendChar(char ch) {
    USART_SendData(USART2, (uint16_t)ch);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
// 发送原始数据（不依赖字符串结束符）
void USART2_SendRawData(uint8_t* data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        USART_SendData(USART2, (uint16_t)data[i]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}


#if EN_USART2_RX   //如果使能了接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
u16 USART2_RX_STA = 0;     //接收状态标记

void USART2_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2时钟
	
	//USART2_TX   PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

	//USART2_RX	  PA3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口2

	TIM3_Int_Init(99, 7199);		//10ms中断
	USART2_RX_STA = 0;		//清零
	TIM_Cmd(TIM3, DISABLE);			//关闭定时器3
}

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART2);	//读取接收到的数据
		if((USART2_RX_STA & 0x8000) == 0) //接收未完成
		{
			if(USART2_RX_STA < USART2_REC_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM3, 0); //计数器清空
				if(USART2_RX_STA == 0) 	//使能定时器3的中断
				{
					TIM_Cmd(TIM3, ENABLE); //使能定时器3
				}
				USART2_RX_BUF[USART2_RX_STA++] = Res;	//记录接收到的值
			}
			else
			{
				USART2_RX_STA |= 1 << 15;				//强制标记接收完成
			}
		}
	}
}
#endif

