/***********************************************************************************************************/
//hc-sr04 ���������
//����11.0592
//���ߣ�ģ��TRIG�� P1.2TX  ECH0 ��P1.1RX
//����ܣ����������P0�����ݿ�,P2.5 P2.4 P2.3��ѡͨ�����
/***********************************************************************************************************/
//*********��������ѡ��
#define AT51
#define Display

#ifdef AT51
//***********ͷ�ļ�
#include <REG51.H>
#include <intrins.h>

//**********������
//#define Trig_Port G1
//#define Trig_Pin P2
//#define G1 1
//#define P1 1
//
//#define Echo_Port G3
//#define Echo_Pin P2
//#define G1 1
//#define P2 2
//***********���峣���﷨
#define uchar unsigned char //����һ�·���ʹ��
#define uint unsigned int
#define ulong unsigned long
#define Scan_timeout 18//����ģ���������
//***********�˿�����
//_sbit Echo = _Pin_read(Echo_Port, Echo_Pin)
sbit Echo = P1^1;
//_sbit Trig = _Pin_read(Trig_Port, Trig_Pin)
sbit Trig = P1^2;
//51������
sbit Test = P1^3;
//***********�Զ���Ĵ���
uint time = 0;
uint timer = 0;
ulong S = 0;
bit flag = 0;

//***********��ʾ���ݻ���
uchar posit = 0;
#ifdef Display
uchar const discode[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xBF, 0xff /*-*/};
uchar const positon[3] = {0xdf, 0xef, 0xf7};
uchar disbuff[4] = {
	0,
	0,
	0,
	0,
};
#endif

//***********��ʱ����
void _delayMicroseconds(uchar time_delay) //��ʱ10us
{
	for (char i = 0; i < time_delay; i++)
	{
    	uchar a;
    	for(a=1;a>0;a--);
    		_nop_();  //if Keil,require use intrins.h
	}
}
#ifdef Display
//*************
void Display(void) //ɨ�������
{
	if (posit == 0)
	{
		P0 = (discode[disbuff[posit]]) & 0x7f;
	}
	else
	{
		P0 = discode[disbuff[posit]];
	}
	P2 = positon[posit];
	if (++posit >= 3)
		posit = 0;
}
#endif
//**************time_to_cm
void Conut(void)
{
	time = TH0 * 256 + TL0;
	TH0 = 0;
	TL0 = 0;

	S = (time * 1.7) / 100;		 //�������CM
	if ((S >= 700) || flag == 1) //����������Χ��ʾ��-��
	{
		flag = 0;
		disbuff[0] = 10; //��-��
		disbuff[1] = 10; //��-��
		disbuff[2] = 10; //��-��
	}
	else
	{
		disbuff[0] = S % 1000 / 100;
		disbuff[1] = S % 1000 % 100 / 10;
		disbuff[2] = S % 1000 % 10 % 10;
	}
}
/********************************************************/
void zd0() interrupt 1 //T0�ж��������������,������෶Χ
{
	flag = 1; //�ж������־
}
/********************************************************/
void zd3() interrupt 3 //T1�ж�����ɨ������ܺͼ�800MS����ģ��
{
	/*
	1��������=12*��1/11.0592��= 0.09us = 0.1us
	340m/1s = 3m/0.113s
	0.113s = 113000us
	3�� = 113000*10��������
	��ʱ���������65535
	1130000/65535 = 17.2 = 18 
	ѭ��17�κ�ʱ 
	*/
	TH1 = 0;
	TL1 = 0;
	Display();//�����
	timer++;
	if (timer >= Scan_timeout)
	{
		timer = 0;
		//_Pin2_wright_High(1)
		Trig = 1; //113MS  ����һ��ģ��
		_delayMicroseconds(10);
		//_Pin2_wright_Low(1)
		Trig = 0;
	}
}
/*********************************************************/

void main(void)
{
	TMOD = 0x11; //��T0Ϊ��ʽ1��GATE=1��
	TH0 = 0;//������
	TL0 = 0;
	TH1 = 0; //6.6MS��ʱ
	TL1 = 0;
	ET0 = 1; //����T0�ж�
	ET1 = 1; //����T1�ж�
	TR1 = 1; //������ʱ��
	EA = 1;	 //�������ж�
	while (1)
	{
		while(!Echo)
			;	 //��RXΪ��ʱ�ȴ�
		TR0 = 1; //��������
		while(Echo)
			;	 //��RXΪ1�������ȴ�
		TR0 = 0; //�رռ���
		Conut(); //����
	}
}
#endif