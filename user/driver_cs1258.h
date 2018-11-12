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
#define	FILTER_AD_TIMES		  4		  //����AD����
#define RES_AD_TIMES			  12//8		  //�ο�����ɼ�����
#define	BODY_AD_TIMES			  20//12		  //�������ɼ�����
//#define RES_STABLE_COUNT		150		//�ο�����AD�ȶ���Χ
//#define	BODY_STABLE_COUNT		350		//�������AD�ȶ���Χ


#define POS_NEG_VAL				3000 	//��������ֵ

#define RES_MIN12     2500			  //250ohm  Z12��Сֵ��250ohm

#define	RES_MIN				2000			  //200ohm
#define	REF_RES				7000			  //700ohm
#define	RES_MAX				15000		  //1500ohm


#define K_RES_MIN       3000      //У׼��֤������С
#define K_RES_MAX       10000     //У׼��֤�������

extern uint32_t BIA_FLAG;   //����BIA�ı�־λ��FunctionBIAFlag
extern uint8_t  BIA_Pointer; 
extern uint8_t freq[3]; //�������Ƶ�Σ�ע��ֻʹ�õ�4bit����4bitΪ0
//freq[0] :��1Ƶ��Ƶ��
//freq[1] :��2Ƶ��Ƶ��
//freq[2] :��3Ƶ��Ƶ��
extern uint8_t f_cnt;   //��¼Ƶ�θ��� 
//0:1��Ƶ�� 
//1:2��Ƶ�� 
//2:3��Ƶ�� 
extern uint8_t test_sta;//����״̬
//0:leg_1 1:hand_1 2:body_1
//3:leg_2 4:hand_2 5:body_2
//6:leg_3 7:hand_3 8:body_3


extern uint8_t  wait_out;

//50Khz 
#define	Short_FWR_50KHz 	0x33	//�ڶ�ģʽ 
#define	Ref0_FWR_50KHz		0x13	//�ο�����0
#define	Ref1_FWR_50KHz		0x23	//�ο�����1
#define	Body_FWR_50KHz		0x03	//����ģʽ

#define __5KHz           0x00  //5k
#define __10KHz          0x01  //10k
#define __50KHz          0x03  //50k
#define __100KHz         0x04  //100k
#define __250KHz         0x05  //250k


/*CS125x �Ĵ���*/
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



  //AFE IO�ڶ���
	//PA8-->CS1258_CS  PA9-->CS1258_SCL  PA10-->CS1258_SDA  
	#define  CS1258_GPIO 			GPIOA
	#define  CS1258_GPIO_CLK 	RCC_AHBPeriph_GPIOA
	#define  FAT_SDA   				GPIO_Pin_10
	#define  FAT_SCL   				GPIO_Pin_9
	#define  FAT_CS    				GPIO_Pin_8

		
	//==============================================================================
	//���� FAT SDA����
#define  SetFATSDAInput(FAT_SDA)  GPIO_InitTypeDef        GPIO_InitStructure;\
																	GPIO_InitStructure.GPIO_Pin = FAT_SDA;\
																	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;\
																	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;\
																	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
																	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
																	GPIO_Init(CS1258_GPIO, &GPIO_InitStructure);
	//���� FAT SDA���
#define  SetFATSDAOutput(FAT_SDA) GPIO_InitTypeDef        GPIO_InitStructure;\
																	GPIO_InitStructure.GPIO_Pin = FAT_SDA;\
																	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;\
																	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;\
																	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
																	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;\
																	GPIO_Init(CS1258_GPIO, &GPIO_InitStructure);

	//�� FAT SDA�����״̬
	#define  GetFATSDAInputStatus(FAT_SDA)	GPIO_ReadInputDataBit(CS1258_GPIO, FAT_SDA)  //����SDA 
	
	//FAT SDA �����
	#define  SetFATSDA(FAT_SDA)          GPIO_WriteBit(CS1258_GPIO,FAT_SDA,(BitAction)1) //SDA	
	//FAT SDA �����
	#define  ClrFATSDA(FAT_SDA)          GPIO_WriteBit(CS1258_GPIO,FAT_SDA,(BitAction)0) //SDA	
	
	
	//FAT SCL �����
	#define  SetFATSCL(FAT_SCL)          GPIO_WriteBit(CS1258_GPIO,FAT_SCL,(BitAction)1) //SCL	
	//FAT SCL �����
	#define  ClrFATSCL(FAT_SCL)          GPIO_WriteBit(CS1258_GPIO,FAT_SCL,(BitAction)0) //SCL	
	
	
	//FAT CS �����
	#define  SetFATCS(FAT_CS)          GPIO_WriteBit(CS1258_GPIO,FAT_CS,(BitAction)1) //CS	
	//FAT CS �����
	#define  ClrFATCS(FAT_CS)          GPIO_WriteBit(CS1258_GPIO,FAT_CS,(BitAction)0) //CS	
	

	//==============================================================================
	
extern uint8_t waste_times; //����AD����
extern uint8_t	all_times;   //�ܹ�AD����
extern uint8_t	read_times;  //��¼��AD����
extern uint8_t	out_times;   //��ʱ����
					
extern uint32_t lastADC;    //�ϴ�AD
extern uint32_t currentADC; //��ǰAD
extern uint32_t sumADC;     //AD�ۼӺ�
extern uint16_t temp_avg;   //ADC
extern uint16_t temp_res;   //RES

extern uint16_t  f1k_val;   //1Ƶkֵ
//extern uint16_t  f2k_val;   //2Ƶkֵ
//extern uint16_t  f3k_val;   //3Ƶkֵ
extern uint16_t  fk_val;    //��Ƶkֵ

//extern uint8_t  fk_presion; //��Ƶ����

//0:leg_1 1:hand_1 2:body_1
//3:leg_2 4:hand_2 5:body_2
//6:leg_3 7:hand_3 8:body_3
typedef enum 
{		
	LEG_1  = 0, 
	HAND_1 = 1,
	BODY_1 = 2,
}FunctionTestSta; //test_sta 9��״̬

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
	F1 = 0,   //��һƵ
}FunctionFreq;


//flag 
typedef enum 
{		
	GetRef   		=0,    //�ѻ�������ο�  1 ��    0 û�� 
	ADRedy   		=1,    //AD����һ������	 1 ��    0 û��
	GetBodyDone =2,    //�����Ƿ����    1 ���  0 û��
	CalKDone    =3,    //У׼K�Ƿ����   1 ���  0 û��
//	Enable_init	=4,   //�Ƿ������л�ͨ�� 1 ����  0 ��
	BErr_Short  =5,   //�ڶ̳����־     1 ��    0 ��
	BErr_Hight  =6,   //���޳����־     1 ��    0 ��
	BErr_Low    =7,   //���޳����־     1 ��    0 ��
	BErr_post   =8,   //վ���쳣�ж�     1 ��    0 ��
	BErr_stb    =9,   //�ж��ȶ�����     1 �޷����� 0 ��
}FunctionBIAFlag;



// �������� Pointer
typedef enum   //�ڶ�
{		
	Short_Init_1   =0,   //�ڶ̲�����ʼ��	
	Ref0_Init_1    =1,   //�ο�0������ʼ��
	Ref1_Init_1    =2,   //�ο�1������ʼ��
}FunctionRefPointer;

typedef enum   //leg
{		
	Z34_Init_1    =1,   //Z34������ʼ�� F1
	Z43_Init_1    =2,   //Z43������ʼ�� F1
}FunctionLegPointer;

typedef enum   //hand
{		
	Z12_Init_1    =1,   //Z12������ʼ�� F1
	Z21_Init_1    =2,   //Z21������ʼ�� F1
}FunctionHandPointer;

typedef enum   //Body
{		
	B34_Init_1    =1,   //B34������ʼ�� F1
	B43_Init_1    =2,   //B43������ʼ�� F1
	B12_Init_1    =3,   //B12������ʼ�� F1
	B21_Init_1    =4,   //B21������ʼ�� F1	
	B13_Init_1    =5,   //B13������ʼ�� F1	
	B14_Init_1    =6,   //B14������ʼ�� F1	
	B23_Init_1    =7,   //B23������ʼ�� F1	
	B24_Init_1    =8,   //B24������ʼ�� F1	
}FunctionBodyPointer;



typedef enum   //leg boyd Cal
{		
	C34_Init_1    =1,   //Z34������ʼ�� F1 
	C43_Init_1    =2,   //Z43������ʼ�� F1
}FunctionCalLegBoydPointer;

typedef enum   //hand Cal
{		
	C12_Init_1    =1,   //Z12������ʼ�� F1 
	C21_Init_1    =2,   //Z12������ʼ�� F1
}FunctionCalHandPointer;


typedef struct
{   
	//bim0 config
	uint8_t BIM0_ISINO; //�������
	uint8_t BIM0_ISINI; //��������
	uint8_t BIM0_VSENP; //��ѹ����
	uint8_t BIM0_VSENN; //��ѹ����
	
	//bim1 config
	uint8_t BIM1_MIXorFWR;
	uint8_t BIM_Mode;
	
	//�˻���ref
	uint8_t HUMANorREF; 
	
	uint8_t BIM0;    //��������BIM0
	uint8_t BIM1[3]; //��������BIM1 ���֧��3Ƶ
	
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
