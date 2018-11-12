#ifndef __DRIVER_CS1258_H
#define __DRIVER_CS1258_H

#include "stm32f0xx.h"

#include "lib_delay.h"
#include "addr_map.h"
#include "timer.h"
#include "flash.h"
#include "csm37f59.h"

/*
 1(inis0)     2(inis1)
			     5
 3(inis2)     4(inis3)

*/
#define	FILTER_AD_TIMES		  4		  //丢掉AD次数
#define RES_AD_TIMES			  12//8		  //参考电阻采集次数
#define	BODY_AD_TIMES			  20//12		  //人体电阻采集次数
//#define RES_STABLE_COUNT		150		//参考电阻AD稳定范围
//#define	BODY_STABLE_COUNT		350		//人体电阻AD稳定范围


#define POS_NEG_VAL				3000 	//正反测阈值

#define RES_MIN12     2500			  //250ohm  Z12最小值是250ohm

#define	RES_MIN				2000			  //200ohm
#define	REF_RES				7000			  //700ohm
#define	RES_MAX				15000		  //1500ohm


#define K_RES_MIN       3000      //校准验证电阻最小
#define K_RES_MAX       10000     //校准验证电阻最大

extern uint32_t BIA_FLAG;   //关于BIA的标志位见FunctionBIAFlag
extern uint8_t  BIA_Pointer; 
extern uint8_t freq[3]; //存放所有频段，注意只使用低4bit，高4bit为0
//freq[0] :第1频段频率
//freq[1] :第2频段频率
//freq[2] :第3频段频率
extern uint8_t f_cnt;   //记录频段个数 
//0:1个频段 
//1:2个频段 
//2:3个频段 
extern uint8_t test_sta;//测试状态
//0:leg_1 1:hand_1 2:body_1
//3:leg_2 4:hand_2 5:body_2
//6:leg_3 7:hand_3 8:body_3


extern uint8_t  wait_out;

//50Khz 
#define	Short_FWR_50KHz 	0x33	//内短模式 
#define	Ref0_FWR_50KHz		0x13	//参考电阻0
#define	Ref1_FWR_50KHz		0x23	//参考电阻1
#define	Body_FWR_50KHz		0x03	//人体模式

#define __5KHz           0x00  //5k
#define __10KHz          0x01  //10k
#define __50KHz          0x03  //50k
#define __100KHz         0x04  //100k
#define __250KHz         0x05  //250k


/*CS125x 寄存器*/
#define	ADDR_AFE_SYS		0
#define ADDR_AFE_ADC0		1
#define ADDR_AFE_ADC1		2
#define ADDR_AFE_ADC2		3
#define ADDR_AFE_ADC3		4
#define ADDR_AFE_ADC4		5
#define ADDR_AFE_ADC5	 	6
#define ADDR_AFE_BIM0		7
#define ADDR_AFE_BIM1		8
#define ADDR_AFE_ADO		9
#define ADDR_AFE_ADS		10

#define ADDR_AFE_CONFIG		0x10
#define ADDR_AFE_VREFT		0x11
#define ADDR_AFE_LDOT			0x12
#define ADDR_AFE_OSCT			0x13
#define ADDR_AFE_TST1			0x14
#define ADDR_AFE_TST2			0x15
#define ADDR_AFE_TMUX			0x16
#define ADDR_AFE_OTPCON		0x59
#define ADDR_AFE_TEST			0x5A



  //AFE IO口定义
	//PA8-->CS1258_CS  PA9-->CS1258_SCL  PA10-->CS1258_SDA  
	#define  CS1258_GPIO 			GPIOA
	#define  CS1258_GPIO_CLK 	RCC_AHBPeriph_GPIOA
	#define  FAT_SDA   				GPIO_Pin_10
	#define  FAT_SCL   				GPIO_Pin_9
	#define  FAT_CS    				GPIO_Pin_8

		
	//==============================================================================
	//设置 FAT SDA输入
#define  SetFATSDAInput(FAT_SDA)  GPIO_InitTypeDef        GPIO_InitStructure;\
																	GPIO_InitStructure.GPIO_Pin = FAT_SDA;\
																	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;\
																	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;\
																	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
																	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
																	GPIO_Init(CS1258_GPIO, &GPIO_InitStructure);
	//设置 FAT SDA输出
#define  SetFATSDAOutput(FAT_SDA) GPIO_InitTypeDef        GPIO_InitStructure;\
																	GPIO_InitStructure.GPIO_Pin = FAT_SDA;\
																	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;\
																	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;\
																	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
																	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;\
																	GPIO_Init(CS1258_GPIO, &GPIO_InitStructure);

	//读 FAT SDA输入的状态
	#define  GetFATSDAInputStatus(FAT_SDA)	GPIO_ReadInputDataBit(CS1258_GPIO, FAT_SDA)  //输入SDA 
	
	//FAT SDA 输出高
	#define  SetFATSDA(FAT_SDA)          GPIO_WriteBit(CS1258_GPIO,FAT_SDA,(BitAction)1) //SDA	
	//FAT SDA 输出低
	#define  ClrFATSDA(FAT_SDA)          GPIO_WriteBit(CS1258_GPIO,FAT_SDA,(BitAction)0) //SDA	
	
	
	//FAT SCL 输出高
	#define  SetFATSCL(FAT_SCL)          GPIO_WriteBit(CS1258_GPIO,FAT_SCL,(BitAction)1) //SCL	
	//FAT SCL 输出低
	#define  ClrFATSCL(FAT_SCL)          GPIO_WriteBit(CS1258_GPIO,FAT_SCL,(BitAction)0) //SCL	
	
	
	//FAT CS 输出高
	#define  SetFATCS(FAT_CS)          GPIO_WriteBit(CS1258_GPIO,FAT_CS,(BitAction)1) //CS	
	//FAT CS 输出低
	#define  ClrFATCS(FAT_CS)          GPIO_WriteBit(CS1258_GPIO,FAT_CS,(BitAction)0) //CS	
	

	//==============================================================================
	
extern uint8_t waste_times; //丢掉AD次数
extern uint8_t	all_times;   //总共AD次数
extern uint8_t	read_times;  //记录读AD次数
extern uint8_t	out_times;   //超时计数
					
extern uint32_t lastADC;    //上次AD
extern uint32_t currentADC; //当前AD
extern uint32_t sumADC;     //AD累加和
extern uint16_t temp_avg;   //ADC
extern uint16_t temp_res;   //RES

extern uint16_t  f1k_val;   //1频k值
//extern uint16_t  f2k_val;   //2频k值
//extern uint16_t  f3k_val;   //3频k值
extern uint16_t  fk_val;    //该频k值

//extern uint8_t  fk_presion; //该频精度

//0:leg_1 1:hand_1 2:body_1
//3:leg_2 4:hand_2 5:body_2
//6:leg_3 7:hand_3 8:body_3
typedef enum 
{		
	LEG_1  = 0, 
	HAND_1 = 1,
	BODY_1 = 2,
}FunctionTestSta; //test_sta 9个状态

//typedef enum 
//{		
//	TRUE = 0, 
//	FALSE =1,
//}BOOL;

typedef enum 
{		
	HH = 0, 
	HL =1,
	LL = 2, 
	LH =3,	
//	H=4,
//	L=5,
}HLX;   

typedef enum 
{			
	H=4,
	L=5,
}XHL;

typedef enum 
{		
	REF = 0, 
	HUMAN = !REF
}CS125xState;

typedef enum 
{		
	FWR = 0, 
	MIX = !FWR
}FunctionDemodulate;

typedef enum 
{		
	TEST_MODE = 0, 
	RES0_MODE = 1,
	RES1_MODE = 2,
	SHORT_MODE = 3
}FunctionBimMode;


typedef enum 
{		
	ISIN0 = 0, 
	ISIN1 = 1, 
	ISIN2 = 2, 
	ISIN3 = 3, 
}FunctionISIN;

typedef enum 
{		
	VSEN0 = 0, 
	VSEN1 = 1, 
	VSEN2 = 2, 
	VSEN3 = 3, 
}FunctionVSEN;

typedef enum 
{		
	F1 = 0,   //第一频
}FunctionFreq;


//flag 
typedef enum 
{		
	GetRef   		=0,    //已获得完整参考  1 是    0 没有 
	ADRedy   		=1,    //AD来了一笔数据	 1 是    0 没有
	GetBodyDone =2,    //测人是否完成    1 完成  0 没有
	CalKDone    =3,    //校准K是否完成   1 完成  0 没有
//	Enable_init	=4,   //是否允许切换通道 1 不是  0 是
	BErr_Short  =5,   //内短出错标志     1 错    0 对
	BErr_Hight  =6,   //上限出错标志     1 错    0 对
	BErr_Low    =7,   //下限出错标志     1 错    0 对
	BErr_post   =8,   //站姿异常判断     1 错    0 对
	BErr_stb    =9,   //判断稳定出错     1 无法判稳 0 能
}FunctionBIAFlag;



// 连续排列 Pointer
typedef enum   //内短
{		
	Short_Init_1   =0,   //内短参数初始化	
	Ref0_Init_1    =1,   //参考0参数初始化
	Ref1_Init_1    =2,   //参考1参数初始化
}FunctionRefPointer;

typedef enum   //leg
{		
	Z34_Init_1    =1,   //Z34参数初始化 F1
	Z43_Init_1    =2,   //Z43参数初始化 F1
}FunctionLegPointer;

typedef enum   //hand
{		
	Z12_Init_1    =1,   //Z12参数初始化 F1
	Z21_Init_1    =2,   //Z21参数初始化 F1
}FunctionHandPointer;

typedef enum   //Body
{		
	B34_Init_1    =1,   //B34参数初始化 F1
	B43_Init_1    =2,   //B43参数初始化 F1
	B12_Init_1    =3,   //B12参数初始化 F1
	B21_Init_1    =4,   //B21参数初始化 F1	
	B13_Init_1    =5,   //B13参数初始化 F1	
	B14_Init_1    =6,   //B14参数初始化 F1	
	B23_Init_1    =7,   //B23参数初始化 F1	
	B24_Init_1    =8,   //B24参数初始化 F1	
}FunctionBodyPointer;



typedef enum   //leg boyd Cal
{		
	C34_Init_1    =1,   //Z34参数初始化 F1 
	C43_Init_1    =2,   //Z43参数初始化 F1
}FunctionCalLegBoydPointer;

typedef enum   //hand Cal
{		
	C12_Init_1    =1,   //Z12参数初始化 F1 
	C21_Init_1    =2,   //Z12参数初始化 F1
}FunctionCalHandPointer;


typedef struct
{   
	//bim0 config
	uint8_t BIM0_ISINO; //电流输出
	uint8_t BIM0_ISINI; //电流输入
	uint8_t BIM0_VSENP; //电压正端
	uint8_t BIM0_VSENN; //电压负端
	
	//bim1 config
	uint8_t BIM1_MIXorFWR;
	uint8_t BIM_Mode;
	
	//人或者ref
	uint8_t HUMANorREF; 
	
	uint8_t BIM0;    //解析到的BIM0
	uint8_t BIM1[3]; //解析到的BIM1 最多支持3频
	
}BIM_InitTypeDef;



extern BIM_InitTypeDef BIM_Init; 
extern uint8_t wait_out;
uint16_t get1byte(uint32_t temp,HLX X);
uint16_t get2byte(uint32_t a,uint32_t b,XHL X);


void SendDataToAddr(uint8_t addr,uint8_t data);
void Ref_Init_All(FunctionBimMode BimMode,FunctionFreq Fx);
void Res_Init_All(FunctionISIN ISINO,FunctionISIN ISINI,FunctionVSEN VSENP,FunctionVSEN VSENN,FunctionFreq Fx);

uint32_t ReadADC(void);

void tab_ref_test(void);
void GetBodyRes(void);
void Reset_CS125x(void);
void InitCS125x(void);
void PowerDownCS125x(void);
void GetRef01ADC(FunctionFreq Fx);
void Parm_Rational(void);
void Calibrate_K(void);
uint16_t get2byte(uint32_t a,uint32_t b,XHL X);
void _CS1258bug(void);

#endif /*__CS1258_H*/
