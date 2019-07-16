#include "AD9833_H.h"
void delay_us(uint8_t x)
{
    uint8_t count = x;
    while(count--);
}

void AD9833_Write(uint16_t TxData)
{
    static unsigned char i;
    SCK_1;
    FSYNC_1;
    FSYNC_0;
    /* transfer from the MSB to LSB */
    for(i = 16; i >0 ; i--)
    {
        /* check the MSB bit */
        if(TxData & 0x8000)
        {
            /* MSB bit is 1 */
            DAT_1;
        }
        else
        {
            /* MSB bit is 0 */
            DAT_0;
        }
        delay_us(1);
        SCK_0;
        SCK_1;
        TxData <<= 1;
    }
    FSYNC_1;
#if 0
    PORTCbits.RC3=1;//SCK_1();P5
    PORTCbits.RC4=1;//FSYNC_1();P6
    PORTCbits.RC4=0;//FSYNC_0();
    for(i = 0; i < 16; i++)
    {
        if (TxData & 0x8000)
        {
            PORTCbits.RC5=1;//DAT_1();P7
        }
        else
        {
            PORTCbits.RC5=0;//DAT_0();
            //AD9833_Delay();
            delay_us(1);
            PORTCbits.RC3=0;//SCK_0();
            PORTCbits.RC3=1;//SCK_1();
            TxData=TxData<<1;
        }
    }
    PORTCbits.RC4=1;//FSYNC_1();
#endif
}
void intiAD9833(uint32_t Freq_kHz, uint32_t Freq_SFR, uint8_t WaveMode, uint16_t  Phase)
{
    uint32_t frequence_LSB, frequence_MSB, Phs_data;
    uint32_t frequence_DATA;
    uint32_t frequence_hex;
    /*********************************计算频率的16进制值***********************************/
    static const uint32_t frequence_mid = 268435456/5;//适合25M晶振
    //如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
    frequence_DATA = Freq_kHz;
    frequence_DATA = frequence_DATA/1000; // MHz
    frequence_DATA = frequence_DATA*frequence_mid;
    frequence_hex = frequence_DATA;  //这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
    frequence_LSB = frequence_hex; //frequence_hex低16位送给frequence_LSB
    frequence_LSB = frequence_LSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位
    frequence_MSB = (uint32_t)(5*Freq_kHz*0.65536);;//此处值Freq*(268435456/25)/1000000/2^14（freq*0.00065536）；
    //frequence_MSB=frequence_hex>>14; //frequence_hex高16位送给frequence_HSB
    frequence_MSB = frequence_MSB & 0x3fff;//去除最高两位，16位数换去掉高位后变成了14位

    Phs_data = Phase|0xC000;  //相位值
    AD9833_Write(0x0100); //复位AD9833,即RESET位为1
    AD9833_Write(0x2100); //选择数据一次写入，B28位和RESET位为1

    if(Freq_SFR==0)               //把数据设置到设置频率寄存器0
    {
    frequence_LSB=frequence_LSB|0x4000;
    frequence_MSB=frequence_MSB|0x4000;
    //使用频率寄存器0输出波形
    AD9833_Write(frequence_LSB); //L14，选择频率寄存器0的低14位数据输入
    AD9833_Write(frequence_MSB); //H14 频率寄存器的高14位数据输入
    AD9833_Write(Phs_data); //设置相位
    //AD9833_Write(0x2000); /**设置FSELECT位为0，芯片进入工作状态,频率寄存器0输出波形**/
    }
    if(Freq_SFR==1)             //把数据设置到设置频率寄存器1
    {
    frequence_LSB=frequence_LSB|0x8000;
    frequence_MSB=frequence_MSB|0x8000;
    //使用频率寄存器1输出波形
    AD9833_Write(frequence_LSB); //L14，选择频率寄存器1的低14位输入
    AD9833_Write(frequence_MSB); //H14 频率寄存器1为
    AD9833_Write(Phs_data); //设置相位
    //AD9833_Write(0x2800); /**设置FSELECT位为0，设置FSELECT位为1，即使用频率寄存器1的值，芯片进入工作状态,频率寄存器1输出波形**/
    }
    if(WaveMode==TRI_WAVE) //输出三角波波形
    {
       AD9833_Write(0x2002);
    }
    else if(WaveMode==SQU_WAVE)  //输出方波波形
    {
       AD9833_Write(0x2028);
    }
    else if(WaveMode==SIN_WAVE)  //输出正弦波形
    {
       AD9833_Write(0x2000);
    }
}
