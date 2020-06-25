/***********************************************************************************************************/
//hc-sr04 ���������
//����11.0592
//���ߣ�ģ��TRIG�� P1.2TX  ECH0 ��P1.1RX
//����ܣ����������P0�����ݿ�,P2.5 P2.4 P2.3��ѡͨ�����
/***********************************************************************************************************/
//*********��������ѡ��
#define AT51

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

//***********�߶�ͼ
//uint MAP[1024] = {0};
//uint Map_Count = 0;

//***********��ʾ���ݻ���
uchar posit = 0;

//***********��ʱ����
void _delayMicroseconds(uchar time_delay) //��ʱ10us
{
	char i;
	for (i = 0; i < time_delay; i++)
	{
    	uchar a;
    	for(a=1;a>0;a--);
    		_nop_();  //if Keil,require use intrins.h
	}
}

//**************
//Dispose_Target(struct Radar_Target * DisTarget, int * PMAP, int loca);

//**************time_to_cm
void Conut(void)
{
	time = TH0 * 256 + TL0;
	TH0 = 0;
	TL0 = 0;

	S = (time * 1.7) / 100;		 //�������CM

//	MAP[Map_Count] = time;//�߶�ͼ����д��

	//if (array_sizeof(MAP) >= Radar_Target1.Frame_Length + Radar_Target1.Frame_Mobile) 
	//{
	//	for (int time_SY = Radar_Target1.Frame_Mobile; time_SY <= array_sizeof(MAP); time_SY++) 
	//		MAP[time_SY - Radar_Target1.Frame_Mobile] = MAP[time_SY];
	//	Dispose_Target(Radar_Target1, MAP, loca);
	//}

//	if (Map_Count > 1024)//�жϸ߶�ͼ�±��Ƿ�Խ��
//	{
//		Map_Count = 0;
//	}
	if ((S >= 700) || flag == 1) //����������Χ��ʾ��-��
	{
		flag = 0;
		Test = 0;
	}
	else
	{
		Test = 1;
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
	timer++;
	if(timer >= Scan_timeout)
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
	TH0 = 0;//��ʱ��
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
			;	 //��EchoΪ1�������ȴ�
		TR0 = 0; //�رռ���
		Conut(); //����
	}
}
#endif