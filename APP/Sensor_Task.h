#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

/* 包含FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// 传感器数据结构体
typedef struct {
    short temperature;     // 温度值
    uint16_t humidity;     // 土壤湿度值
    uint16_t light;        // 光照值
    _Bool animal_detected; // 动物检测
    uint16_t co2;          // CO2值
	_Bool rain;
} SensorData_t;


// 声明外部变量
extern SensorData_t sensor_data;
/* 任务函数声明 */
void Sensor_Task(void *pvParameters);


#endif /* __CONTROL_TASK_H */




