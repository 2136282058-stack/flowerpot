#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "font.h"


/**
  * @brief  将GBR格式颜色值转换为RGB格式
  * @param  c: GBR格式的颜色值
  * @retval RGB格式的颜色值
  * @note   从ILI93xx读出的数据为GBR格式，而写入时需要RGB格式
  */
u16 LCD_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;
  
  // 从GBR格式中提取各颜色分量（5-6-5格式）
  b = (c >> 0) & 0x1f;   // 提取蓝色分量（低5位）
  g = (c >> 5) & 0x3f;   // 提取绿色分量（中间6位）
  r = (c >> 11) & 0x1f;  // 提取红色分量（高5位）
  
  // 重新组合为RGB格式
  rgb = (b << 11) + (g << 5) + (r << 0);
  
  return rgb;
}

/**
  * @brief  使用Bresenham算法画圆
  * @param  X: 圆心X坐标
  * @param  Y: 圆心Y坐标
  * @param  R: 圆半径
  * @param  fc: 圆的颜色
  */
void Gui_Circle(u16 X, u16 Y, u16 R, u16 fc)
{
  unsigned short a, b;
  int c;
  
  a = 0;
  b = R;
  c = 3 - 2 * R;  // 决策参数初始化
  
  // Bresenham画圆算法主循环
  while (a < b)
  {
    // 绘制圆的8个对称点
    Gui_DrawPoint(X + a, Y + b, fc);  // 第7象限点
    Gui_DrawPoint(X - a, Y + b, fc);  // 第6象限点
    Gui_DrawPoint(X + a, Y - b, fc);  // 第2象限点
    Gui_DrawPoint(X - a, Y - b, fc);  // 第3象限点
    Gui_DrawPoint(X + b, Y + a, fc);  // 第8象限点
    Gui_DrawPoint(X - b, Y + a, fc);  // 第5象限点
    Gui_DrawPoint(X + b, Y - a, fc);  // 第1象限点
    Gui_DrawPoint(X - b, Y - a, fc);  // 第4象限点
    
    // 更新决策参数
    if (c < 0)
      c = c + 4 * a + 6;      // 选择像素E
    else
    {
      c = c + 4 * (a - b) + 10; // 选择像素SE
      b -= 1;                  // y减1
    }
    a += 1;                    // x增1
  }
  
  // 当a==b时，绘制剩余的对称点
  if (a == b)
  {
    Gui_DrawPoint(X + a, Y + b, fc);
    Gui_DrawPoint(X + a, Y + b, fc);
    Gui_DrawPoint(X + a, Y - b, fc);
    Gui_DrawPoint(X - a, Y - b, fc);
    Gui_DrawPoint(X + b, Y + a, fc);
    Gui_DrawPoint(X - b, Y + a, fc);
    Gui_DrawPoint(X + b, Y - a, fc);
    Gui_DrawPoint(X - b, Y - a, fc);
  }
}

/**
  * @brief  使用Bresenham算法画直线
  * @param  x0: 起始点X坐标
  * @param  y0: 起始点Y坐标
  * @param  x1: 结束点X坐标
  * @param  y1: 结束点Y坐标
  * @param  Color: 线条颜色
  */
void Gui_DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color)
{
  int dx, dy, dx2, dy2, x_inc, y_inc, error, index;
  
  Lcd_SetXY(x0, y0);  // 设置起始坐标
  
  dx = x1 - x0;  // 计算x方向增量
  dy = y1 - y0;  // 计算y方向增量
  
  // 确定x方向步进值（正负）
  if (dx >= 0)
  {
    x_inc = 1;   // 向右移动
  }
  else
  {
    x_inc = -1;  // 向左移动
    dx = -dx;    // 取绝对值
  }
  
  // 确定y方向步进值（正负）
  if (dy >= 0)
  {
    y_inc = 1;   // 向下移动
  }
  else
  {
    y_inc = -1;  // 向上移动
    dy = -dy;    // 取绝对值
  }
  
  dx2 = dx << 1;  // 2*dx
  dy2 = dy << 1;  // 2*dy
  
  // 根据斜率选择算法分支
  if (dx > dy)  // 斜率绝对值小于1的情况
  {
    error = dy2 - dx;  // 初始化误差项
    
    // 循环绘制每个像素
    for (index = 0; index <= dx; index++)
    {
      Gui_DrawPoint(x0, y0, Color);  // 绘制当前点
      
      // 检查误差项，决定是否需要改变y坐标
      if (error >= 0)
      {
        error -= dx2;   // 误差修正
        y0 += y_inc;    // y坐标步进
      }
      
      error += dy2;     // 更新误差项
      x0 += x_inc;      // x坐标步进
    }
  }
  else  // 斜率绝对值大于1的情况
  {
    error = dx2 - dy;  // 初始化误差项
    
    // 循环绘制每个像素
    for (index = 0; index <= dy; index++)
    {
      Gui_DrawPoint(x0, y0, Color);  // 绘制当前点
      
      // 检查误差项，决定是否需要改变x坐标
      if (error >= 0)
      {
        error -= dy2;   // 误差修正
        x0 += x_inc;    // x坐标步进
      }
      
      error += dx2;     // 更新误差项
      y0 += y_inc;      // y坐标步进
    }
  }
}

/**
  * @brief  绘制带边框的填充矩形
  * @param  x: 矩形左上角X坐标
  * @param  y: 矩形左上角Y坐标
  * @param  w: 矩形宽度
  * @param  h: 矩形高度
  * @param  bc: 矩形填充颜色
  * @note   绘制立体效果边框（左上亮，右下暗），内部填充指定颜色
  */
void Gui_box(u16 x, u16 y, u16 w, u16 h, u16 bc)
{
  // 绘制立体边框
  Gui_DrawLine(x, y, x + w, y, 0xEF7D);        // 上边框（亮色）
  Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);  // 右边框（暗色）
  Gui_DrawLine(x, y + h, x + w, y + h, 0x2965);  // 下边框（暗色）
  Gui_DrawLine(x, y, x, y + h, 0xEF7D);        // 左边框（亮色）
  
  // 填充矩形内部
  Gui_DrawLine(x + 1, y + 1, x + 1 + w - 2, y + 1 + h - 2, bc);
}

/**
  * @brief  绘制矩形框（三种模式）
  * @param  x: 矩形左上角X坐标
  * @param  y: 矩形左上角Y坐标
  * @param  w: 矩形宽度
  * @param  h: 矩形高度
  * @param  mode: 矩形模式
  *         0 - 凸起效果（左上亮，右下暗）
  *         1 - 凹陷效果（左上暗，右下亮）
  *         2 - 白色边框
  */
void Gui_box2(u16 x, u16 y, u16 w, u16 h, u8 mode)
{
  if (mode == 0)  // 凸起效果
  {
    Gui_DrawLine(x, y, x + w, y, 0xEF7D);      // 上边框（亮色）
    Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);  // 右边框（暗色）
    Gui_DrawLine(x, y + h, x + w, y + h, 0x2965);  // 下边框（暗色）
    Gui_DrawLine(x, y, x, y + h, 0xEF7D);      // 左边框（亮色）
  }
  
  if (mode == 1)  // 凹陷效果
  {
    Gui_DrawLine(x, y, x + w, y, 0x2965);      // 上边框（暗色）
    Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xEF7D);  // 右边框（亮色）
    Gui_DrawLine(x, y + h, x + w, y + h, 0xEF7D);  // 下边框（亮色）
    Gui_DrawLine(x, y, x, y + h, 0x2965);      // 左边框（暗色）
  }
  
  if (mode == 2)  // 白色边框
  {
    Gui_DrawLine(x, y, x + w, y, 0xffff);      // 上边框
    Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xffff);  // 右边框
    Gui_DrawLine(x, y + h, x + w, y + h, 0xffff);  // 下边框
    Gui_DrawLine(x, y, x, y + h, 0xffff);      // 左边框
  }
}

/**
  * @brief  显示凸起的按钮框（模拟按钮按下效果）
  * @param  x1: 左上角X坐标
  * @param  y1: 左上角Y坐标
  * @param  x2: 右下角X坐标
  * @param  y2: 右下角Y坐标
  */
void DisplayButtonDown(u16 x1, u16 y1, u16 x2, u16 y2)
{
  // 左上角边框（暗色，模拟阴影）
  Gui_DrawLine(x1, y1, x2, y1, GRAY2);        // 上边框
  Gui_DrawLine(x1 + 1, y1 + 1, x2, y1 + 1, GRAY1);  // 上边框内侧
  Gui_DrawLine(x1, y1, x1, y2, GRAY2);        // 左边框
  Gui_DrawLine(x1 + 1, y1 + 1, x1 + 1, y2, GRAY1);  // 左边框内侧
  
  // 右下角边框（亮色，模拟反光）
  Gui_DrawLine(x1, y2, x2, y2, WHITE);        // 下边框
  Gui_DrawLine(x2, y1, x2, y2, WHITE);        // 右边框
}

/**
  * @brief  显示凹下的按钮框（模拟按钮弹起效果）
  * @param  x1: 左上角X坐标
  * @param  y1: 左上角Y坐标
  * @param  x2: 右下角X坐标
  * @param  y2: 右下角Y坐标
  */
void DisplayButtonUp(u16 x1, u16 y1, u16 x2, u16 y2)
{
  // 左上角边框（亮色，模拟反光）
  Gui_DrawLine(x1, y1, x2, y1, WHITE);        // 上边框
  Gui_DrawLine(x1, y1, x1, y2, WHITE);        // 左边框
  
  // 右下角边框（暗色，模拟阴影）
  Gui_DrawLine(x1 + 1, y2 - 1, x2, y2 - 1, GRAY1);  // 下边框内侧
  Gui_DrawLine(x1, y2, x2, y2, GRAY2);        // 下边框
  Gui_DrawLine(x2 - 1, y1 + 1, x2 - 1, y2, GRAY1);  // 右边框内侧
  Gui_DrawLine(x2, y1, x2, y2, GRAY2);        // 右边框
}

/**
  * @brief  绘制16x16点阵GBK字体（支持ASCII和汉字）
  * @param  x: 起始X坐标
  * @param  y: 起始Y坐标
  * @param  fc: 字体颜色
  * @param  bc: 背景颜色
  * @param  s: 要显示的字符串指针
  * @note   支持换行符（ASCII 13）处理
  */
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
  unsigned char i, j;
  unsigned short k, x0;
  x0 = x;  // 保存起始X坐标用于换行处理
  
  while (*s)
  {
    if ((*s) < 128)  // ASCII字符（单字节）
    {
      k = *s;
      
      // 处理换行符
      if (k == 13)  // 回车符
      {
        x = x0;     // X坐标回到行首
        y += 16;    // Y坐标下移一行
      }
      else
      {
        // 计算字符在字库中的索引
        if (k > 32)
          k -= 32;  // 可显示字符从空格（32）开始
        else
          k = 0;    // 控制字符
        
        // 绘制8x16 ASCII字符
        for (i = 0; i < 16; i++)
          for (j = 0; j < 8; j++)
          {
            if (asc16[k * 16 + i] & (0x80 >> j))  // 判断像素点是否需要显示
              Gui_DrawPoint(x + j, y + i, fc);     // 绘制字体颜色
            else if (fc != bc)                     // 背景颜色与字体颜色不同时才绘制背景
              Gui_DrawPoint(x + j, y + i, bc);     // 绘制背景颜色
          }
        x += 8;  // 字符宽度为8像素
      }
      s++;
    }
    else  // 汉字（双字节GBK编码）
    {
      // 在汉字字库中查找对应的字模
      for (k = 0; k < hz16_num; k++)
      {
        if ((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1)))
        {
          // 绘制16x16汉字
          for (i = 0; i < 16; i++)
          {
            // 绘制左半部分（8像素）
            for (j = 0; j < 8; j++)
            {
              if (hz16[k].Msk[i * 2] & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, fc);
              else if (fc != bc)
                Gui_DrawPoint(x + j, y + i, bc);
            }
            
            // 绘制右半部分（8像素）
            for (j = 0; j < 8; j++)
            {
              if (hz16[k].Msk[i * 2 + 1] & (0x80 >> j))
                Gui_DrawPoint(x + j + 8, y + i, fc);
              else if (fc != bc)
                Gui_DrawPoint(x + j + 8, y + i, bc);
            }
          }
          break;  // 找到字模后跳出查找循环
        }
      }
      s += 2;    // 跳过两个字节（一个汉字）
      x += 16;   // 汉字宽度为16像素
    }
  }
}

/**
  * @brief  绘制24x24点阵GBK字体（支持ASCII和汉字）
  * @param  x: 起始X坐标
  * @param  y: 起始Y坐标
  * @param  fc: 字体颜色
  * @param  bc: 背景颜色
  * @param  s: 要显示的字符串指针
  * @note   ASCII字符使用16x16字库绘制，汉字使用24x24字库
  */
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
  unsigned char i, j;
  unsigned short k;
  
  while (*s)
  {
    if (*s < 0x80)  // ASCII字符
    {
      k = *s;
      
      // 计算字符索引
      if (k > 32)
        k -= 32;
      else
        k = 0;
      
      // 绘制8x16 ASCII字符
      for (i = 0; i < 16; i++)
        for (j = 0; j < 8; j++)
        {
          if (asc16[k * 16 + i] & (0x80 >> j))
            Gui_DrawPoint(x + j, y + i, fc);
          else if (fc != bc)
            Gui_DrawPoint(x + j, y + i, bc);
        }
      s++;
      x += 8;
    }
    else  // 汉字
    {
      // 在24x24汉字字库中查找
      for (k = 0; k < hz24_num; k++)
      {
        if ((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1)))
        {
          // 绘制24x24汉字（分三部分绘制，每部分8像素宽）
          for (i = 0; i < 24; i++)
          {
            // 第一部分（左8像素）
            for (j = 0; j < 8; j++)
            {
              if (hz24[k].Msk[i * 3] & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, fc);
              else if (fc != bc)
                Gui_DrawPoint(x + j, y + i, bc);
            }
            
            // 第二部分（中8像素）
            for (j = 0; j < 8; j++)
            {
              if (hz24[k].Msk[i * 3 + 1] & (0x80 >> j))
                Gui_DrawPoint(x + j + 8, y + i, fc);
              else if (fc != bc)
                Gui_DrawPoint(x + j + 8, y + i, bc);
            }
            
            // 第三部分（右8像素）
            for (j = 0; j < 8; j++)
            {
              if (hz24[k].Msk[i * 3 + 2] & (0x80 >> j))
                Gui_DrawPoint(x + j + 16, y + i, fc);
              else if (fc != bc)
                Gui_DrawPoint(x + j + 16, y + i, bc);
            }
          }
          break;
        }
      }
      s += 2;
      x += 24;
    }
  }
}

/**
  * @brief  绘制32x32点阵数字字体
  * @param  x: 起始X坐标
  * @param  y: 起始Y坐标
  * @param  fc: 字体颜色
  * @param  bc: 背景颜色
  * @param  num: 要显示的数字（0-9）
  */
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
  unsigned char i, j, k, c;
  
  // 绘制32x32数字字符
  for (i = 0; i < 32; i++)          // 32行
  {
    for (j = 0; j < 4; j++)         // 每行4字节（32位）
    {
      // 获取字模数据（每个数字占32x4字节）
      c = *(sz32 + num * 32 * 4 + i * 4 + j);
      
      // 绘制每个字节的8个像素
      for (k = 0; k < 8; k++)
      {
        if (c & (0x80 >> k))        // 判断像素点
          Gui_DrawPoint(x + j * 8 + k, y + i, fc);
        else if (fc != bc)
          Gui_DrawPoint(x + j * 8 + k, y + i, bc);
      }
    }
  }
}
