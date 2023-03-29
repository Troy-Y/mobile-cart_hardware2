//#include <stdio.h>
//#include "platform.h"
#include "xil_printf.h"

//设置 MIO引脚地址
#define MIO_PIN_07		(*(volatile unsigned int *)0xF800071C)
#define MIO_PIN_50		(*(volatile unsigned int *)0xF80007C8)
#define MIO_PIN_51		(*(volatile unsigned int *)0xF80007CC)

//设置 GPIO端口方向寄存器地址  用于设定GPIO端口 1 2 3 4 方向  //控制GPIO 的输入或者输出  输入常开  DIRM[X] = 0  输出无效  DIRM[X] = 1 输出  状态
#define DIRM_0			(*(volatile unsigned int *)0xE000A204)
#define DIRM_1			(*(volatile unsigned int *)0xE000A244)
#define DIRM_2			(*(volatile unsigned int *)0xE000A284)
#define DIRM_3			(*(volatile unsigned int *)0xE000A2C4)
//设置 GPIO端口输出使能寄存器地址   用于设定GPIO端口 1 2 3 4 输出  //DIRM[X] = 1 时   控制引脚的输出使能   是否输出
#define OEN_0			(*(volatile unsigned int *)0xE000A208)
#define OEN_1			(*(volatile unsigned int *)0xE000A248)
#define OEN_2			(*(volatile unsigned int *)0xE000A288)
#define OEN_3			(*(volatile unsigned int *)0xE000A2C8)
//设置 GPIO端口输出寄存器地址      //data寄存器的值  是要输出到GPIO引脚上的值
#define DATA_0			(*(volatile unsigned int *)0xE000A040)  //led9
#define DATA_1			(*(volatile unsigned int *)0xE000A044)
#define DATA_2			(*(volatile unsigned int *)0xE000A048)  //led 6 7
#define DATA_3			(*(volatile unsigned int *)0xE000A04C)
//设置 GPIO端口输入寄存器地址    //读取GPIO引脚值，无论引脚配置是输入还是输出
#define DATA_0_RO		(*(volatile unsigned int *)0xE000A060)
#define DATA_1_RO		(*(volatile unsigned int *)0xE000A064)  //BTN8.BTN9
#define DATA_2_RO		(*(volatile unsigned int *)0xE000A068)  //BTNU  BTND  BTNL  BTNR  BTNC
#define DATA_3_RO		(*(volatile unsigned int *)0xE000A06C)

//设置 UART1引脚地址的宏定义
#define rMIO_PIN_48		(*(volatile unsigned long*)0xF80007C0)
#define rMIO_PIN_49 	(*(volatile unsigned long*)0xF80007C4)
#define rUART_CLK_CTRL 	(*(volatile unsigned long*)0xF8000154)
#define rControl_reg0 	(*(volatile unsigned long*)0xE0001000)
#define rMode_reg0 		(*(volatile unsigned long*)0xE0001004)
//设置 UART1端口波特率等参数地址寄存器的宏定义
#define rBaud_rate_gen_reg0 (*(volatile unsigned long*)0xE0001018)
#define rBaud_rate_divider_reg0 (*(volatile unsigned long*)0xE0001034)
#define rTx_Rx_FIFO0 (*(volatile unsigned long*)0xE0001030)
#define rChannel_sts_reg0 (*(volatile unsigned long*)0xE000102C)

void Car_run(int Car_ID,int Car_dir);

void send_Char_9(unsigned char modbus[]);				//9字节串口发送函数
void send_Char(unsigned char data);						//串口发送函数，一次一个字节
void RS232_Init();										//串口初始化函数

void delay(int i,int n,int m);							//延时函数

int main()
{
	u32 flag,flag1;		//变量flag用于记录SW0~SW7按键按下信息；变量flag1用于记录BTN8、BTN9按键按下信息
	//UNSIGN LONG INT
	//注：下面MIO引脚和EMIO引脚的序号是统一编号的，MIO序号为0~31及32~53，EMIO序号为54~85及86~117
	//配置及初始化MIO07引脚的相关寄存器，MIO07作为LED灯控制的输出引脚
	MIO_PIN_07 = 0x00003600;    //设置07 为GPIO  led9
	DIRM_0 = DIRM_0|0x00000080;  // 设置MIO-07 为输出   第七位是1
	OEN_0 = OEN_0|0x00000080;  //确认MIO07 输出
	//配置及初始化MIO50、MIO51引脚的相关寄存器，MIO50、MIO51作为按键输入引脚
	MIO_PIN_50 = 0x00003600;    //设置50 为GPIO   BTN8
	MIO_PIN_51 = 0x00003600;    //设置51 为GPIO   BTN9
	DIRM_1 = DIRM_1 & 0xFFF3FFFF;   // 18 19   位 置零
	//初始化EMIO54~EMIO58的引脚，它们对应BTNU、BTND、BTNL、BTNR、BTNC按键，输入
	DIRM_2 = DIRM_2 & 0xFFFFFFE0;   //0 1 2 3 4   置零
	//初始化EMIO59~EMIO66的引脚，它们对应SW7~SW0拨动开关，输入
	DIRM_2 = DIRM_2 & 0xFFFFE01F;   //5 6 7 8  置零
	//初始化EMIO67~EMIO74的引脚，它们对应LED7~LED0，输出
	DIRM_2 = DIRM_2|0x001FE000;  //13 14 15 16 17 18 19 20   置1
	OEN_2 = OEN_2|0x001FE000;  //确认输出

	//初始化UART1
	RS232_Init();
    while(1){
    	//读模式信息，即读SW7、SW6的输入信息
    	flag = DATA_2_RO&0x00000060;

    	//读BTN8  BTN9的输入信息
    	flag1 = DATA_1_RO&0x000C0000; //
        switch(flag){
        case 0x00:					//复位模式
        	DATA_2 = DATA_2&0xFFE01FFF;		//模式指示灯LED7~LED0灭

        	break;
        case 0x20:					//手动控制模式
        	DATA_2 = (DATA_2|0x00002000)&0xFFFFBFFF;	//模式指示灯LED7亮、LED6灭
        	//读BTNU、BTND、BTNL、BTNR、BTNC按键的输入信息
        	//Car_run(4,0)  下降
        	//Car_run(4,1)  上升
        	//Car_run(1,0)  前进
        	//Car_run(1,1)  后退
        	//Car_run(2,0)  向左
        	//Car_run(2,1)  向右
        	//Car_run(3,*)  旋转
           	flag = DATA_2_RO & 0x0000001F;

           	if(flag1 == 0x00040000){//判断BTN8按键是否按下
           	           		DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	           	    flag = DATA_2_RO & 0x0000001F;
           	           	    while(flag == 0x00000001){			//判断BTNU按键是否抬起
           	           	         flag = DATA_2_RO & 0x0000001F;
           	           	    }
           	           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	           	    //发送机械臂“下降”命令
           	           	    Car_run(4,0);
           	           	}
           	if(flag1 == 0x00080000){//判断BTN9按键是否按下
           		DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	    flag = DATA_2_RO & 0x0000001F;
           	    while(flag == 0x00000001){			//判断BTNU按键是否抬起
           	         flag = DATA_2_RO & 0x0000001F;
           	    }
           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	    //发送机械臂“上升”命令
           	    Car_run(4,1);
           	}
           	if (flag == 0x00000001){				//判断BTNU按键是否按下
           	    DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	    flag = DATA_2_RO & 0x0000001F;
           	    while(flag == 0x00000001){			//判断BTNU按键是否抬起
           	    	flag = DATA_2_RO & 0x0000001F;
           	    }
           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	    //发送小车“前进”命令
           	    Car_run(1,0);
           	}
           	else if (flag == 0x00000002){			//判断BTND按键是否按下
           	           	    DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	           	    flag = DATA_2_RO & 0x0000001F;
           	           	    while(flag == 0x00000002){			//判断BTND按键是否抬起
           	           	    	flag = DATA_2_RO & 0x0000001F;
           	           	    }
           	           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	           	    //发送小车“后退”命令
           	           	    Car_run(1,1);
           	}
           	else if (flag == 0x00000004){				//判断BTNL按键是否按下
           	    DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	    flag = DATA_2_RO & 0x0000001F;
           	    while(flag == 0x00000004){			//判断BTNL按键是否抬起
           	    	flag = DATA_2_RO & 0x0000001F;
           	    }
           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	    //发送小车“左进”命令
           	    Car_run(2,0);
           	}
           	else if (flag == 0x00000008){			//判断BTNR按键是否按下
           	    DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	    flag = DATA_2_RO & 0x0000001F;
           	    while(flag == 0x00000008){			//判断BTNR按键是否抬起
           	    	flag = DATA_2_RO & 0x0000001F;
           	    }
           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	    //发送小车“右进”命令
           	    Car_run(2,1);
           	}
           	else if (flag == 0x00000010){			//判断BTNC按键是否按下
           	    DATA_0 = DATA_0 | 0x00000080;		//LED9指示灯亮
           	    delay(100,500,50);					//延时约1秒，进行消抖动处理
           	    flag = DATA_2_RO & 0x0000001F;
           	    while(flag == 0x000000010){			//判断BTNR按键是否抬起
           	    	flag = DATA_2_RO & 0x0000001F;
           	    }
           	    DATA_0 = DATA_0 & 0xFFFFFF7F;		//LED9指示灯灭
           	    //发送小车“旋转”命令
           	    Car_run(3,1);
           	}
           	if (DATA_2_RO == 0x00000240){
           		xipan(0);
           		delay(1000,500,100);

           	}
           	else if (DATA_2_RO == 0x00000040){
           		xipan(1);
           		delay(1000,500,100);
           	}


	        break;
        case 0x40:					//自动控制模式
        	//Car_run(4,0)  下降
        	//Car_run(4,1)  上升
        	//Car_run(1,0)  前进
        	//Car_run(1,1)  后退
        	//Car_run(2,0)  向左
        	//Car_run(2,1)  向右
        	//Car_run(3,*)  旋转

        	for(int i=0;i<11;i++)//向右
        	    Car_run(2,1)  ;
        	delay(500,500,50);

        	for(int i=0;i<3;i++){//向前
        	    Car_run(1,0)  ;
        	delay(500,500,50);}

        	xipan(0);			//吸
        	for(int i=0;i<3;i++)//上升
        	    Car_run(4,0)  ;
        	for(int i=0;i<13;i++){//向后
        		Car_run(1,1)  ;
        	delay(100,500,50);}
        	for(int i=0;i<18;i++){//右旋
        	    Car_run(3,1)  ;
        	    delay(100,500,50);
        	}
        	for(int i=0;i<19;i++){//向右
        	    Car_run(2,1)  ;
        	delay(100,500,50);}
        	for(int i=0;i<18;i++){//右旋
        	    Car_run(3,1)  ;
        	delay(100,500,50);
        	        	}
        	for(int i=0;i<35;i++){//向后
        	    Car_run(1,1)  ;
        	delay(100,500,50);}

        	for(int i=0;i<18;i++){//左旋
        	    Car_run(3,0)  ;
        	delay(100,500,50);}
        	for(int i=0;i<30;i++)
        	Car_run(6,0)  ;//开门
        	delay(1000,500,50);
        	for(int i=0;i<30;i++){//向右
        	    Car_run(2,1)  ;
        	delay(100,500,50);}

        	for(int i=0;i<15;i++){//向右进门
        	    Car_run(2,1)  ;
        	delay(100,500,50);}

        	xipan(1);
        	for(int i=0;i<45;i++){//向左
        	    Car_run(2,0)  ;
        	delay(100,500,50);}
        	for(int i=0;i<30;i++)
        	    Car_run(6,1)  ;//关门
        	delay(1000,500,50);

        	for(int i=0;i<18;i++)//左旋
        	    Car_run(3,0)  ;
        	delay(1000,500,50);
        	for(int i=0;i<35;i++)//向前
        	    Car_run(1,0)  ;
        	delay(1000,500,50);
        	for(int i=0;i<18;i++)//左旋
        	    Car_run(3,0)  ;
        	delay(1000,500,50);
        	for(int i=0;i<30;i++)//向左
        	    Car_run(2,0)  ;
        	delay(1000,500,50);
        	for(int i=0;i<18;i++)//右旋
        	    Car_run(3,1)  ;
        	delay(1000,500,50);
        	for(int i=0;i<10;i++)//向前
        	    Car_run(1,0)  ;
        	delay(5000,500,50);

        	break;
        case 0x60:					//机械臂示教模式（该模式暂不实现）
        	DATA_2 = DATA_2|0x00006000;					//LED7亮、LED6亮


        	break;


        }
    }
    return 0;
}

//小车各部件动作函数
void Car_run(int Car_ID,int Car_dir)
{
	unsigned char modbus_com[9];
	modbus_com[0]='#';				//起始符，固定为#
	modbus_com[1]='3';				//小车
	modbus_com[2]='0';
	modbus_com[3]='0';
    modbus_com[4]='0';
    modbus_com[5]='0';
    modbus_com[6]='0';
    modbus_com[7]='0';
    modbus_com[8]='0';

	switch(Car_ID){
	case 1:						//前后方向
	     if (Car_dir==0){
		    modbus_com[2]='1';
	     }
	     else if(Car_dir==1){
	    	modbus_com[2]='2';
	     }
	     else if(Car_dir==2){
	    	    	modbus_com[2]='0';
	    	     }
	     break;
	case 2:						//左右方向
		 if (Car_dir==0){
		    modbus_com[3]='1';
		 }
		 else if(Car_dir==1){
		    modbus_com[3]='2';
		 }
		 else if(Car_dir==2){
				    modbus_com[3]='0';
				 }
	   	 break;
	case 3:						//旋转方向
		if (Car_dir==0){
				    modbus_com[4]='1';
				 }
				 else if(Car_dir==1){
				    modbus_com[4]='2';
				 }
				 else if(Car_dir==2){
				    modbus_com[4]='0';
				 				 }
			   	 break;
	case 4:						//抬升臂
		 if (Car_dir==0){
		    modbus_com[5]='1';
		 }
		 else if(Car_dir==1){
		   	modbus_com[5]='2';
		 }
		 else if(Car_dir==2){
				   	modbus_com[5]='0';
				 }
	   	 break;
	case 5:						//1号门
		 modbus_com[6]='1';
		 break;
	case 6:						//2号门
		if (Car_dir==0){
				    modbus_com[7]='1';
				 }
				 else if(Car_dir==1){
				   	modbus_com[7]='2';
				 }
				 else if(Car_dir==2){
				 	modbus_com[7]='0';
				 				 }
			   	 break;


	case 7:						//3号门
			 modbus_com[8]='1';

		break;
	}


	send_Char_9(modbus_com);
}
void xipan(int xi_ID){
	unsigned char xi_com[9];
		xi_com[0]='#';				//起始符，固定为#
		xi_com[1]='5';
		xi_com[2]='0';
		xi_com[3]='0';
	    xi_com[4]='0';
	    xi_com[5]='0';
	    xi_com[6]='0';
	    xi_com[7]='0';
	    xi_com[8]='0';
	    switch(xi_ID){
	    case 0:
	    		xi_com[4]='1';//吸
	    		break;
	    case 1:
	            xi_com[4]='2';//放
	            break;
	    case 2:
	            xi_com[4]='0';
	            break;
	    }
	    send_Char_9(xi_com);
}

//9个字节数据的发送函数
void send_Char_9(unsigned char modbus[])
{
	int i;
	char data;
	for(i=0;i<9;i++){
		data=modbus[i];
		send_Char(data);
		delay(100,10,10);		//延时
	}
}

//单个字节数据的发送函数
void send_Char(unsigned char data)
{
     while((rChannel_sts_reg0&0x10)==0x10);
     rTx_Rx_FIFO0=data;
}

//UART1的初始化函数
void RS232_Init()
{
     rMIO_PIN_48=0x000026E0;
     rMIO_PIN_49=0x000026E0;
     rUART_CLK_CTRL=0x00001402;
     rControl_reg0=0x00000017;
     rMode_reg0=0x00000020;
     rBaud_rate_gen_reg0=62;
     rBaud_rate_divider_reg0=6;
}

//延时函数
void delay(int n,int m,int p)
{
	 int i,j,k;
	 for(i=1;i<=n;i++){
		 for(j=1;j<=m;j++){
			 for(k=1;k<=p;k++){

			 }
		 }
	 }
}
