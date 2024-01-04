#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// IO方向设置
#define DS18B20_IO_IN()  {GPIOA->CRL &= 0X0FFFFFFF; GPIOA->CRL |= (uint32_t)8 << 28;} 
//这一行通过位与操作(&)和0X0FFFFFFF清除了配置GPIO引脚模式的位。通过与0X0FFFFFFF进行按位与运算，将引脚对应的位设置为0。
/*设置了输入模式的引脚配置。通过按位或操作(|)设置了适当的位。
值(uint32_t)8 << 28表示输入模式的配置，其中值8对应于输入模式（浮空输入），
通过左移28位来设置GPIO控制寄存器（CRL）中的适当位。*/

#define DS18B20_IO_OUT() {GPIOA->CRL&=0X0FFFFFFF;GPIOA->CRL|=3<<28;}
//设置了输出模式的引脚配置。值3对应于输出模式，最大速度50 MHz，通过左移28位来设置GPIO控制寄存器（CRL）中的适当位。

// IO操作函数
#define DS18B20_DQ_OUT_LOW()  (GPIOA->BRR |= GPIO_Pin_0)  // 拉低 DQ
//GPIOA->BRR |= GPIO_Pin_0 意味着将 GPIOA 的第 0 位引脚的相应位清零，即拉低 DQ。
#define DS18B20_DQ_OUT_HIGH() (GPIOA->BSRR |= GPIO_Pin_0) // 拉高 DQ
//GPIOA->BSRR |= GPIO_Pin_0 意味着将 GPIOA 的第 0 位引脚的相应位设置为 1，即拉高 DQ。
#define DS18B20_DQ_IN_STATE   (GPIOA->IDR & GPIO_Pin_0)   // 读取 DQ 状态
//GPIOA->IDR & GPIO_Pin_0 将读取 GPIOA 的第 0 位引脚的当前状态，即 DQ 的电平状态。

//复位DS18B20
void DS18B20_Rst(void)       
{                 
    DS18B20_IO_OUT();    // 设置为输出模式
    DS18B20_DQ_OUT_LOW(); // 拉低 DQ
    Delay_us(750);        //拉低750us
    DS18B20_DQ_OUT_HIGH();     //DQ=1 
    Delay_us(15);         //15us
}

//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 DS18B20_Check(void)        
{   
    uint16_t retry=0;
    DS18B20_IO_IN();    //将数据引脚配置为输入     
    while (DS18B20_DQ_IN_STATE && retry < 200)//等待 DS18B20 数据引脚为低电平，最多重试 200 次
    {
        retry++;
        Delay_us(1);
    };     
    if(retry>=200) 
        return 1;//如果超过 200 次重试，表示 DS18B20 数据引脚一直为高电平，返回1
    else 
        retry=0;
    while (!DS18B20_DQ_IN_STATE && retry < 240)//等待 DS18B20 数据引脚为高电平，最多重试 240 次
    {
        retry++;
        Delay_us(1);
    };
    if(retry>=240) 
        return 1;// 如果超过 240 次重试，表示 DS18B20 数据引脚一直为低电平，返回1
    return 0;//// 如果两个循环都没有超过最大重试次数，则返回 0，表示 DS18B20 存在
}

//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit(void)      
{
    u8 data;
    DS18B20_IO_OUT();   // 设置为输出模式
    DS18B20_DQ_OUT_LOW(); // 拉低 DQ 
    Delay_us(2);        //延时 2 微秒，确保拉低命令的稳定性
    DS18B20_DQ_OUT_HIGH(); //恢复 DQ 为高电平，准备接收 DS18B20 的响应
    DS18B20_IO_IN();    //将数据引脚配置为输入模式
    Delay_us(12);       //延时 12 微秒，等待 DS18B20 的响应信号
// 读取 DQ 引脚的状态，如果为高电平，表示 DS18B20 返回 1，否则返回 0
    if(DS18B20_DQ_IN_STATE) 
        data=1;
    else 
        data=0; 
    Delay_us(50);//延时 50 微秒，等待总线恢复到稳定状态
    return data;// 返回读取到的位的数值
}

//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)     
{        
    u8 i,j,dat;
    dat=0;
    for (i=1;i<=8;i++) 
    {
        j=DS18B20_Read_Bit();//调用前面解释的 DS18B20_Read_Bit() 函数，逐位读取数据
        dat=(j<<7)|(dat>>1);//// 将每次读取的位数据合并到 dat 变量中
    }                            
    return dat;// 返回读取到的一个字节数据
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
    DS18B20_IO_OUT(); // 将 GPIO 配置为输出模式，准备写入数据
    for (j=1;j<=8;j++) 
    {
        testb=dat&0x01;// 取最低位的值，即当前要写入的数据的最低位

        dat=dat>>1;// 右移一位，准备下一位的写入
        if (testb) 
        {
            DS18B20_DQ_OUT_LOW();// 写入 1，先拉低 DQ
            Delay_us(2);                            
            DS18B20_DQ_OUT_HIGH();// 拉高 DQ，产生写入脉冲
            Delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT_LOW();// 写入 0，先拉低 DQ
            Delay_us(60);             
            DS18B20_DQ_OUT_HIGH();
            Delay_us(2);                          
        }
    }
}

//开始温度转换
void DS18B20_Start(void) 
{                                          
    DS18B20_Rst();// 复位 DS18B20
    DS18B20_Check();// 检测 DS18B20 是否存在
    DS18B20_Write_Byte(0xcc);// 发送跳过 ROM 命令，因为只有一个 DS18B20，可以直接跳过 ROM 操作
    DS18B20_Write_Byte(0x44);// 发送温度转换命令，开始测量温度
} 

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    
u8 DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能PORTA口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;// 配置 GPIOA.0 引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// 配置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DS18B20_Rst();// 调用 DS18B20_Rst() 函数，复位 DS18B20 温度传感器
    return DS18B20_Check();// 调用 DS18B20_Check() 函数，检测 DS18B20 是否正常响应
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    u8 temp;// 用于存储温度的符号，表示正负
    u8 TL,TH;// 用于存储温度的低八位和高八位
    short tem;// 存储最终的温度值
    DS18B20_Start ();// 启动 DS18B20 温度转换
    DS18B20_Rst();  // 复位 DS18B20
    DS18B20_Check();// 检测 DS18B20 是否正常响应
    DS18B20_Write_Byte(0xcc);// 跳过 ROM 操作
    DS18B20_Write_Byte(0xbe);// 发送读取温度的命令
    TL=DS18B20_Read_Byte();// 读取温度的低八位
    TH=DS18B20_Read_Byte(); // 读取温度的高八位(TH>7)
    if(TH>7)// 判断温度是否为负数
    {
        TH=~TH;
        TL=~TL; 
        temp=0;                    //温度为负  
    }
    else 
        temp=1;                //温度为正            
    tem=TH;                     //获得高八位
    tem<<=8;    
    tem+=TL;                    //获得低八位
    tem=(float)tem*0.625;  // 转换为实际温度值 温度值被转换成以浮点数表示，然后乘以0.625来获得十分之一摄氏度的精度
    if(temp) 
        return tem;// 返回正温度值
    else 
        return -tem;// 返回负温度值
}
