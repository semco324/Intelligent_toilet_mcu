#include "lib_myiic.h"

uint16_t SlaveAddr;   //�豸��ַ
uint8_t  SlaveAddrLen=1;//�豸��ַ�ֽ���
uint16_t SubAddr;     //Ƭ�ڵ�ַ
uint8_t  SubAddrLen=2;  //Ƭ�ڵ�ַ�ֽ���
uint8_t IICReadBuf[IIC_SIZE]={0}; //��IIC����IICLen�����ݻ���
uint8_t IICWriteBuf[IIC_SIZE]={0};//��д��IICLen�����ݻ���
uint16_t IICLen; //IIC��������



//IO��������  
void SDA_IN(void) 
{	
	GPIO_InitTypeDef        GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(IIC_GPIO, &GPIO_InitStructure);
}

void SDA_OUT(void)
{	
	GPIO_InitTypeDef        GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(IIC_GPIO, &GPIO_InitStructure);
}



//��ʼ��IIC 
//PA9-->IIC_SCL PA10-->IIC_SDA  
void IIC_Init(void)
{				     
	GPIO_InitTypeDef        GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
  /* Configure PA9 and PA10 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IIC_GPIO, &GPIO_InitStructure);	
	
	IIC_SCL_H;
	IIC_SDA_H;
}

void IIC_close(void)
{				     
	GPIO_InitTypeDef        GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
  /* Configure PA9 and PA10 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(IIC_GPIO, &GPIO_InitStructure);	
	
	IIC_SCL_H;
	IIC_SDA_H;
}

//����IIC��ʼ�ź�
uint8_t IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(5);
	if(!READ_SDA) return 1; //SDA��Ϊ�͵�ƽ������æ,�˳�
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	if(READ_SDA) return 1; //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
	return 0;
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(5);
	IIC_SCL_H; 
	delay_us(5);			
	IIC_SDA_H;//����I2C���߽����ź�
	delay_us(5);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_H;delay_us(1);	   
	IIC_SCL_H;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
				delay_us(1);
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//ʱ�����0 	   
	return 0;  
} 

//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	delay_us(3);
	IIC_SDA_L;
	delay_us(3);
	IIC_SCL_H;
	delay_us(3);
	IIC_SCL_L;
	delay_us(3);
}
//����NACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	delay_us(3);
	IIC_SDA_H;
	delay_us(3);
	IIC_SCL_H;
	delay_us(3);
	IIC_SCL_L;
	delay_us(3);
}		
	
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
        if(((txd&0x80)>>7)){IIC_SDA_H;}
			  else {IIC_SDA_L;}
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H;
		delay_us(4); 
		IIC_SCL_L;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=0ʱ������ACK��ack=1������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	IIC_SCL_L; 
  for(i=0;i<8;i++ )
	{
        IIC_SCL_L; 
        delay_us(4);
		IIC_SCL_H;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(4); //delay_us(1); 
  }					 
    if (!ack)
        IIC_Ack();//����ACK
    else
        IIC_NAck(); //����NACK   
    return receive;
}



uint8_t IIC_WriteDate(uint8_t SlaveAddr,uint16_t SubAddr,uint8_t *pWriteData,uint16_t Len)
{
	 uint16_t i=0;
	
	 IIC_Start();
	
	 if(SlaveAddrLen==2)
	 {
	 IIC_Send_Byte(((SlaveAddr&0xFF00)>>8));
	 if(IIC_Wait_Ack()==1) return 1;	
	 delay_us(30);		 
	 }		 
	 IIC_Send_Byte(SlaveAddr&0x00FE);
	 if(IIC_Wait_Ack()==1) return 1;	
	 delay_us(30);
	
	
	 if(SubAddrLen==2)
	 {
	  IIC_Send_Byte(((SubAddr&0xFF00)>>8));
    if(IIC_Wait_Ack()==1) return 1;	
    delay_us(30);
	 }	 
	 IIC_Send_Byte((SubAddr&0x00FF));
   if(IIC_Wait_Ack()==1) return 1;	
	 delay_us(30);
	 
	 
	 for(i=0;i<Len;i++)
	 {
		IIC_Send_Byte((pWriteData[i]));
		if(IIC_Wait_Ack()==1) return 1;	
		  delay_us(30);
	 }
	 IIC_Stop();
	 return 0;		  
}

uint8_t IIC_ReadDate(uint8_t SlaveAddr,uint16_t SubAddr,uint8_t *pReadData,uint16_t Len)
{
	   uint8_t i=0;
	  
	   if(IIC_Start()==1)  return 1;    //Start
	
		 if(SlaveAddrLen==2)
		 {
		 IIC_Send_Byte(((SlaveAddr&0xFF00)>>8));
		 if(IIC_Wait_Ack()==1) return 1;	
		 delay_us(30);		 //30us
		 }		 
		 IIC_Send_Byte(SlaveAddr&0x00FE);
		 if(IIC_Wait_Ack()==1) return 1;	
		 delay_us(30);
	
	  if(SubAddrLen==2)
	  {
	    IIC_Send_Byte(((SubAddr&0xFF00)>>8));
      if(IIC_Wait_Ack()==1) return 1;		
			delay_us(30);
	  }		
	  IIC_Send_Byte((SubAddr&0x00FF));
    if(IIC_Wait_Ack()==1) return 1;	
		delay_us(30);
		
		IIC_Start();
		
		if(SlaveAddrLen==2)
		{
		IIC_Send_Byte(((SlaveAddr&0xFF00)>>8));
		if(IIC_Wait_Ack()==1) return 1;	
		delay_us(30);		 
		}		 
		IIC_Send_Byte(((SlaveAddr&0x00FE)|0x01));
		if(IIC_Wait_Ack()==1) return 1;	
		delay_us(30);
		
		for(i=0;i<(Len-1);i++)
		{
		   pReadData[i]=IIC_Read_Byte(0);
			 delay_us(30);
		}
		pReadData[Len-1]=IIC_Read_Byte(1);
		delay_us(30);
		IIC_Stop();
	  return 0;				
}



uint8_t IIC_StateCheck(uint8_t SlaveAddr)
{
	  if(IIC_Start()==1){IIC_Stop();return 1;}
	
		if(SlaveAddrLen==2)
		{
		IIC_Send_Byte(((SlaveAddr&0xFF00)>>8));
		if(IIC_Wait_Ack()==1) {IIC_Stop(); return 1;}	
		delay_us(30);		 
		}		 
		IIC_Send_Byte(SlaveAddr&0x00FE);
		if(IIC_Wait_Ack()==1) {IIC_Stop(); return 1;}	
		delay_us(30); 

    IIC_Stop();
	  return 0;	
}




