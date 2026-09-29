#include "BluetoothDisplay_Task.h"
#include "Sensor_Task.h"
#include "system.h"



void BluetoothDisplay_Task(void *pvParameters)
{
    SensorData_t sensor_data = {0, 0, 0, 0, 0, 0};
    TickType_t xLastWakeTime;
	 SensorData_t temp_data;
    const TickType_t xFrequency = pdMS_TO_TICKS(3000); // 3秒周期
    
    // 初始化上次唤醒时间
    xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        while(xQueueReceive(sensorQueue2, &temp_data, 0) == pdPASS)
        {
            sensor_data = temp_data;
        }
        
        // 温度湿度（使用实际的小数显示）
        int temp_int = sensor_data.temperature / 10;
        int temp_dec = sensor_data.temperature % 10;
        UsartPrintf(USART1, "\r\n温度: %d.%d℃  湿度: %d%%", 
                   temp_int, temp_dec, sensor_data.humidity);
        
        // 光照和CO2
        UsartPrintf(USART1, "\r\n光照: %dLux  CO2: %dppm", 
                   sensor_data.light, sensor_data.co2);
        
        // 雨水检测
        if(CHECK_R == 0)
            UsartPrintf(USART1, "\r\n雨水: 有雨");
        else
            UsartPrintf(USART1, "\r\n雨水: 无雨");
        
        // 动物检测
        if(sensor_data.animal_detected == 0)
            UsartPrintf(USART1, "  动物: 有");
        else
            UsartPrintf(USART1, "  动物: 无");
        
        // 遮阳状态
        if(flag_Step2 == 0)
            UsartPrintf(USART1, "\r\n遮阳: 关闭  ");
        else
            UsartPrintf(USART1, "\r\n遮阳: 打开  ");
        
        // 工作模式
        if(flag_mode == 0)
            UsartPrintf(USART1, "模式: 自动");
        else
            UsartPrintf(USART1, "模式: 手动");
        
        // 4. 等待下一个3秒周期
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
