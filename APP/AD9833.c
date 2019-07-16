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
    /*********************************����Ƶ�ʵ�16����ֵ***********************************/
    static const uint32_t frequence_mid = 268435456/5;//�ʺ�25M����
    //���ʱ��Ƶ�ʲ�Ϊ25MHZ���޸ĸô���Ƶ��ֵ����λMHz ��AD9833���֧��25MHz
    frequence_DATA = Freq_kHz;
    frequence_DATA = frequence_DATA/1000; // MHz
    frequence_DATA = frequence_DATA*frequence_mid;
    frequence_hex = frequence_DATA;  //���frequence_hex��ֵ��32λ��һ���ܴ�����֣���Ҫ��ֳ�����14λ���д���
    frequence_LSB = frequence_hex; //frequence_hex��16λ�͸�frequence_LSB
    frequence_LSB = frequence_LSB&0x3fff;//ȥ�������λ��16λ����ȥ����λ������14λ
    frequence_MSB = (uint32_t)(5*Freq_kHz*0.65536);;//�˴�ֵFreq*(268435456/25)/1000000/2^14��freq*0.00065536����
    //frequence_MSB=frequence_hex>>14; //frequence_hex��16λ�͸�frequence_HSB
    frequence_MSB = frequence_MSB & 0x3fff;//ȥ�������λ��16λ����ȥ����λ������14λ

    Phs_data = Phase|0xC000;  //��λֵ
    AD9833_Write(0x0100); //��λAD9833,��RESETλΪ1
    AD9833_Write(0x2100); //ѡ������һ��д�룬B28λ��RESETλΪ1

    if(Freq_SFR==0)               //���������õ�����Ƶ�ʼĴ���0
    {
    frequence_LSB=frequence_LSB|0x4000;
    frequence_MSB=frequence_MSB|0x4000;
    //ʹ��Ƶ�ʼĴ���0�������
    AD9833_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���0�ĵ�14λ��������
    AD9833_Write(frequence_MSB); //H14 Ƶ�ʼĴ����ĸ�14λ��������
    AD9833_Write(Phs_data); //������λ
    //AD9833_Write(0x2000); /**����FSELECTλΪ0��оƬ���빤��״̬,Ƶ�ʼĴ���0�������**/
    }
    if(Freq_SFR==1)             //���������õ�����Ƶ�ʼĴ���1
    {
    frequence_LSB=frequence_LSB|0x8000;
    frequence_MSB=frequence_MSB|0x8000;
    //ʹ��Ƶ�ʼĴ���1�������
    AD9833_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���1�ĵ�14λ����
    AD9833_Write(frequence_MSB); //H14 Ƶ�ʼĴ���1Ϊ
    AD9833_Write(Phs_data); //������λ
    //AD9833_Write(0x2800); /**����FSELECTλΪ0������FSELECTλΪ1����ʹ��Ƶ�ʼĴ���1��ֵ��оƬ���빤��״̬,Ƶ�ʼĴ���1�������**/
    }
    if(WaveMode==TRI_WAVE) //������ǲ�����
    {
       AD9833_Write(0x2002);
    }
    else if(WaveMode==SQU_WAVE)  //�����������
    {
       AD9833_Write(0x2028);
    }
    else if(WaveMode==SIN_WAVE)  //������Ҳ���
    {
       AD9833_Write(0x2000);
    }
}
