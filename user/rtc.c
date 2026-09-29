#include "rtc.h"
#include "system.h"

// 备份寄存器兼容性处理
#if !defined(BKP_DR0)
    #ifndef BACKUP_REG_FLAG
    #define BACKUP_REG_FLAG BKP_DR1
    #endif
#endif

/**
  * @brief  RTC初始化函数（配置时钟源、预分频器、备份域）
  * @param  无
  * @retval 状态：0-成功，1-LSE启动失败
  */
uint8_t RTC_Init(void) {
    /* 1. 使能PWR和BKP时钟（访问备份域前必须开启） */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);  // 解除备份域写保护
    
    /* 2. 检查是否首次初始化（避免重复配置丢失时间） */
    if (BKP_ReadBackupRegister(BACKUP_REG_FLAG) != INIT_FLAG_VALUE) {
        /* 2.1 配置LSE时钟（若时钟源选择LSE） */
        RCC_LSEConfig(RCC_LSE_ON);  // 开启LSE晶振
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); // 等待LSE就绪
        
        /* 2.2 配置RTC时钟源 */
        RCC_RTCCLKConfig(RTC_CLOCK_SOURCE);  // 选择时钟源
        RCC_RTCCLKCmd(ENABLE);              // 使能RTC时钟
        
        /* 2.3 修复：正确设置预分频器 */
        // 对于32.768kHz LSE，要得到1Hz时钟：
        // 预分频值 = 32768 / 1 - 1 = 32767
        // 或者使用异步127分频 + 同步255分频
        uint32_t prescaler_value = 32767;  // 直接分频到1Hz
        // 或者：uint32_t prescaler_value = (127 << 16) | 255;  // 异步127，同步255
        RTC_SetPrescaler(prescaler_value); 
        
        /* 2.4 等待RTC寄存器同步 */
        RTC_WaitForSynchro();  // 确保寄存器可写入
        
        /* 2.5 设置初始化完成标志 */
        BKP_WriteBackupRegister(BACKUP_REG_FLAG, INIT_FLAG_VALUE);
        
        return 1; // 返回1表示首次初始化
    }
    
    /* 3. 使能RTC秒中断（可选） */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);  // 开启秒中断
    // NVIC_EnableIRQ(RTC_IRQn);         // 使能RTC全局中断（如果不需要中断，可以注释掉）
    
    return 0;  // 已经初始化过
}

 
/**
  * @brief  设置RTC时间
  * @param  time: 时间结构体指针
  * @retval 无
  */
void RTC_SetTime(RTC_TimeTypeDef *time) {
    uint32_t counter = 0;
    
    /* 将日历时间转换为秒计数器（简化版，忽略闰年） */
    counter += (time->year - 1970) * 31536000;  // 年转换为秒（非精确）
    counter += (time->month - 1) * 2592000;      // 月转换为秒
    counter += (time->date - 1) * 86400;        // 日转换为秒
    counter += time->hours * 3600;              // 小时转秒
    counter += time->minutes * 60;              // 分钟转秒
    counter += time->seconds;                   // 秒
    
    /* 写入计数器值 */
    RTC_SetCounter(counter);  // 设置RTC计数器
}
 
/**
  * @brief  读取RTC时间
  * @param  time: 时间结构体指针
  * @retval 无
  */
void RTC_GetTime(RTC_TimeTypeDef *time) {
    uint32_t counter = RTC_GetCounter();  // 获取当前计数器值
    
    /* 将秒计数器转换为日历时间（简化版，忽略闰年） */
    time->year = 1970 + (counter / 31536000);
    counter %= 31536000;
    time->month = 1 + (counter / 2592000);
    counter %= 2592000;
    time->date = 1 + (counter / 86400);
    counter %= 86400;
    time->hours = counter / 3600;
    counter %= 3600;
    time->minutes = counter / 60;
    time->seconds = counter % 60;
}
