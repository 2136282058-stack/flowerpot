#include "key_function.h"
#include "system.h"


uint8_t key_num = 0;
void Key_function(void *pvParameters)
{
	while(1)
	{
		key_num = Chiclet_Keyboard_Scan(0);		//°´¼üÉ¨Ãè
	    if(key_num != 0)
		{ 
			switch(key_num)
			{
				case 1:
					xQueueSend(keyQueue,&key_num,portMAX_DELAY);
				break;
				case 2:
					xQueueSend(keyQueue,&key_num,portMAX_DELAY);
				break;
				case 3:
					xQueueSend(keyQueue,&key_num,portMAX_DELAY);
				break;
				default:
			    break;
			}
		}
		vTaskDelay(1);
	}	
}
