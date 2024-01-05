#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "DS18B20.h"
#include <stdio.h>
#include <stdbool.h> // 包含bool类型的头文件
uint8_t Key_Num;
int8_t Speed = 0;
bool autoMode = false; // 添加一个标志来判断是否处于自动模式

// 根据温度设置电机速度
void SetSpeedByTemperature(short temperature)
{
    if (temperature <= 100)
    {
        Speed = -25;
    }
    else if (temperature > 100 && temperature <= 150)
    {
        Speed = -50;
    }
    else if (temperature > 150 && temperature <= 250)
    {
        Speed = -75;
    }
    else
    {
        Speed = -100;
    }
}

int main(void)
{
    OLED_Init();
    Motor_Init();
    Key_Init();
    DS18B20_Init();
    OLED_ShowString(1, 1, "Temp:");
    OLED_ShowString(2, 1, "Speed:");

    while (1)
    {
        // 读取温度
        short temperature = DS18B20_Get_Temp();

        // 将温度转换为字符串
        char tempStr[10];
        sprintf(tempStr, "%+d.%d C", temperature / 10, temperature % 10);

        // 在 OLED 上显示温度
        OLED_ShowString(1, 6, tempStr);

        // 处理按键事件
        Key_Num = Key_GetNum();
        if (Key_Num == 1)
        {
            Speed -= 25;
            if (Speed < -100)
            {
                Speed = 0;
            }
            OLED_ShowString(3, 1, "Fixed");// 在OLED上显示固定速度模式
            autoMode = false; // 切换到手动模式
        }
        else if (Key_Num == 2)
        {
            // 显示设置方式
            OLED_ShowString(3, 1, "Auto ");// 在OLED上显示自动模式
            autoMode = true; // 切换到自动模式
        }

        // 如果处于自动模式，根据温度设置电机速度
        if (autoMode)
        {
            SetSpeedByTemperature(temperature);
        }

        // 设置电机速度
        Motor_SetSpeed(Speed);

        // 在 OLED 上显示速度
        OLED_ShowSignedNum(2, 7, Speed, 3);

        // 延时一段时间，以便观察温度变化
        Delay_ms(500);
    }
}
