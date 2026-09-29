#include "system.h"

TaskHandle_t startTaskHandle;
TaskHandle_t displayTaskHandle;

// 定义全局句柄
QueueHandle_t sensorQueue;
QueueHandle_t sensorQueue1;
QueueHandle_t sensorQueue2;
QueueHandle_t sensorQueue3;
QueueHandle_t keyQueue;
QueueHandle_t BT;

/* 任务函数声明 */
static void startTask(void *pvParameters);
void Key_function(void *pvParameters);
void Control_Task(void *pvParameters);
void Display_Task(void *pvParameters);
void Sensor_Task(void *pvParameters);
void Bluetooth_Task(void *pvParameters);
void keyControl_Task(void *pvParameters);
void BluetoothDisplay_Task(void *pvParameters);
void Voice_Task(void *pvParameters);

int main()
{
    systemInit();
    xTaskCreate(startTask, "START_TASK", 256, NULL, 1, &startTaskHandle);
    vTaskStartScheduler();  // 开启任务调度
    while(1);  
}

void startTask(void *pvParameters)
{
    taskENTER_CRITICAL();  // 进入临界区
    
    // 创建队列
    sensorQueue = xQueueCreate(6, sizeof(SensorData_t));
    sensorQueue1 = xQueueCreate(6, sizeof(SensorData_t));
    sensorQueue2 = xQueueCreate(6, sizeof(SensorData_t));
    sensorQueue3 = xQueueCreate(6, sizeof(SensorData_t));
    keyQueue = xQueueCreate(3, sizeof(u8));
    BT = xQueueCreate(1, sizeof(char));
    
    // 创建任务
    xTaskCreate(BluetoothDisplay_Task, "BluetoothDisplay_Task", 256, NULL, 2, NULL);
    xTaskCreate(Voice_Task, "Voice_Task", 256, NULL, 2, NULL);
    xTaskCreate(Key_function, "Key_function", 128, NULL, 3, NULL);
    xTaskCreate(Sensor_Task, "Sensor_Task", 256, NULL, 3, NULL);
    xTaskCreate(Display_Task, "Display_Task", 256, NULL, 2, &displayTaskHandle);
    xTaskCreate(keyControl_Task, "keyControl_Task", 128, NULL, 1, NULL);
    xTaskCreate(Control_Task, "Control_Task", 128, NULL, 4, NULL);
    xTaskCreate(Bluetooth_Task, "Bluetooth_Task", 256, NULL, 2, NULL);
    
    vTaskDelete(startTaskHandle);  // 删除开始任务
    taskEXIT_CRITICAL();           // 退出临界区
}
