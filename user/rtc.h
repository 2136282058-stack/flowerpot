#ifndef __RTC_H
#define __RTC_H


/* 包含STM32标准外设库头文件 */
#include "stm32f10x.h"
 
/* 宏定义硬件配置（示例：LSE作为时钟源，备份寄存器DR0存储初始化标志） */
#define RTC_CLOCK_SOURCE     RCC_RTCCLKSource_LSE  // RTC时钟源：LSE或LSI
#define RTC_ASYNC_PREDIV     0x7F     // 异步预分频器（127，分频值=127+1）
#define RTC_SYNC_PREDIV      0x00FF   // 同步预分频器（255，分频值=255+1）
#define BACKUP_REG_FLAG      BKP_DR1 // 备份寄存器选择（DR0-DR41）
#define INIT_FLAG_VALUE      0xA5A5  // 初始化完成标志
 
/* 时间结构体类型定义 */
typedef struct {
    uint8_t hours;      // 小时（0-23）
    uint8_t minutes;    // 分钟（0-59）
    uint8_t seconds;    // 秒（0-59）
    uint8_t weekday;    // 周几（1-7，1=周一）
    uint8_t date;       // 日（1-31）
    uint8_t month;      // 月（1-12）
    uint16_t year;      // 年（0-4095）
} RTC_TimeTypeDef;

uint8_t RTC_Init(void);
void RTC_SetTime(RTC_TimeTypeDef *time);
void RTC_GetTime(RTC_TimeTypeDef *time);


#endif
