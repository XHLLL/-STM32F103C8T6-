#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// 推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    PWM_Init();

}

void Motor_SetSpeed(int8_t Speed)
{
// 如果速度为正数
    if (Speed >= 0)
    {
        // 设置引脚4为高电平（正转）
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        // 设置引脚5为低电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        // 调用PWM模块设置通道3的占空比
        PWM_SetCompare3(Speed);
    }
    // 如果速度为负数
    else
    {
        // 设置引脚4为低电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        // 设置引脚5为高电平（反转）
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        // 调用PWM模块设置通道3的占空比（取绝对值）
        PWM_SetCompare3(-Speed);
    }
}
