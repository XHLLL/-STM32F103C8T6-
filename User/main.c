#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DS18B20.h"
#include <stdio.h>
int main(void)
{
    OLED_Init();
    OLED_ShowString(1,1,"Temp:");// 在OLED上显示"Temp:"
    DS18B20_Init();

    while(1)
    {
// 读取温度
        short temperature = DS18B20_Get_Temp();// 读取DS18B20温度传感器的温度值

        // 将温度转换为字符串 temperature / 10为整数部分，temperature % 10为小数部分
        char tempStr[10];
        sprintf(tempStr, "%+d.%d C", temperature / 10, temperature % 10);// 将温度值格式化为字符串
        //由于DS18B20_Get_Temp()函数乘0.625来获得十分之一摄氏度的精度，这里再除以10来以摄氏度为单位显示温度

        // 在 OLED 上显示温度
        OLED_ShowString(1, 6, tempStr);

        // 延时一段时间，以便观察温度变化
        Delay_ms(500);
    }
}
