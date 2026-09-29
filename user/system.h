/**
  * @file system.h
  * @brief 系统总头文件 - 集成所有模块接口和全局声明
  * @details 
  * 1. 集中管理所有外设驱动和任务头文件包含
  * 2. 声明全局队列、任务句柄和系统标志位
  * 3. 提供系统初始化函数声明
  * 
  * @note 此文件为整个项目的核心头文件，修改时需注意依赖关系
  */

/* 防止重复包含的头文件保护 */
#ifndef __SYSTEM_H
#define __SYSTEM_H

/***************************** 外设驱动层包含 *****************************/
#include "sys.h"           // 系统核心（时钟、中断等）
#include "delay.h"         // 延时函数
#include "gpio.h"          // GPIO控制
#include "usart.h"         // 串口通信
#include "adc.h"           // 模数转换
#include "rtc.h"           // 实时时钟
#include "stm32f10x_bkp.h" // 备份寄存器
#include "stm32f10x_pwr.h" // 电源控制
#include "stm32f10x_rcc.h" // 时钟控制

/***************************** 传感器驱动包含 *****************************/
#include "ds18b20.h"       // 温度传感器
#include "sgp30.h"         // CO2/空气质量传感器
#include "key.h"           // 按键驱动

/***************************** 输出设备驱动包含 *****************************/
#include "oled.h"          // OLED显示屏
#include "motor_timer.h"   // 电机定时器控制
#include "Lcd_Driver.h"    // LCD底层驱动
#include "GUI.h"           // 图形用户界面
#include "syn6288_uart.h"

/***************************** 操作系统包含 *****************************/
#include "FreeRTOS.h"      // FreeRTOS核心
#include "task.h"          // 任务管理
#include "queue.h"         // 队列管理
#include "portable.h"      // 端口移植层

/***************************** 标准库包含 *****************************/
#include "stdio.h"         // 标准输入输出
#include "string.h"        // 字符串处理
#include "stdlib.h"        // 标准库函数

/***************************** 应用任务包含 *****************************/
#include "Sensor_Task.h"   // 传感器采集任务
#include "Display_Task.h"  // 显示任务
#include "BluetoothDisplay_Task.h" // 蓝牙显示任务
#include "Voice_Task.h"
/***************************** 外部队列声明 *****************************/
// 说明：这些队列用于任务间通信，具体用途参考对应任务模块
extern QueueHandle_t keyQueue;      // 按键消息队列
extern QueueHandle_t sensorQueue;   // 传感器数据队列（主）
extern QueueHandle_t sensorQueue1;  // 传感器数据队列1（备用/分类）
extern QueueHandle_t sensorQueue2;  // 传感器数据队列2（备用/分类）
extern QueueHandle_t sensorQueue3;  // 传感器数据队列2（备用/分类）
extern QueueHandle_t BT;            // 蓝牙通信队列

/***************************** 外部任务句柄声明 *****************************/
extern TaskHandle_t displayTaskHandle; // 显示任务句柄

/***************************** 全局系统标志位声明 *****************************/
extern uint8_t flag_display;       // 显示刷新标志位
extern _Bool flag_mode;            // 系统模式标志位（如手动/自动）
extern _Bool setting_mode;         // 设置模式标志位

/***************************** 传感器阈值声明 *****************************/
// 说明：这些阈值用于环境参数监控和报警判断
extern u16 temp_max;      // 温度上限阈值
extern u16 temp_min;      // 温度下限阈值
extern u16 humi_min;      // 湿度下限阈值
extern u16 light_max;     // 光照上限阈值
extern u16 light_min;     // 光照下限阈值
extern uint16_t co2_max;  // CO2浓度上限阈值

/***************************** 系统控制标志位声明 *****************************/
extern _Bool flag_step2_o;    // 步骤2开启标志（具体功能需参考业务逻辑）
extern _Bool flag_step2_c;    // 步骤2关闭标志
extern _Bool flag_Step2;      // 步骤2总控标志
extern _Bool animal_value;    // 动物检测标志（可能用于智能农业场景）



#define SHADE_OPEN_ANGLE    180.0f   // 打开角度
#define SHADE_CLOSE_ANGLE   -180.0f  // 关闭角度

/***************************** 函数声明 *****************************/
/**
  * @brief  系统初始化函数
  * @details 初始化所有硬件外设、操作系统组件和全局变量
  * @note   必须在main函数开始时调用，且只调用一次
  */
void systemInit(void);

#endif /* __SYSTEM_H 结束头文件保护 */
