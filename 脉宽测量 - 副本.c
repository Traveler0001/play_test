/***********************************************************************************************************/
//hc-sr04 超声波测距
//晶振：11.0592
//接线：模块TRIG接 P1.2TX  ECH0 接P1.1RX
//数码管：共阳数码管P0接数据口,P2.5 P2.4 P2.3接选通数码管
/***********************************************************************************************************/
//*********条件编译选择
#define AT51
#define Display

#ifdef AT51
//***********头文件
#include <REG51.H>
#include <intrins.h>

//**********阿杜呢
//#define Trig_Port G1
//#define Trig_Pin P2
//#define G1 1
//#define P1 1
//
//#define Echo_Port G3
//#define Echo_Pin P2
//#define G1 1
//#define P2 2
//***********定义常用语法
#define uchar unsigned char //定义一下方便使用
#define uint unsigned int
#define ulong unsigned long
#define Scan_timeout 18//调节模块启动间隔
//***********端口声明
//_sbit Echo = _Pin_read(Echo_Port, Echo_Pin)
sbit Echo = P1^1;
//_sbit Trig = _Pin_read(Trig_Port, Trig_Pin)
sbit Trig = P1^2;
//51调试用
sbit Test = P1^3;
//***********自定义寄存器
uint time = 0;
uint timer = 0;
ulong S = 0;
bit flag = 0;

//***********显示数据缓冲
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

//***********延时函数
void _delayMicroseconds(uchar time_delay) //延时10us
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
void Display(void) //扫描数码管
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

	S = (time * 1.7) / 100;		 //算出来是CM
	if ((S >= 700) || flag == 1) //超出测量范围显示“-”
	{
		flag = 0;
		disbuff[0] = 10; //“-”
		disbuff[1] = 10; //“-”
		disbuff[2] = 10; //“-”
	}
	else
	{
		disbuff[0] = S % 1000 / 100;
		disbuff[1] = S % 1000 % 100 / 10;
		disbuff[2] = S % 1000 % 10 % 10;
	}
}
/********************************************************/
void zd0() interrupt 1 //T0中断用来计数器溢出,超过测距范围
{
	flag = 1; //中断溢出标志
}
/********************************************************/
void zd3() interrupt 3 //T1中断用来扫描数码管和计800MS启动模块
{
	/*
	1机器周期=12*（1/11.0592）= 0.09us = 0.1us
	340m/1s = 3m/0.113s
	0.113s = 113000us
	3米 = 113000*10机器周期
	定时器最大周期65535
	1130000/65535 = 17.2 = 18 
	循环17次后超时 
	*/
	TH1 = 0;
	TL1 = 0;
	Display();//数码管
	timer++;
	if (timer >= Scan_timeout)
	{
		timer = 0;
		//_Pin2_wright_High(1)
		Trig = 1; //113MS  启动一次模块
		_delayMicroseconds(10);
		//_Pin2_wright_Low(1)
		Trig = 0;
	}
}
/*********************************************************/

void main(void)
{
	TMOD = 0x11; //设T0为方式1，GATE=1；
	TH0 = 0;//计数器
	TL0 = 0;
	TH1 = 0; //6.6MS定时
	TL1 = 0;
	ET0 = 1; //允许T0中断
	ET1 = 1; //允许T1中断
	TR1 = 1; //开启定时器
	EA = 1;	 //开启总中断
	while (1)
	{
		while(!Echo)
			;	 //当RX为零时等待
		TR0 = 1; //开启计数
		while(Echo)
			;	 //当RX为1计数并等待
		TR0 = 0; //关闭计数
		Conut(); //计算
	}
}
#endif