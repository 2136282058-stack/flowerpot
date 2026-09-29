#include "Sensor_Task.h"
#include "system.h"

/* 控制任务函数 */
void Sensor_Task(void *pvParameters)
{
	SensorData_t sensor_data = {0,0,0,0,0,0};

	while(1)
	{
		sensor_data.temperature = DS18B20_Get_Temp();										//获取温度值
		sensor_data.humidity = 99-30*(Get_Adc_Average(0,3)*3.3/4096.0);	//获取土壤湿度
		sensor_data.light = 99-30*(Get_Adc_Average(1,3)*3.3/4096.0);	//获取光照值
		sensor_data.animal_detected = CHECK_H;
		sensor_data.rain = CHECK_R;
		Sgp30_Get_Co2_Value(&sensor_data.co2);
		xQueueSend(sensorQueue, &sensor_data, 0);
		xQueueSend(sensorQueue1, &sensor_data, 0);
		xQueueSend(sensorQueue2, &sensor_data, 0);
		vTaskDelay(1000);  // 添加1秒延时，让出CPU
	}
}

