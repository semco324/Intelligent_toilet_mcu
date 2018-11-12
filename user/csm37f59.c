#include "csm37f59.h"
#include <string.h>


uint8_t tim_cnt=0;  //INTERVAL_SEND*10 ��ʱ
uint8_t sta_flg=CS1258_IDLE;  //CSM37F58 ״̬ 

uint8_t mes_flg=0;  //����������� 1.��ʼ�� 0.�ر�
uint8_t calres_flg=0; //У׼���迪�� 1.��У׼ 0.�ر�
uint8_t calhr_flg=0;  //У׼���ʿ��� 1.��У׼ 0.�ر�

uint8_t mt_status=0;    //��ǰ��Ͱ����״̬ 0 ��״̬ 1 ��ʼ����� 2 ��ʼ������ 
uint8_t premt_status=0; //�ϴ���Ͱ����״̬

a_res_t a_res={{0},0,0,0};



//blestatus  //�������� EXTI_Trigger_Falling
//#define BLEStatus_GPIO      GPIOB
//#define BLEStatus_Pin       GPIO_Pin_5
void setup_key_enable(void)
{
	//blestatus
	GPIO_InitTypeDef        GPIO_InitStructure;
	EXTI_InitTypeDef        EXTI_InitStructure;
	NVIC_InitTypeDef        NVIC_InitStructure;	
	
  /* Enable GPIOB	clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PB5 pins as input floating */
  GPIO_InitStructure.GPIO_Pin = BLEStatus_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BLEStatus_GPIO, &GPIO_InitStructure);
  
   /* Connect EXTI5 Line to PB5 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);

  /* Configure EXTI5 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /* Enable and set EXTI4_15 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

void test_gpio(void)
{
	//test_gpio
	GPIO_InitTypeDef        GPIO_InitStructure;
	
  /* Enable GPIOB	clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  /* Configure PB12 pins as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void setup_key_disable(void)
{	
	//blestatus
	GPIO_InitTypeDef        GPIO_InitStructure;
	EXTI_InitTypeDef        EXTI_InitStructure;
	NVIC_InitTypeDef        NVIC_InitStructure;	
	
  /* Enable GPIOB	clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PB5 pins as input floating */
  GPIO_InitStructure.GPIO_Pin = BLEStatus_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BLEStatus_GPIO, &GPIO_InitStructure);
  
   /* Connect EXTI5 Line to PB5 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);

  /* Configure EXTI5 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /* Enable and set EXTI4_15 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}




/*
ִ������ 1.if((BIA_FLAG>>GetRef)&0x01) ���� 
         2.sta_flg==0  CSM37F58���ڿ���״̬
         3.��������(���滻�����ȳɹ�)
�����־ 
*/
//sta_flg��״̬˵��
//#define CS1258_IDLE                       0       // 0 ����״̬
//#define CS1258_SEND_MEASURE_RES_CMD       1       // 1 �����˲�����������
//#define CS1258_START_MEASURE_RES          2       // 2 DRDY���߿�ʼ��������
//#define CS1258_END_MEASURE_RES            3       // 3 DRDY���͵���������
//#define CS1258_SEND_MEASURE_HR            4       // 4 ���������ʲ�������
//#define CS1258_START_MEASURE_HR           5       // 5 DRDY���߿�ʼ��������
//#define CS1258_END_MEASURE_HR             6       // 6 DRDY�������ʲ������

//#define CS1258_SEND_CAL_K                 7       // 7 ������У׼Kֵ����
//#define CS1258_START_CAL_K                8       // 8 DRDY���߿�ʼУ׼Kֵ
//#define CS1258_END_CAL_K                  9       // 9 DRDY����У׼Kֵ���

//#define CS1258_SEND_CAL_HR                10      // 10����������У׼����
//#define CS1258_START_CAL_HR               11      // 11DRDY���߿�ʼУ׼����
//#define CS1258_END_CAL_HR                 12      // 12DRDY����У׼�������


extern uint8_t done;
void measure(void)
{
		if((((BIA_FLAG>>GetRef)&0x01) == 1) && sta_flg == CS1258_IDLE) //�Ѿ���������ο� && 	����
		{
			/*********************����ģʽ****************************/
			 if(mes_flg == 1)			//�����������
				{  	
            mes_flg=0;done=0;calres_flg=0;calhr_flg=0;
					
					  memset(&a_res,0,sizeof(a_res));
					  cur_result_pkg=0;
						sta_flg = CS1258_SEND_MEASURE_RES_CMD;     // 1 �����˲�����������
            EquMent.STATUS = 0x80; 
						mt_status=1; //��ʼ�����

				}
			 /*********************У׼ģʽ****************************/
				else if(calres_flg == 1) //У׼����
				{
						mes_flg=0;done=0;calres_flg=0;calhr_flg=0;
						
						EquMent.FREQ1_CalRes_H = DEF_CALRES1_H;//дУ׼����		
						EquMent.FREQ1_CalRes_L = DEF_CALRES1_L;

						EquMent.CONFIG_K = 0x80;
						sta_flg = CS1258_SEND_CAL_K_CMD;           // 7 ������У׼Kֵ����
						mt_status=1; //��ʼУ׼����

				}
				else if(calhr_flg == 1) //У׼����
				{
						mes_flg=0;done=0;calres_flg=0;calhr_flg=0;
						sta_flg = CS1258_SEND_CAL_HR_CMD;          // 10����������У׼����
						EquMent.STATUS = 0x40; 
						mt_status=2; //��ʼУ����
				}
    }
}
#define A_RANGE  100

uint16_t resabs(uint16_t x,uint16_t y)
{
	return ( x >=y ? (x-y):(y-x) );
}

void arr_delete(a_res_t * a_res)
{
	uint8_t i=0;
	for(i=0;i<A_RES_SIZE-1;i++) //ע������iֻ�ܵ�A_RES_SIZE-1 (4)
  {
	  a_res->res[i] = a_res->res[i+1];//1->0 2->1 3->2 4->3 5->4
	}
}

uint16_t arr_agv(a_res_t * a_res)
{
	 uint8_t i=0; uint32_t sum=0;
   for(i=0;i<A_RES_SIZE;i++)
	{
	  sum = sum + a_res->res[i];
	}
	return sum/A_RES_SIZE;
}


void queue_a_res(void)
{
	
	if(EquMent.ERR_STATUS==0) //����һ��
	{
				if(a_res.num <= (A_RES_SIZE-1)) //δ��
				{
						a_res.res[a_res.num] = ((UsrBIAP.Freq1_All_Res_H&0xff) <<8) | (UsrBIAP.Freq1_All_Res_L&0xff); //�����
						a_res.num++;
					
            goto 	exit_lp1; //�ٲ�һ��
				}
				else  //�� a_res.num == A_RES_SIZE;
				{
					  a_res.num = A_RES_SIZE-1;
						if( resabs(a_res.res[0],a_res.res[A_RES_SIZE-1]) < A_RANGE &&\
							  resabs(a_res.res[A_RES_SIZE/2],a_res.res[A_RES_SIZE-1]) < A_RANGE &&\
						    resabs(a_res.res[0],a_res.res[A_RES_SIZE/2])  < A_RANGE \
						  )
						{
							  a_res.ok_num++;
							  if(a_res.ok_num >=3)
								{
								   UsrBIAP.Freq1_All_Res_H = arr_agv(&a_res) >>8;
									 UsrBIAP.Freq1_All_Res_L = arr_agv(&a_res) &0xff;
									 
									 //����ɷ�
									 sta_flg = CS1258_SEND_MEASURE_HR_CMD;   // 4 ���������ʲ�������
									 EquMent.STATUS = 0x40; 
									 mt_status=2; //��ʼ������
									 return;
								}
								else
								{
								    arr_delete(&a_res);
										goto exit_lp1;//�ٲ�һ��
								}
						}
						else
						{
						   a_res.ok_num = 0;
							 arr_delete(&a_res);
						   goto exit_lp1;
						}
				}
	}
	else //�쳣���ֱ�ӽ��� err_num
	{
		exit_lp0:
			a_res.err_num++;
			if(a_res.err_num > 5)
			{
			      sta_flg = CS1258_END_MEASURE_HR;      // 6 DRDY�������ʲ������
						return;
			}
			else //�ٲ�һ��
			{
		exit_lp1:		
						sta_flg = CS1258_SEND_MEASURE_RES_CMD;     // 1 �����˲�����������
						EquMent.STATUS = 0x80; 
						mt_status=1; //��ʼ�����
						BIA_FLAG&=~(1<<GetBodyDone);							 
						BIA_FLAG&=~(1<<BErr_Short);	
						BIA_FLAG&=~(1<<BErr_Hight);	
						BIA_FLAG&=~(1<<BErr_Low);	
						BIA_FLAG&=~(1<<BErr_post);	
						BIA_FLAG&=~(1<<BErr_stb);		
						EquMent.ERR_STATUS=0;		
						return;
			}
	}
	
}


//void update_sta_flg(void)
//{			
//	    /******************�������������******************************/		
//			if(sta_flg == CS1258_START_MEASURE_RES)  // 2 DRDY���߿�ʼ��������
//			{
//				  
//					sta_flg = CS1258_END_MEASURE_RES;    // 3 DRDY���͵���������
//			     //EquMent.ERR_STATUST //�������־		
//	
//			    if(UsrBIAP.Freq1_All_Res_H==0 && UsrBIAP.Freq1_All_Res_L==0) //��������쳣��ֱ�ӷ���,�������һ��sta_flg = 6
//					{
//			      sta_flg = CS1258_END_MEASURE_HR;      // 6 DRDY�������ʲ������
//						return;
//					}
//			    sta_flg = CS1258_SEND_MEASURE_HR_CMD;   // 4 ���������ʲ�������
//					EquMent.STATUS = 0x40; 
//			    mt_status=2; //��ʼ������
//					return;
//			}
//		  /******************У׼����******************************/		
//			else if(sta_flg == CS1258_START_CAL_K)  // 8 DRDY���߿�ʼУ׼Kֵ
//			{
//					 sta_flg = CS1258_END_CAL_K;        // 9 DRDY����У׼Kֵ���
//			}
//}
void update_sta_flg(void)
{			
	    /******************�������������******************************/		
			if(sta_flg == CS1258_START_MEASURE_RES)  // 2 DRDY���߿�ʼ��������
			{
				  
					sta_flg = CS1258_END_MEASURE_RES;    // 3 DRDY���͵���������	
	
				   queue_a_res();
//			    if(UsrBIAP.Freq1_All_Res_H==0 && UsrBIAP.Freq1_All_Res_L==0) //��������쳣��ֱ�ӷ���,�������һ��sta_flg = 6
//					{
//			      sta_flg = CS1258_END_MEASURE_HR;      // 6 DRDY�������ʲ������
//						return;
//					}
//			    sta_flg = CS1258_SEND_MEASURE_HR_CMD;   // 4 ���������ʲ�������
//					EquMent.STATUS = 0x40; 
//			    mt_status=2; //��ʼ������
					return;
			}
		  /******************У׼����******************************/		
			else if(sta_flg == CS1258_START_CAL_K)  // 8 DRDY���߿�ʼУ׼Kֵ
			{
					 sta_flg = CS1258_END_CAL_K;        // 9 DRDY����У׼Kֵ���
			}
}



uint8_t bh_val=0;
uint8_t cal_err=0;	//У׼�����־

void Get_BR(void)
{
	  if(UsrBIAP.Freq1_All_Res_H==0 && UsrBIAP.Freq1_All_Res_L==0 && sta_flg == CS1258_END_MEASURE_HR) //�����쳣ֱ������clr
		{
		   bh_val = 0;
			 return;
		}
		if(sta_flg == CS1258_END_MEASURE_HR) //�����������ʲ������
		{			
			 bh_val = CAL_HR*Analys.BH/EquMent.HR_CAL_VAL;//ע�� �˴�����ֵ������
		}
		
	  if(sta_flg == CS1258_END_CAL_K)  //����У׼���
	  {
			  cal_err = EquMent.ERR_STATUS; //�������־		
//			k_res[0] = EquMent.FREQ1K_VAL_H;
//			k_res[1] = EquMent.FREQ1K_VAL_L;  
	  }		
	  if(sta_flg == CS1258_END_CAL_HR)  //����У׼���
	  {
			 sta_flg = CS1258_END_CAL_HR+1;
			//Analys.BH = 62;
			if((Analys.BH>=(CAL_HR-10)) && (Analys.BH<=(CAL_HR+10))) //��׼���ʵġ�10�򱣴���flash
				{
					 if( FLASH_PageWrite_halfword(F_HR_CAL_VAL_ADDR,Analys.BH) != 0 ||\
						   ((FLASH_PageRead_halfword(F_HR_CAL_VAL_ADDR)&0xff) !=Analys.BH))
								cal_err = 0xE0; //flash д��ʧ��
					 else
								{cal_err = 0x00;}
				}
			else
				{
				      cal_err = 0xE1; //������Χʧ��
				}
	  }	
}

