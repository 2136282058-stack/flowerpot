#ifndef __CONTROL_TASK_H
#define __CONTROL_TASK_H

/* 包含FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* 任务函数声明 */
void Control_Task(void *pvParameters);


#endif /* __CONTROL_TASK_H */

