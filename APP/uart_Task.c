#include "uart_task.h"
#include "system.h"

_Bool flag_mode = 0; //模式标志位(1代表手动)(0代表自动)

void Bluetooth_Task(void *pvParameters)
{
    char BluetoothCmd;
    
    while(1)
    {
        if(xQueueReceive(BT, &BluetoothCmd, portMAX_DELAY) == pdPASS)
        {
			switch(BluetoothCmd)
			{
				case('A'):											//A：切换自动模式
					flag_mode = 0;
				break;

				case('B'):											//B：风扇
						flag_mode = 1;
						RELAY_FS = !RELAY_FS;
				break;
				
				case('C'):											//C：加热
						flag_mode = 1;
						RELAY_JR = !RELAY_JR;
				break;

				case('H'):											//D：水泵
						flag_mode = 1;
						RELAY_SB = !RELAY_SB;
				break;
				
				case('E'):											//E：补光
						flag_mode = 1;
						RELAY_BG = !RELAY_BG; 
				break;

				case('F'):											//F：遮阳
						flag_mode = 1;
						if(flag_Step2 == 0)
						{
							Step2_Rotate_Turns(8);//转360度
						}
						else
						{
							Step2_Rotate_Turns(-8);//转360度
						}
				break;
          
                case('G'):											//G：驱赶
						flag_mode = 1;
						RELAY_QG = !RELAY_QG;
				break;
				default:
				break;
			}	
			USART1_Clear();
        }
    }
}
