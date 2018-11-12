#ifndef __MYIIC_H
#define __MYIIC_H

#include "stm32f0xx.h"
#include "lib_delay.h"

//PA9-->IIC_SCL PA10-->IIC_SDA  
#define IIC_GPIO GPIOA
#define IIC_SCL_PIN GPIO_Pin_9
#define IIC_SDA_PIN GPIO_Pin_10
#define IIC_SIZE 128

extern uint16_t SlaveAddr;   //设备地址
extern uint8_t  SlaveAddrLen;//设备地址字节数
extern uint16_t SubAddr;     //片内地址
extern uint8_t  SubAddrLen;  //片内地址字节数

extern uint8_t IICReadBuf[IIC_SIZE]; //从IIC读到IICLen个数据缓存
extern uint8_t IICWriteBuf[IIC_SIZE];//待写入IICLen个数据缓存
extern uint16_t IICLen; //IIC操作长度

////IO方向设置  
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


//IO操作函数	 IIC_SCL_H
#define IIC_SCL_L  GPIO_WriteBit(IIC_GPIO,IIC_SCL_PIN,(BitAction)0) //SCL
#define IIC_SCL_H  GPIO_WriteBit(IIC_GPIO,IIC_SCL_PIN,(BitAction)1)
#define IIC_SDA_L  GPIO_WriteBit(IIC_GPIO,IIC_SDA_PIN,(BitAction)0) //SDA	
#define IIC_SDA_H  GPIO_WriteBit(IIC_GPIO,IIC_SDA_PIN,(BitAction)1)
#define READ_SDA   GPIO_ReadInputDataBit(IIC_GPIO, IIC_SDA_PIN)  //输入SDA 
#define READ_SCL   GPIO_ReadInputDataBit(IIC_GPIO, IIC_SCL_PIN)) //输入SCL




//IIC所有操作函数

void IIC_Init(void);                     //初始化IIC的IO口				 
uint8_t IIC_Start(void);				             //发送IIC开始信号
void IIC_Stop(void);	  			           //发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			   //IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				     //IIC等待ACK信号
void IIC_Ack(void);					             //IIC发送ACK信号
void IIC_NAck(void);				             //IIC不发送ACK信号
uint8_t IIC_StateCheck(uint8_t SlaveAddr);

 
uint8_t IIC_WriteDate(uint8_t SlaveAddr,uint16_t SubAddr,uint8_t *pWriteData,uint16_t Len);
uint8_t IIC_ReadDate(uint8_t SlaveAddr,uint16_t SubAddr,uint8_t *pReadData,uint16_t Len);

#endif
