/***********************
hc-sr04 ���������
����11.0592
���ߣ�ģ��TRIG�� P1.2
���η�Ӧʱ�䣺Max0.011ms
****************/

/**********ͷ�ļ�************/
#include <REG51.H>
#include <intrins.h>

//**********������
//#define Trig_Port G1
//#define Trig_Pin P2
//#define G1 1
//#define P1 1

//***********���峣���﷨
#define uchar unsigned char 
#define uint unsigned int
#define ulong unsigned long

//************��ʱ����ʼֵ
#define time_count_L TL0 = 0x18
#define time_count_H TH0 = 0x02
#define _sbit sbit
#define _bit bit
#define _TR TR0
#define _IT IT0
#define _EX EX0
#define _ET ET0
#define _EA EA


//***********�˿�����
//_sbit Trig = _Pin_read(Trig_Port, Trig_Pin)
_sbit Trig = P1^2;
//_sbit Test = P1^3;    //51������
_sbit Int0 = P3^2;    //�ⲿ�ж�

//ʱ������������������룬�������Ϊuint�ͣ������ʱֵ�����ulong
//��ʱ���ֵ130000
ulong time = 0;      

_bit flag = 0;        //��ʱ�������־λ
uchar count = 0;     //�������

//***********�߶�ͼ
//uint MAP[1024] = {0};
//uint Map_Count = 0;

//***********1΢����ʱ����
void _delayMicroseconds(uchar time_delay) //��ʱ10us
{
	uchar i;
	for (i = 0; i < time_delay; i++)
	{
    	uchar a;
    	for(a=1;a>0;a--);
    		_nop_();  //if Keil,require use intrins.h
	}
}

//***********����ģ�麯��
void Scan_up(void)
{
	_TR = 0;
	time_count_H;
    time_count_L;
	//_Pin2_wright_High(1)
	Trig = 1; //1130uS  ����һ��ģ��
	_delayMicroseconds(10);
	//_Pin2_wright_Low(1)
	Trig = 0;
	Int0 = 1;
	_TR = 1;
}

//************�ⲿ�жϴ���
void zd0() interrupt 0 //�ⲿ�ж�0
{
	if (time != 130000)//��ʱ�������
	{
	//	Test = 1;
		if (count == 0)
		{
			time = (TH0*256 + TL0) - 33;
		}
		else
		{
			time += (65000 + ((TH0*256 + TL0) - 33));
		}
	}
	
	TR0 = 0;//�رն�ʱ��
	//AS = (time * 1.7) / 100;		 //�������CM
	if((time >= 110000) || flag == 1) //����������Χ
	{
		flag = 0;     //��־λ��0
	//	Test = 1;
		Scan_up();
		count = 0;    //���������0
	}
	else
	{
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
		count = 0;
	//	Test = 0;
		Scan_up();
	}
}

/********************************************************/
void ds1() interrupt 1 //��ʱ�����
{
	if (count == 0)
	{
		time = 65000;
		count++;
		Scan_up();
	}
	else if(count == 1)
	{
		time = 130000;
		_TR = 0;
		flag = 1; //�ж������־
	}
}
/*********************************************************/

void main(void)
{
	_IT = 1;  //�ⲿ�ж�0�ж�����ѡ��1Ϊ�½��ش���
	_EX = 1;  //EX0���ⲿ�ж�bai0����λ��1��ʾ�����ⲿ�ж�0���ж�����
	_ET = 1; //���Ŷ�ʱ��0�жϿ���λ
	TMOD = 0x11; //��T0Ϊ��ʽ1��GATE=1��

	Scan_up();

	_EA = 1;	 //�������ж�
	Int0 = 1;

	while (1)
	{
		/*******��if���������ѭ����********/
		if (flag)
		{
			Int0 = 0;
		}
		;
	}
}
	/*
	1��������=12*��1/11.0592��= 0.09us = 0.1us
	34000cm/1s = 3cm/0.113ms
	0.113ms = 1130us
	3�� = 1130*10��������
	��ʱ���������65535
	11300/65535 = 0.x 
	*/