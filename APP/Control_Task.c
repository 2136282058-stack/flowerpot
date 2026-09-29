#include "control_task.h"
#include "system.h"
#include "Sensor_Task.h"

extern _Bool flag_mode ;	
/* 继电器状态枚举 */
typedef enum {
    RELAY_OFF = 0,
    RELAY_ON = 1
} RelayState_t;

u16 temp_max = 40;                    //温度最大值
u16 temp_min = 10;                    //温度最小值
u16 humi_value = 0;										//土壤湿度值
u16 humi_min = 30;										//土壤湿度小值
u16 light_max = 60;										//光照最大值
u16 light_min = 35;										//光照最小值
uint16_t co2_max = 500;						  	//co2最大值

/* 控制任务函数 */
void Control_Task(void *pvParameters)
{
    SensorData_t sensor_data = {0,0,0,0,0};
    
    while(1)
    {
        /* 等待接收传感器数据 */
        if (xQueueReceive(sensorQueue1, &sensor_data, portMAX_DELAY) == pdPASS)
        {
			
		if(flag_mode == 0)										//自动模式
		{
			if(sensor_data.temperature > temp_max*10)				//如果温度大于最大值，开风扇
			{
				RELAY_FS = 1;
				RELAY_JR = 0;
				
			}
			else if(sensor_data.temperature < temp_min*10)		//如果温度小于最小值，开加热
			{
				RELAY_FS = 0;
				RELAY_JR = 1;
			}
			else																//否则关闭
			{
				RELAY_FS = 0;
				RELAY_JR = 0;
			}

			if(sensor_data.humidity < humi_min)				//如果湿度小于最小值，开水泵
			{
				RELAY_SB = 1;
			}
			else
			{
				RELAY_SB = 0;
			}

			if(sensor_data.light < light_min)			//如果光照小于最小值，开补光
			{
				RELAY_BG = 1;
				//Write_Data(0X3D);
			}
			else 
			{
				RELAY_BG = 0;
			}

			if(sensor_data.light > light_max || CHECK_R == 0)	//如果光照大于最大值或正在下雨，开遮阳棚
			{
				if(flag_Step2 == 0 && remaining_steps2 == 0)
				{
					Step2_Rotate_Turns(8);//转360度
				}
			}
			else
			{
				if(flag_Step2 == 1 && remaining_steps2 == 0)
				{
					Step2_Rotate_Turns(-8);//转360度
				}
			}

			if(sensor_data.co2 > co2_max)										//二氧化碳超过最大值，声光报警，否则关闭
			{

					BEEP = ~BEEP;
			}
			else
			{
				BEEP = 0;
			}
      
      if(sensor_data.animal_detected == 0)											//有动物，开启驱赶，否则关闭
      {
        RELAY_QG = 1;
      }
      else
      {
        RELAY_QG = 0;
      }
		}

		}
	}
}
