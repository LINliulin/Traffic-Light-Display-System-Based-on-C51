
#include <reg51.h>

sbit Red_nb=P2^6;		                //�ϱ���Ʊ�־
sbit Yellow_nb=P2^5;	                //�ϱ��ƵƱ�־
sbit Green_nb=P2^4;                     //�ϱ��̵Ʊ�־

sbit Red_dx=P2^3;		                //������Ʊ�־
sbit Yellow_dx=P2^2;	                //�����ƵƱ�־
sbit Green_dx=P2^1;		                //�����̵Ʊ�־

bit dx_nb=0;			                //�����ϱ�����λ
bit Yellow_status=0;	                //�Ƶ���˸��־λ

unsigned char  sec_dx;  		    	//��������ʱ��洢����
unsigned char  sec_nb;			    	//�ϱ�����ʱ��洢����
float  set_timedx=30;			    	//���������ͨ�е���ʱ��
float  set_timenb=30;				    //�ϱ������ͨ�е���ʱ��
unsigned char  buf[4];				    //����ʾ�ı���
unsigned char data countt0;             //��ʱ��0�жϼ�����
unsigned char data countt1;             //��ʱ��1�жϼ�����

unsigned char flag_option=0;            //����ѡ��λ:1 ����ģʽ��2 ҹ��ģʽ

sbit  key_EP=P3^2;		                //����ģʽ
unsigned char conut_ER;                 //����״̬

sbit  key_night=P3^3;		            //ҹ��ģʽ
unsigned char flag_Night;		        //ҹ��״̬
unsigned char conut_night;              //ҹ��ģʽ

unsigned char code led[11]={	        //������������
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



/*��ʱ����*/
void delay(int time)
{
    unsigned int j,k;
    for(j=0;j<time;j++)			
       for(k=0;k<124;k++);		
}

/*��ʾ����*/
void display(void)
{
    buf[0]=sec_nb/10; 		//��1λ ����ʮλ
    buf[1]=sec_nb%10; 		//��2λ ������λ
    buf[2]=sec_dx/10; 		//��3λ �ϱ�ʮλ
    buf[3]=sec_dx%10; 		//��4λ �ϱ���λ
    P1=~0xfe;           		//LED1
    P0=~led[buf[0]];		    //����ʮλ
    delay(1);
    P1=~0xfd;             	//LED2
    P0=~led[buf[1]];		    //������λ
    delay(1);				
    P1=~0Xfb;		  		//LED3
    P0=~led[buf[2]];		    //�ϱ�ʮλ
    delay(1);				
    P1=~0Xf7;				//LED4
    P0=~led[buf[3]];		    //�ϱ���λ
    delay(1);				
}

/*���ܺ���*/
void function(void)
{
    switch (flag_option)
    {
        case 1:
        {
            conut_ER=1-conut_ER;
            if(conut_ER==1)
            {
                TR0=0;						//�ؽ�ͨ�ƶ�ʱ��
                P2=0x00;					//��ȫ���źŵ�
                Red_dx=1;                   //ȫ���ú��
                Red_nb=1;
            }		
            if(conut_ER==0)
            {
                TR0=1;						//����ͨ�ƶ�ʱ��
                P2=0X00;					//��ȫ���źŵ�
                Green_dx=1;					//�����̵���
                Red_nb=1;					//�ϱ������
                sec_dx=set_timedx;          //�ص���ֵ
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
                TR0=0;						//�ؽ�ͨ�ƶ�ʱ��
                P2=0x00;                    //��ȫ���źŵ�
                flag_Night=1;               //��ҹ��ģʽ
                sec_dx=00;					//�ĸ������ʱ�䶼Ϊ00
                sec_nb=00;
            }
            if(conut_night==0)
            {
                TR0=1;						//����ͨ�ƶ�ʱ��
                P2=0x00;                    //��ȫ���źŵ�
                Green_dx=1;					//�����̵���
                Red_nb=1;					//�ϱ������
                flag_Night=0;               //��ҹ��ģʽ
                sec_dx=set_timedx;          //�ص���ֵ
                sec_nb=sec_dx+5;
            }
            flag_option=0;
            break;
        }
        default:
            break;
    }
}

/*����������ͨ�Ƽ�ʱ*/
void time0(void) interrupt 1 using 1  	//��ʱ�ж��ӳ���
{
    TH0=0x4C;							//�ظ���ֵ;11.0592m����50ms
    TL0=0X00;
    TR0=1;								//����ͨ�ƶ�ʱ��
    countt0++;							//����������1�����ڼ���ʱ�䣩
    if(countt0==20)	                  	//��ʱ���жϴ���=20ʱ����1��ʱ��
    {	
        countt0=0;						//���������
        sec_dx--;						//����ʱ���1
        sec_nb--;						//�ϱ�ʱ���1
        if(sec_nb==0&&sec_dx==5&&(Yellow_status==0))		//���ϱ�����ʱ��0ʱ������5�룬���ڻƵ���˸ʱ��
        {
            sec_nb=5;
            Yellow_status=1;
        }
        if(sec_dx==0&&sec_nb==5&&(Yellow_status==0)) 		//����������ʱ��0ʱ������5�룬���ڻƵ���˸ʱ��
        {
            sec_dx=5;
            Yellow_status=1;			//�����Ƶƿ�ʼ����ʱ
        }
        if((sec_nb<=5)&&(dx_nb==0)&&(Yellow_status==1))  		//�����Ƶ���
        {
            Green_dx=0;
            Yellow_dx=~Yellow_dx;
        }
        if((sec_dx<=5)&&(dx_nb==1)&&(Yellow_status==1))  		//�ϱ��Ƶ���
        {
            Green_nb=0;
            Yellow_nb=~Yellow_nb;
        }
        if(dx_nb==0&&sec_nb==0&&sec_dx==0)			            //����ͨ�У����Ƶ�ʱ�䡢ͨ�з��򵹼�ʱ��0ʱ
        {
            P2=0x00;					//���ö����ϱ�����ĺ��̵�
            Green_nb=1;					//�ϱ��̵���
            Red_dx=1;					//���������
            dx_nb=!dx_nb;				//����ͨ�з���
            Yellow_status=0;			//�رջƵ���˸
            sec_dx=set_timedx+5;			//�ظ������������ʼֵ
            sec_nb=set_timenb;		//�ظ��ϱ��������ʼֵ
        }
        if(dx_nb==1&&sec_dx==0&&sec_nb==0)			            //�ϱ�ͨ�У����Ƶ�ʱ�䡢ͨ�з��򵹼�ʱ��0ʱ
        {
            P2=0X00;					//���ö����ϱ��ĺ��̵�״̬
            Green_dx=1;					//�����̵���
            Red_nb=1;					//�ϱ������
            dx_nb=!dx_nb;				//����ͨ�з���
            Yellow_status=0;					//�رջƵ���˸
            sec_dx=set_timedx;		//�ظ������������ʼֵ
            sec_nb=set_timenb+5;			//�ظ��ϱ��������ʼֵ
        }
    }
}

/*��������ҹ��Ƶ���˸*/
void time1(void) interrupt 3 			//��ʱ�ж��ӳ���
{
    TH1=0x4C;							//�ظ���ֵ;11.0592m����50ms
    TL1=0X00;
    countt1++;							//����������1�����ڼ���ʱ�䣩
    if(countt1==10)					    //��ʱ���жϴ���=10ʱ����0.5�룩
	{	   
		if(flag_Night==1)
	    {
			Yellow_nb=~Yellow_nb;		//�ϱ��Ƶ�
			Yellow_dx=~Yellow_nb;		//�����Ƶ�   
		}	
	} 
    if(countt1==20)	                  	//��ʱ���жϴ���=20ʱ����1��ʱ��
    {
        countt1=0;						//���������
        if(flag_Night==1)
        {
            Yellow_nb=~Yellow_nb;		//�ϱ��Ƶ�
            Yellow_dx=~Yellow_nb;		//�����Ƶ�
        }
    }
}


//�ⲿ�ж�0
void int0(void) interrupt 0 using 1	    //����ģʽ
{
    while(!key_EP)display();
    flag_option=1;
}

//�ⲿ�ж�1
void int1(void) interrupt 2 using 1	    //ҹ��ģʽ
{
    while(!key_night)display();
    flag_option=2;
}

/*��ʼ������*/
void init()
{
    TMOD=0X11;	   						//��ʱ������
    TH1=0x4C;							//TH1�ظ���ֵ;11.0592m����50ms
    TL1=0X00;
    TH0=0x4C;							//TH0�ظ���ֵ;11.0592m����50ms
    TL0=0X00;
    TCON=0X50;							//������ʱ0����ʱ1
    IE=0x8F;							//�����ж�;��ʱ��1�жϿ���;���ⲿ�ж�1;��ʱ��0�жϿ���;���ⲿ�ж�
    P2=0Xc3;							//��ʼĬ��״̬�������̵ƣ��ϱ��Ƶ�
    sec_nb=35;							//�ϱ������ʼʱ��
    sec_dx=30;							//���������ʼʱ��
}

//������
void main()
{
    init();								//��ʼ����ʱ�����жϷ���
    while(1)					 		
    {
        if(flag_option!=0)function(); 	//���ù��ܺ���
        display(); 						//������ʾ����
    }
}