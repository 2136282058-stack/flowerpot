#include "keyControl_Task.h"
#include "system.h"

u8 dataToSend[] = {0xAA, 0x55, 0xFF, 0x3D, 0xFB};//进入设置界面
u8 dataToSend1[] = {0xAA, 0x55, 0xFF, 0x3E, 0xFB};//保持成功

_Bool setting_mode = 1;      // 设置界面模式标志位(0设置)(1不设置)
_Bool need_redraw_main = 0;  // 需要重绘主界面标志
uint8_t cursor_pos = 0;      // 光标位置(0-5对应6个设置项)
uint8_t exit_count = 0;      // 退出计数(0-3)

// 长按检测相关变量
static TickType_t key1_press_start_time = 0;  // 按键1按下的开始时间
static uint8_t key1_long_press_detected = 0;  // 长按已检测标志
#define LONG_PRESS_TIMEOUT pdMS_TO_TICKS(1000) // 长按时间阈值1000ms

// 设置项数量（包括退出项）
#define SETTING_ITEMS 7  // 原来是6，现在加上退出项变成7

// 函数声明
void draw_setting_screen(void);
void update_cursor_position(uint8_t new_pos);
void handle_exit_count(void);
void check_and_exit(void);
void save_settings_and_exit(void);  // 新增函数

// 保存设置并退出
void save_settings_and_exit(void)
{
    setting_mode = 1;
    cursor_pos = 0;
    exit_count = 0;
    need_redraw_main = 1;
    // 显示保存提示
    Lcd_Clear(SETTING_BG_COLOR);
	USART2_SendRawData(dataToSend1,sizeof(dataToSend1));
    vTaskDelay(pdMS_TO_TICKS(500));  // 缩短延迟时间
}

// 检查并执行退出
void check_and_exit(void)
{
    if(exit_count >= 3)
    {
        // 达到3次，保存并退出设置模式
        save_settings_and_exit();
    }
}

void keyControl_Task(void *pvParameters)
{
    char buf[20];
    u8 key;
    
    while(1)
    {
        xQueueReceive(keyQueue, &key, portMAX_DELAY);
        
        switch(key)
        {
            case 1:
            {
				
                if(setting_mode == 1)
                {
					USART2_SendRawData(dataToSend,sizeof(dataToSend));
                    // 主界面模式：进入设置模式
                    setting_mode = 0;
                    cursor_pos = 0; // 光标初始位置
                    exit_count = 0; // 重置退出计数
                    draw_setting_screen();
                }
                else
                {
                    // 设置模式：移动光标
                    uint8_t new_pos = (cursor_pos + 1) % SETTING_ITEMS;
                    update_cursor_position(new_pos);
                }
                break;
            }
                
            case 2:
                // 在设置模式下
                if(setting_mode == 0)
                {
                    if(cursor_pos < 6)
                    {
                        // 前6项：增加参数值
                        switch(cursor_pos)
                        {
                            case 0: // 温度最大值
                                temp_max++;
                                break;
                            case 1: // 温度最小值
                                temp_min++;
                                break;
                            case 2: // 湿度最小值
                                humi_min++;
                                break;
                            case 3: // 光照最大值
                                light_max++;
                                break;
                            case 4: // 光照最小值
                                light_min++;
                                break;
                            case 5: // CO2最大值
                                co2_max++;
                                break;
                        }
                        // 更新显示当前设置项的值
                        draw_setting_screen();
                    }
                    else if(cursor_pos == 6)
                    {
                        // 第7项(退出项)：增加退出计数，上限为3
                        if(exit_count < 3)
                        {
                            exit_count++;
                            // 检查是否需要退出
                            check_and_exit();
                        }
                        // 只有在没有退出时才更新显示
                        if(setting_mode == 0)
                        {
                            draw_setting_screen();
                        }
                    }
                }
                break;
                
            case 3:
                // 在设置模式下
                if(setting_mode == 0)
                {
                    if(cursor_pos < 6)
                    {
                        // 前6项：减少参数值
                        switch(cursor_pos)
                        {
                            case 0: // 温度最大值
                                if(temp_max > 0) temp_max--;
                                break;
                            case 1: // 温度最小值
                                if(temp_min > 0) temp_min--;
                                break;
                            case 2: // 湿度最小值
                                if(humi_min > 0) humi_min--;
                                break;
                            case 3: // 光照最大值
                                if(light_max > 0) light_max--;
                                break;
                            case 4: // 光照最小值
                                if(light_min > 0) light_min--;
                                break;
                            case 5: // CO2最大值
                                if(co2_max > 0) co2_max--;
                                break;
                        }
                        // 更新显示当前设置项的值
                        draw_setting_screen();
                    }
                    else if(cursor_pos == 6)
                    {
                        // 第7项(退出项)：减少退出计数，下限为0
                        if(exit_count > 0)
                        {
                            exit_count--;
                        }
                        // 更新显示
                        draw_setting_screen();
                    }
                }
                break;
        }
        
        vTaskDelay(10);
    }
}

// 绘制设置界面
void draw_setting_screen(void)
{
    char buf[20];
    
    Lcd_Clear(SETTING_BG_COLOR);
    
    // 绘制前6个设置项
    for(int i = 0; i < 6; i++)
    {
        uint16_t text_color = SETTING_TEXT_COLOR;
        uint16_t bg_color = SETTING_BG_COLOR;
        
        if(i == cursor_pos)
        {
            // 选中项显示蓝色背景
            bg_color = SETTING_CURSOR_COLOR;
            text_color = WHITE;
        }
        
        switch(i)
        {
            case 0:
                if(i == cursor_pos)
                    sprintf(buf, ">温度最大值%d℃", temp_max);
                else
                    sprintf(buf, " 温度最大值%d℃", temp_max);
                break;
            case 1:
                if(i == cursor_pos)
                    sprintf(buf, ">温度最小值%d℃", temp_min);
                else
                    sprintf(buf, " 温度最小值%d℃", temp_min);
                break;
            case 2:
                if(i == cursor_pos)
                    sprintf(buf, ">湿度最小值%d%%", humi_min);
                else
                    sprintf(buf, " 湿度最小值%d%%", humi_min);
                break;
            case 3:
                if(i == cursor_pos)
                    sprintf(buf, ">光照最大值%dLux", light_max);
                else
                    sprintf(buf, " 光照最大值%dLux", light_max);
                break;
            case 4:
                if(i == cursor_pos)
                    sprintf(buf, ">光照最小值%dLux", light_min);
                else
                    sprintf(buf, " 光照最小值%dLux", light_min);
                break;
            case 5:
                if(i == cursor_pos)
                    sprintf(buf, ">CO2最大值%dppm", co2_max);
                else
                    sprintf(buf, " CO2最大值%dppm", co2_max);
                break;
        }
        
        Gui_DrawFont_GBK16(0, i * 20, text_color, bg_color, (u8*)buf);
    }
    
    // 绘制退出项（第7项）
    uint16_t exit_text_color = SETTING_EXIT_COLOR;
    uint16_t exit_bg_color = SETTING_BG_COLOR;
    
    if(cursor_pos == 6)
    {
        exit_bg_color = SETTING_CURSOR_COLOR;
        exit_text_color = WHITE;
    }
    
    // 格式化退出项显示
    if(cursor_pos == 6)
    {
        sprintf(buf, ">退出 %d/3", exit_count);
    }
    else
    {
        sprintf(buf, " 退出 %d/3", exit_count);
    }
    
    Gui_DrawFont_GBK16(25, 130, exit_text_color, exit_bg_color, (u8*)buf);
}

// 更新光标位置并重绘
void update_cursor_position(uint8_t new_pos)
{
    cursor_pos = new_pos;
    draw_setting_screen();
}
