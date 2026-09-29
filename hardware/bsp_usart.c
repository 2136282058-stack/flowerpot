#include "bsp_usart.h"

// USART2初始化
void USART2_init(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    // 使能USART2和GPIOA时钟（USART2在APB1总线上）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置TX引脚（PA2）为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    

    // 配置RX引脚（PA3）为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    // 配置USART2参数
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure); 
    
    // 配置中断：禁用发送中断，使能接收中断
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);  
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
    
    // 清除标志位，使能USART2
    USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE);
    
    // 配置NVIC（嵌套向量中断控制器）
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @Brief: USART2发送单个字节
 * @Parm: ch: 待发送的数据
 */
void USART2_Send_U8(uint8_t ch)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART2, ch);
}

/**
 * @Brief: USART2发送字符串
 * @Parm: BufferPtr: 待发送的字符串（以NULL结尾）
 */
void USART2_Send_String(uint8_t *BufferPtr)
{
    while (*BufferPtr)
    {
        USART2_Send_U8(*BufferPtr++);
    }
}

/**
 * @Brief: USART2发送数组
 * @Parm: buffer: 待发送的数据数组, length: 数据长度
 */
void USART2_Send_Array(uint8_t *buffer, uint16_t length)
{
    for(uint16_t i = 0; i < length; i++)
    {
        USART2_Send_U8(buffer[i]);
    }
}

// 串口2中断服务函数
void USART2_IRQHandler(void)
{
    uint8_t Rx_Temp = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        Rx_Temp = USART_ReceiveData(USART2);
        // 处理接收到的数据
        Processing_Data(Rx_Temp);
    }
}

// 如果需要重定向printf到USART2，可以添加以下函数
#ifdef USE_USART2_FOR_PRINTF
#include <stdio.h>

/**
 * @Brief: 重定向printf到USART2
 */
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口2 */
    USART_SendData(USART2, (uint8_t)ch);
    
    /* 等待发送完毕 */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;
    return (ch);
}

/**
 * @Brief: 重定向scanf从USART2读取
 */
int fgetc(FILE *f)
{
    /* 等待串口输入数据 */
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
        ;
    return (int)USART_ReceiveData(USART2);
}
#endif

// 以下为您的数据处理函数
uint8_t recv_buf[5] = {0}; // 接收缓冲区
uint8_t g_index = 0;

/**
 * @Brief: 处理接收到的数据
 * @Parm: RXdata: 接收到的字节
 */
void Processing_Data(uint8_t RXdata)
{
    recv_buf[g_index++] = RXdata;
    
    if (g_index >= 5)
    {
        g_index = 0;
        // 这里可以添加数据处理逻辑
        // 例如：printf("Read_ID: %d\n", recv_buf[3]);
    }
}

/**
 * @Brief: 发送命令数据
 * @Parm: dat: 要发送的数据字节
 */
void Write_Data(uint8_t dat)
{
    uint8_t cmd[5];
    
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = 0xFF;
    cmd[3] = dat;  // 使用传入的数据
    cmd[4] = 0xFB;

    // 发送命令数组
    USART2_Send_Array(cmd, 5);
}
