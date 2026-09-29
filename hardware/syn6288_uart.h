#ifndef __SYN6288_UART_H
#define __SYN6288_UART_H
#include "stdio.h"	
#include "sys.h" 

#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收
	  	
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void USART2_SendString(char* str);
void USART2_SendChar(char ch);
void USART2_Init(u32 bound);
void USART2_SendRawData(uint8_t* data, uint32_t len);

#endif

