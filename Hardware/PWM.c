#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //TIM2是APB1总线的外设，开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;// 复用推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;// 使用引脚2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    TIM_InternalClockConfig(TIM2);  // 选择TIM2时钟为内部时钟
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式为向上计数
    TIM_TimeBaseInitStructure.TIM_Period = 100-1; //周期，ARR自动重装器的值 在100k频率下计100个数，得到1ms的时间
    TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;   //PSC预分频器的值 对72MHz进行720分频得到100k计数频率
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;    //重复计数器的值
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);  //初始化时基单元
    
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;       //CCR 占空比
    TIM_OC3Init(TIM2,&TIM_OCInitStructure);

    TIM_Cmd(TIM2,ENABLE);   //启动定时器
}

void PWM_SetCompare3(uint16_t Compare)
{
    TIM_SetCompare3(TIM2,Compare);
}
