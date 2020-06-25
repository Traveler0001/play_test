/***********************
hc-sr04 超声波测距
晶振：11.0592
接线：模块TRIG接 P1.2
单次反应时间：Max0.011ms
****************/

/**********头文件************/
#include <REG51.H>
#include <intrins.h>

//**********阿杜呢
//#define Trig_Port G1
//#define Trig_Pin P2
//#define G1 1
//#define P1 1

//***********定义常用语法
#define uchar unsigned char 
#define uint unsigned int
#define ulong unsigned long

//************定时器初始值
#define time_count_L TL0 = 0x18
#define time_count_H TH0 = 0x02
#define _sbit sbit
#define _bit bit
#define _TR TR0
#define _IT IT0
#define _EX EX0
#define _ET ET0
#define _EA EA


//***********端口声明
//_sbit Trig = _Pin_read(Trig_Port, Trig_Pin)
_sbit Trig = P1^2;
//_sbit Test = P1^3;    //51调试用
_sbit Int0 = P3^2;    //外部中断

//时间计算结果，如果储存距离，则可设置为uint型，储存计时值则必须ulong
//计时最大值130000
ulong time = 0;      

_bit flag = 0;        //定时器溢出标志位
uchar count = 0;     //溢出计数

//***********高度图
//uint MAP[1024] = {0};
//uint Map_Count = 0;

//***********1微妙延时函数
void _delayMicroseconds(uchar time_delay) //延时10us
{
	uchar i;
	for (i = 0; i < time_delay; i++)
	{
    	uchar a;
    	for(a=1;a>0;a--);
    		_nop_();  //if Keil,require use intrins.h
	}
}

//***********启动模块函数
void Scan_up(void)
{
	_TR = 0;
	time_count_H;
    time_count_L;
	//_Pin2_wright_High(1)
	Trig = 1; //1130uS  启动一次模块
	_delayMicroseconds(10);
	//_Pin2_wright_Low(1)
	Trig = 0;
	Int0 = 1;
	_TR = 1;
}

//************外部中断处理
void zd0() interrupt 0 //外部中断0
{
	if (time != 130000)//计时结果处理
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
	
	TR0 = 0;//关闭定时器
	//AS = (time * 1.7) / 100;		 //算出来是CM
	if((time >= 110000) || flag == 1) //超出测量范围
	{
		flag = 0;     //标志位清0
	//	Test = 1;
		Scan_up();
		count = 0;    //溢出计数置0
	}
	else
	{
	//	MAP[Map_Count] = time;//高度图数据写入

	//if (array_sizeof(MAP) >= Radar_Target1.Frame_Length + Radar_Target1.Frame_Mobile) 
	//{
	//	for (int time_SY = Radar_Target1.Frame_Mobile; time_SY <= array_sizeof(MAP); time_SY++) 
	//		MAP[time_SY - Radar_Target1.Frame_Mobile] = MAP[time_SY];
	//	Dispose_Target(Radar_Target1, MAP, loca);
	//}

//	if (Map_Count > 1024)//判断高度图下标是否越界
//	{
//		Map_Count = 0;
//	}
		count = 0;
	//	Test = 0;
		Scan_up();
	}
}

/********************************************************/
void ds1() interrupt 1 //定时器溢出
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
		flag = 1; //中断溢出标志
	}
}
/*********************************************************/

void main(void)
{
	_IT = 1;  //外部中断0中断类型选择，1为下降沿触发
	_EX = 1;  //EX0：外部中断bai0允许位，1表示允许外部中断0的中断申请
	_ET = 1; //开放定时器0中断控制位
	TMOD = 0x11; //设T0为方式1，GATE=1；

	Scan_up();

	_EA = 1;	 //开启总中断
	Int0 = 1;

	while (1)
	{
		/*******此if需放置在主循环中********/
		if (flag)
		{
			Int0 = 0;
		}
		;
	}
}
	/*
	1机器周期=12*（1/11.0592）= 0.09us = 0.1us
	34000cm/1s = 3cm/0.113ms
	0.113ms = 1130us
	3米 = 1130*10机器周期
	定时器最大周期65535
	11300/65535 = 0.x 
	*/