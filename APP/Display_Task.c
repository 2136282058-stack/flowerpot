#include "Display_Task.h"
#include "system.h"
#include "Sensor_Task.h"
#include "keyControl_Task.h"

// 主界面绘图标志
static uint8_t main_screen_drawn = 0;

// 绘制主界面框架
void draw_main_screen_frame(void)
{
    char buf[20];

    // 绘制各参数标签（固定部分）
    Gui_DrawFont_GBK16(5, 5, MAIN_LABEL_COLOR, MAIN_BG_COLOR, (u8*)"温度:");
    Gui_DrawFont_GBK16(5, 25, MAIN_LABEL_COLOR, MAIN_BG_COLOR, (u8*)"湿度:");
    Gui_DrawFont_GBK16(5, 45, MAIN_LABEL_COLOR, MAIN_BG_COLOR, (u8*)"光照:");
    Gui_DrawFont_GBK16(5, 65, MAIN_LABEL_COLOR, MAIN_BG_COLOR, (u8*)"CO2 :");
    Gui_DrawFont_GBK16(5, 85, MAIN_LABEL_COLOR, MAIN_BG_COLOR, (u8*)"动物:");
    Gui_DrawFont_GBK16(5, 105, MAIN_LABEL_COLOR, MAIN_BG_COLOR, (u8*)"雨水:");
    
    // 绘制提示信息
    Gui_DrawFont_GBK16(5, 125, MAIN_HINT_COLOR, MAIN_BG_COLOR, (u8*)"按键1进入设置");
    
    main_screen_drawn = 1;
}

// 优化版本：只更新变化的数据
void Display_Task(void *pvParameters)
{
    SensorData_t sensor_data = {0, 0, 0, 0, 0, 0};
    SensorData_t last_sensor_data = {2, 2, 2, 2, 2, 2};
    
    char buf[20];
    char time_buffer[30];
    char time_buffer1[30];
    
    while(1)
    {
		
        // 主界面模式
        if(setting_mode == 1)
        {
            // 如果需要重绘主界面，先清屏并绘制框架
            if(need_redraw_main == 1 || main_screen_drawn == 0)
            {
                Lcd_Clear(MAIN_BG_COLOR);
                draw_main_screen_frame();
                need_redraw_main = 0;
                // 重置上次数据，强制重新显示所有内容
                last_sensor_data.temperature = 2;
                last_sensor_data.humidity = 2;
                last_sensor_data.light = 2;
                last_sensor_data.co2 = 2;
                last_sensor_data.animal_detected = 2;
                last_sensor_data.rain = 2;
            }
            
            // 尝试接收传感器数据（非阻塞方式）
            if(xQueueReceive(sensorQueue, &sensor_data, 0) == pdPASS)
            {
                // 只在数据变化时更新显示
                if(sensor_data.temperature != last_sensor_data.temperature)
                {
                    int temp_int = sensor_data.temperature / 10;
                    sprintf(buf, "%d℃", temp_int);
                    Gui_DrawFont_GBK16(45, 5, MAIN_DATA_COLOR, MAIN_BG_COLOR, (u8*)buf);
                    last_sensor_data.temperature = sensor_data.temperature;
                }
                
                if(sensor_data.humidity != last_sensor_data.humidity)
                {
                    sprintf(buf, "%d%%", sensor_data.humidity);
                    Gui_DrawFont_GBK16(45, 25, MAIN_DATA_COLOR, MAIN_BG_COLOR, (u8*)buf);
                    last_sensor_data.humidity = sensor_data.humidity;
                }
                
                if(sensor_data.light != last_sensor_data.light)
                {
                    sprintf(buf, "%dLux", sensor_data.light);
                    Gui_DrawFont_GBK16(45, 45, MAIN_DATA_COLOR, MAIN_BG_COLOR, (u8*)buf);
                    last_sensor_data.light = sensor_data.light;
                }
                
                if(sensor_data.co2 != last_sensor_data.co2)
                {
                    sprintf(buf, "%dppm", sensor_data.co2);
                    Gui_DrawFont_GBK16(45, 65, MAIN_DATA_COLOR, MAIN_BG_COLOR, (u8*)buf);
                    last_sensor_data.co2 = sensor_data.co2;
                }
                
                if(sensor_data.animal_detected != last_sensor_data.animal_detected)
                {
                    if(sensor_data.animal_detected == 0)
                        Gui_DrawFont_GBK16(45, 85, MAIN_ALARM_COLOR, MAIN_BG_COLOR, (u8*)"有");
                    else
                        Gui_DrawFont_GBK16(45, 85, MAIN_NORMAL_COLOR, MAIN_BG_COLOR, (u8*)"无");
                    last_sensor_data.animal_detected = sensor_data.animal_detected;
                }
                
                if(sensor_data.rain != last_sensor_data.rain)
                {
                    if(sensor_data.rain == 1)
                        Gui_DrawFont_GBK16(45, 105, MAIN_ALARM_COLOR, MAIN_BG_COLOR, (u8*)"有");
                    else
                        Gui_DrawFont_GBK16(45, 105, MAIN_NORMAL_COLOR, MAIN_BG_COLOR, (u8*)"无");
                    last_sensor_data.rain = sensor_data.rain;
                }
            }
        }
        
        vTaskDelay(50);  // 50毫秒延时，降低CPU占用
    }
}
