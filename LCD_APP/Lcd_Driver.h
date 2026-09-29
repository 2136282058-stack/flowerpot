/******************************************************************************
//本程序适用与STM32F103C8
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PB3（SCL）
//              SDA   接PB5（SDA）
//              RES   接PB0
//              DC    接PA12
//              CS    接PA15 
//              注：背光控制引脚未使用，请直接接VCC或GND使屏幕常亮/常暗
*******************************************************************************/

/* ==============================
 * RGB565颜色宏定义 (16位颜色)
 * 格式：R(5位) G(6位) B(5位)
 * ============================== */

#define BLACK       0x0000  // 黑色: 0, 0, 0
#define WHITE       0xFFFF  // 白色: 255, 255, 255
#define RED         0xF800  // 红色: 255, 0, 0
#define GREEN       0x07E0  // 绿色: 0, 255, 0
#define BLUE        0x001F  // 蓝色: 0, 0, 255

// UI主题颜色 - 主界面
#define MAIN_BG_COLOR        0xDEFB  // 主界面背景 - 浅灰蓝色
#define MAIN_LABEL_COLOR     0x001F  // 参数标签颜色 - 深蓝色
#define MAIN_DATA_COLOR      0x0000  // 数据颜色 - 黑色
#define MAIN_HINT_COLOR      0x07E0  // 提示文字颜色 - 绿色
#define MAIN_ALARM_COLOR     0xF800  // 报警状态颜色 - 红色
#define MAIN_NORMAL_COLOR    0x07E0  // 正常状态颜色 - 绿色

// UI主题颜色 - 设置界面
#define SETTING_BG_COLOR     0xFFFF  // 设置界面背景 - 白色
#define SETTING_TEXT_COLOR   0x0000  // 普通文字颜色 - 黑色
#define SETTING_CURSOR_COLOR 0x001F  // 光标选中背景 - 蓝色
#define SETTING_EXIT_COLOR   0xF800  // 退出项颜色 - 红色
#define SETTING_HINT_COLOR   0x4A69  // 提示文字颜色 - 深灰色

// 通用灰色系
#define GRAY0                0xEF7D  // 最浅灰
#define GRAY1                0xC618  // 浅灰
#define GRAY2                0x8410  // 中灰
#define GRAY3                0x4208  // 深灰
// 常用的颜色转换宏
#define RGB_TO_565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#define GET_RED(color)      (((color) >> 8) & 0xF8)
#define GET_GREEN(color)    (((color) >> 3) & 0xFC)
#define GET_BLUE(color)     (((color) << 3) & 0xF8)

// 颜色混合宏（简单混合）
#define COLOR_MIX(c1, c2, ratio) \
    (((((c1) >> 11) * (256 - (ratio)) + ((c2) >> 11) * (ratio)) >> 8) << 11) | \
    ((((((c1) >> 5) & 0x3F) * (256 - (ratio)) + (((c2) >> 5) & 0x3F) * (ratio)) >> 8) << 5) | \
    (((((c1) & 0x1F) * (256 - (ratio)) + ((c2) & 0x1F) * (ratio)) >> 8))

// 颜色变亮/变暗
#define COLOR_BRIGHTEN(color, factor) \
    RGB_TO_565(MIN(255, GET_RED(color) + factor), \
               MIN(255, GET_GREEN(color) + factor), \
               MIN(255, GET_BLUE(color) + factor))

#define COLOR_DARKEN(color, factor) \
    RGB_TO_565(MAX(0, GET_RED(color) - factor), \
               MAX(0, GET_GREEN(color) - factor), \
               MAX(0, GET_BLUE(color) - factor))

// 定义端口（根据你的引脚分配）
#define LCD_CTRLA   	  	GPIOA		// PA12(DC), PA15(CS)
#define LCD_CTRLB   	  	GPIOB		// PB1(RST), PB3(SCL), PB5(SDA)

// 引脚定义 - 修改为你的引脚
#define LCD_SCL        	GPIO_Pin_3	// PB3--->>TFT --SCL/SCK
#define LCD_SDA        	GPIO_Pin_5	// PB5--->>TFT --SDA/DIN
#define LCD_CS        	GPIO_Pin_15  // PA15--->>TFT --CS/CE

// 背光引脚未使用 - 注释掉
//#define LCD_LED        	GPIO_Pin_1  // 背光控制引脚，未使用
#define LCD_RS         	GPIO_Pin_12	// PA12--->>TFT --RS/DC (数据/命令选择)
#define LCD_RST     	GPIO_Pin_0	// PB1--->>TFT --RST

// 液晶控制口置1操作语句宏定义
#define	LCD_SCL_SET  	LCD_CTRLB->BSRR=LCD_SCL    
#define	LCD_SDA_SET  	LCD_CTRLB->BSRR=LCD_SDA   
#define	LCD_CS_SET  	LCD_CTRLA->BSRR=LCD_CS  

// 背光控制未使用 - 注释掉
//#define	LCD_LED_SET  	LCD_CTRLB->BSRR=LCD_LED   
#define	LCD_RS_SET  	LCD_CTRLA->BSRR=LCD_RS 
#define	LCD_RST_SET  	LCD_CTRLB->BSRR=LCD_RST 

// 液晶控制口置0操作语句宏定义
#define	LCD_SCL_CLR  	LCD_CTRLB->BRR=LCD_SCL  
#define	LCD_SDA_CLR  	LCD_CTRLB->BRR=LCD_SDA 
#define	LCD_CS_CLR  	LCD_CTRLA->BRR=LCD_CS 

// 背光控制未使用 - 注释掉
//#define	LCD_LED_CLR  	LCD_CTRLB->BRR=LCD_LED 
#define	LCD_RST_CLR  	LCD_CTRLB->BRR=LCD_RST
#define	LCD_RS_CLR  	LCD_CTRLA->BRR=LCD_RS  

#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //数据输出
#define LCD_DATAIN     LCD_DATA->IDR;   //数据输入

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 



void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
unsigned int Lcd_ReadPoint(u16 x,u16 y);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void LCD_WriteData_16Bit(u16 Data);
void Lcd_FillRect(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color);

