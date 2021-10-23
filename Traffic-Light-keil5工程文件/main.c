
#include <reg51.h>

sbit Red_nb=P2^6;		                //南北红灯标志
sbit Yellow_nb=P2^5;	                //南北黄灯标志
sbit Green_nb=P2^4;                     //南北绿灯标志

sbit Red_dx=P2^3;		                //东西红灯标志
sbit Yellow_dx=P2^2;	                //东西黄灯标志
sbit Green_dx=P2^1;		                //东西绿灯标志

bit dx_nb=0;			                //东西南北控制位
bit Yellow_status=0;	                //黄灯闪烁标志位

unsigned char  sec_dx;  		    	//东西方向时间存储变量
unsigned char  sec_nb;			    	//南北方向时间存储变量
float  set_timedx=30;			    	//东西方向的通行单次时间
float  set_timenb=30;				    //南北方向的通行单次时间
unsigned char  buf[4];				    //秒显示的变量
unsigned char data countt0;             //定时器0中断计数器
unsigned char data countt1;             //定时器1中断计数器

unsigned char flag_option=0;            //功能选择位:1 紧急模式；2 夜间模式

sbit  key_EP=P3^2;		                //紧急模式
unsigned char conut_ER;                 //紧急状态

sbit  key_night=P3^3;		            //夜间模式
unsigned char flag_Night;		        //夜间状态
unsigned char conut_night;              //夜间模式

unsigned char code led[11]={	        //共阴极字型码
    0x3f,  //--0
    0x06,  //--1
    0x5b,  //--2
    0x4f,  //--3
    0x66,  //--4
    0x6d,  //--5
    0x7d,  //--6
    0x07,  //--7
    0x7f,  //--8
    0x6f,  //--9
    0x00   //--NULL
};



/*延时函数*/
void delay(int time)
{
    unsigned int j,k;
    for(j=0;j<time;j++)			
       for(k=0;k<124;k++);		
}

/*显示函数*/
void display(void)
{
    buf[0]=sec_nb/10; 		//第1位 东西十位
    buf[1]=sec_nb%10; 		//第2位 东西个位
    buf[2]=sec_dx/10; 		//第3位 南北十位
    buf[3]=sec_dx%10; 		//第4位 南北个位
    P1=~0xfe;           		//LED1
    P0=~led[buf[0]];		    //东西十位
    delay(1);
    P1=~0xfd;             	//LED2
    P0=~led[buf[1]];		    //东西个位
    delay(1);				
    P1=~0Xfb;		  		//LED3
    P0=~led[buf[2]];		    //南北十位
    delay(1);				
    P1=~0Xf7;				//LED4
    P0=~led[buf[3]];		    //南北个位
    delay(1);				
}

/*功能函数*/
void function(void)
{
    switch (flag_option)
    {
        case 1:
        {
            conut_ER=1-conut_ER;
            if(conut_ER==1)
            {
                TR0=0;						//关交通灯定时器
                P2=0x00;					//关全部信号灯
                Red_dx=1;                   //全部置红灯
                Red_nb=1;
            }		
            if(conut_ER==0)
            {
                TR0=1;						//开交通灯定时器
                P2=0X00;					//关全部信号灯
                Green_dx=1;					//东西绿灯亮
                Red_nb=1;					//南北红灯亮
                sec_dx=set_timedx;          //回到初值
                sec_nb=sec_dx+5;
            }	
            flag_option=0;	
            break;
        }
        case 2:
        {
            conut_night=1-conut_night;
            if(conut_night==1)
            {
                TR0=0;						//关交通灯定时器
                P2=0x00;                    //关全部信号灯
                flag_Night=1;               //打开夜间模式
                sec_dx=00;					//四个方向的时间都为00
                sec_nb=00;
            }
            if(conut_night==0)
            {
                TR0=1;						//开交通灯定时器
                P2=0x00;                    //关全部信号灯
                Green_dx=1;					//东西绿灯亮
                Red_nb=1;					//南北红灯亮
                flag_Night=0;               //打开夜间模式
                sec_dx=set_timedx;          //回到初值
                sec_nb=sec_dx+5;
            }
            flag_option=0;
            break;
        }
        default:
            break;
    }
}

/*处理整个交通灯计时*/
void time0(void) interrupt 1 using 1  	//定时中断子程序
{
    TH0=0x4C;							//重赋初值;11.0592m晶振50ms
    TL0=0X00;
    TR0=1;								//开交通灯定时器
    countt0++;							//计数器数加1（用于计算时间）
    if(countt0==20)	                  	//定时器中断次数=20时（即1秒时）
    {	
        countt0=0;						//清零计数器
        sec_dx--;						//东西时间减1
        sec_nb--;						//南北时间减1
        if(sec_nb==0&&sec_dx==5&&(Yellow_status==0))		//当南北倒计时到0时，重置5秒，用于黄灯闪烁时间
        {
            sec_nb=5;
            Yellow_status=1;
        }
        if(sec_dx==0&&sec_nb==5&&(Yellow_status==0)) 		//当东西倒计时到0时，重置5秒，用于黄灯闪烁时间
        {
            sec_dx=5;
            Yellow_status=1;			//启动黄灯开始倒计时
        }
        if((sec_nb<=5)&&(dx_nb==0)&&(Yellow_status==1))  		//东西黄灯闪
        {
            Green_dx=0;
            Yellow_dx=~Yellow_dx;
        }
        if((sec_dx<=5)&&(dx_nb==1)&&(Yellow_status==1))  		//南北黄灯闪
        {
            Green_nb=0;
            Yellow_nb=~Yellow_nb;
        }
        if(dx_nb==0&&sec_nb==0&&sec_dx==0)			            //东西通行：当黄灯时间、通行方向倒计时到0时
        {
            P2=0x00;					//重置东西南背方向的红绿灯
            Green_nb=1;					//南北绿灯亮
            Red_dx=1;					//东西红灯亮
            dx_nb=!dx_nb;				//交换通行方向
            Yellow_status=0;			//关闭黄灯闪烁
            sec_dx=set_timedx+5;			//重赋东西方向的起始值
            sec_nb=set_timenb;		//重赋南北方向的起始值
        }
        if(dx_nb==1&&sec_dx==0&&sec_nb==0)			            //南北通行：当黄灯时间、通行方向倒计时到0时
        {
            P2=0X00;					//重置东西南北的红绿灯状态
            Green_dx=1;					//东西绿灯亮
            Red_nb=1;					//南北红灯亮
            dx_nb=!dx_nb;				//交换通行方向
            Yellow_status=0;					//关闭黄灯闪烁
            sec_dx=set_timedx;		//重赋东西方向的起始值
            sec_nb=set_timenb+5;			//重赋南北方向的起始值
        }
    }
}

/*用来处理夜间黄灯闪烁*/
void time1(void) interrupt 3 			//定时中断子程序
{
    TH1=0x4C;							//重赋初值;11.0592m晶振50ms
    TL1=0X00;
    countt1++;							//计数器数加1（用于计算时间）
    if(countt1==10)					    //定时器中断次数=10时（即0.5秒）
	{	   
		if(flag_Night==1)
	    {
			Yellow_nb=~Yellow_nb;		//南北黄灯
			Yellow_dx=~Yellow_nb;		//东西黄灯   
		}	
	} 
    if(countt1==20)	                  	//定时器中断次数=20时（即1秒时）
    {
        countt1=0;						//清零计数器
        if(flag_Night==1)
        {
            Yellow_nb=~Yellow_nb;		//南北黄灯
            Yellow_dx=~Yellow_nb;		//东西黄灯
        }
    }
}


//外部中断0
void int0(void) interrupt 0 using 1	    //紧急模式
{
    while(!key_EP)display();
    flag_option=1;
}

//外部中断1
void int1(void) interrupt 2 using 1	    //夜间模式
{
    while(!key_night)display();
    flag_option=2;
}

/*初始化函数*/
void init()
{
    TMOD=0X11;	   						//定时器设置
    TH1=0x4C;							//TH1重赋初值;11.0592m晶振50ms
    TL1=0X00;
    TH0=0x4C;							//TH0重赋初值;11.0592m晶振50ms
    TL0=0X00;
    TCON=0X50;							//启动定时0、定时1
    IE=0x8F;							//开总中断;定时器1中断开启;开外部中断1;定时器0中断开启;开外部中断
    P2=0Xc3;							//开始默认状态，东西绿灯，南北黄灯
    sec_nb=35;							//南北方向初始时间
    sec_dx=30;							//东西方向初始时间
}

//主程序
void main()
{
    init();								//初始化定时器、中断服务
    while(1)					 		
    {
        if(flag_option!=0)function(); 	//调用功能函数
        display(); 						//调用显示函数
    }
}