#include "system.h"

// 定义互斥锁
SemaphoreHandle_t oledMutex;
SemaphoreHandle_t settingMutex;

void ResourceManager_Init(void)
{
    // 创建互斥锁
    oledMutex = xSemaphoreCreateMutex();
    settingMutex = xSemaphoreCreateMutex();
    
    if(oledMutex == NULL || settingMutex == NULL) {
        // 错误处理
        while(1);
    }
}
