#ifndef __MYIIC_H
#define __MYIIC_H

#include "stm32f0xx.h"
#include "lib_delay.h"

//PA9-->IIC_SCL PA10-->IIC_SDA  
#define IIC_GPIO GPIOA
#define IIC_SCL_PIN GPIO_Pin_9
#define IIC_SDA_PIN GPIO_Pin_10
#define IIC_SIZE 128

extern uint16_t SlaveAddr;   //�豸��ַ
extern uint8_t  SlaveAddrLen;//�豸��ַ�ֽ���
extern uint16_t SubAddr;     //Ƭ�ڵ�ַ
extern uint8_t  SubAddrLen;  //Ƭ�ڵ�ַ�ֽ���

extern uint8_t IICReadBuf[IIC_SIZE]; //��IIC����IICLen�����ݻ���
extern uint8_t IICWriteBuf[IIC_SIZE];//��д��IICLen�����ݻ���
extern uint16_t IICLen; //IIC��������

////IO��������  
//#define SDA_IN()  GPIO_InitTypeDef        GPIO_InitStructure;\
//									GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;\
//									GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;\
//									GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;\
//									GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
//									GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
//									GPIO_Init(IIC_GPIO, &GPIO_InitStructure);

//#define SDA_OUT() GPIO_InitTypeDef        GPIO_InitStructure;\
//									GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;\
//									GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;\
//									GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;\
//									GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
//									GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
//									GPIO_Init(IIC_GPIO, &GPIO_InitStructure);


//IO��������	 IIC_SCL_H
#define IIC_SCL_L  GPIO_WriteBit(IIC_GPIO,IIC_SCL_PIN,(BitAction)0) //SCL
#define IIC_SCL_H  GPIO_WriteBit(IIC_GPIO,IIC_SCL_PIN,(BitAction)1)
#define IIC_SDA_L  GPIO_WriteBit(IIC_GPIO,IIC_SDA_PIN,(BitAction)0) //SDA	
#define IIC_SDA_H  GPIO_WriteBit(IIC_GPIO,IIC_SDA_PIN,(BitAction)1)
#define READ_SDA   GPIO_ReadInputDataBit(IIC_GPIO, IIC_SDA_PIN)  //����SDA 
#define READ_SCL   GPIO_ReadInputDataBit(IIC_GPIO, IIC_SCL_PIN)) //����SCL




//IIC���в�������

void IIC_Init(void);                     //��ʼ��IIC��IO��				 
uint8_t IIC_Start(void);				             //����IIC��ʼ�ź�
void IIC_Stop(void);	  			           //����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			   //IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				     //IIC�ȴ�ACK�ź�
void IIC_Ack(void);					             //IIC����ACK�ź�
void IIC_NAck(void);				             //IIC������ACK�ź�
uint8_t IIC_StateCheck(uint8_t SlaveAddr);

 
uint8_t IIC_WriteDate(uint8_t SlaveAddr,uint16_t SubAddr,uint8_t *pWriteData,uint16_t Len);
uint8_t IIC_ReadDate(uint8_t SlaveAddr,uint16_t SubAddr,uint8_t *pReadData,uint16_t Len);

#endif
