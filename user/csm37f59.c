#include "csm37f59.h"
#include <string.h>


uint8_t tim_cnt=0;  //INTERVAL_SEND*10 定时
uint8_t sta_flg=CS1258_IDLE;  //CSM37F58 状态 

uint8_t mes_flg=0;  //电阻测量开关 1.开始测 0.关闭
uint8_t calres_flg=0; //校准电阻开关 1.开校准 0.关闭
uint8_t calhr_flg=0;  //校准心率开关 1.开校准 0.关闭

uint8_t mt_status=0;    //当前马桶测量状态 0 无状态 1 开始测电阻 2 开始测心率 
uint8_t premt_status=0; //上次马桶测量状态

a_res_t a_res={{0},0,0,0};



//blestatus  //上啦输入 EXTI_Trigger_Falling
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
执行条件 1.if((BIA_FLAG>>GetRef)&0x01) 就绪 
         2.sta_flg==0  CSM37F58处于空闲状态
         3.按键按下(可替换成判稳成功)
输出标志 
*/
//sta_flg各状态说明
//#define CS1258_IDLE                       0       // 0 空闲状态
//#define CS1258_SEND_MEASURE_RES_CMD       1       // 1 发送了测量电阻命令
//#define CS1258_START_MEASURE_RES          2       // 2 DRDY拉高开始测量电阻
//#define CS1258_END_MEASURE_RES            3       // 3 DRDY拉低电阻测量完成
//#define CS1258_SEND_MEASURE_HR            4       // 4 发送了心率测量命令
//#define CS1258_START_MEASURE_HR           5       // 5 DRDY拉高开始测量心率
//#define CS1258_END_MEASURE_HR             6       // 6 DRDY拉低心率测量完成

//#define CS1258_SEND_CAL_K                 7       // 7 发送了校准K值命令
//#define CS1258_START_CAL_K                8       // 8 DRDY拉高开始校准K值
//#define CS1258_END_CAL_K                  9       // 9 DRDY拉低校准K值完成

//#define CS1258_SEND_CAL_HR                10      // 10发送了心率校准命令
//#define CS1258_START_CAL_HR               11      // 11DRDY拉高开始校准心率
//#define CS1258_END_CAL_HR                 12      // 12DRDY拉低校准心率完成


extern uint8_t done;
void measure(void)
{
		if((((BIA_FLAG>>GetRef)&0x01) == 1) && sta_flg == CS1258_IDLE) //已经获得完整参考 && 	空闲
		{
			/*********************测量模式****************************/
			 if(mes_flg == 1)			//电阻测量开关
				{  	
            mes_flg=0;done=0;calres_flg=0;calhr_flg=0;
					
					  memset(&a_res,0,sizeof(a_res));
					  cur_result_pkg=0;
						sta_flg = CS1258_SEND_MEASURE_RES_CMD;     // 1 发送了测量电阻命令
            EquMent.STATUS = 0x80; 
						mt_status=1; //开始测电阻

				}
			 /*********************校准模式****************************/
				else if(calres_flg == 1) //校准电阻
				{
						mes_flg=0;done=0;calres_flg=0;calhr_flg=0;
						
						EquMent.FREQ1_CalRes_H = DEF_CALRES1_H;//写校准电阻		
						EquMent.FREQ1_CalRes_L = DEF_CALRES1_L;

						EquMent.CONFIG_K = 0x80;
						sta_flg = CS1258_SEND_CAL_K_CMD;           // 7 发送了校准K值命令
						mt_status=1; //开始校准电阻

				}
				else if(calhr_flg == 1) //校准心率
				{
						mes_flg=0;done=0;calres_flg=0;calhr_flg=0;
						sta_flg = CS1258_SEND_CAL_HR_CMD;          // 10发送了心率校准命令
						EquMent.STATUS = 0x40; 
						mt_status=2; //开始校心率
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
	for(i=0;i<A_RES_SIZE-1;i++) //注意这里i只能到A_RES_SIZE-1 (4)
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
	
	if(EquMent.ERR_STATUS==0) //正常一次
	{
				if(a_res.num <= (A_RES_SIZE-1)) //未满
				{
						a_res.res[a_res.num] = ((UsrBIAP.Freq1_All_Res_H&0xff) <<8) | (UsrBIAP.Freq1_All_Res_L&0xff); //入电阻
						a_res.num++;
					
            goto 	exit_lp1; //再测一次
				}
				else  //满 a_res.num == A_RES_SIZE;
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
									 
									 //计算成分
									 sta_flg = CS1258_SEND_MEASURE_HR_CMD;   // 4 发送了心率测量命令
									 EquMent.STATUS = 0x40; 
									 mt_status=2; //开始测心率
									 return;
								}
								else
								{
								    arr_delete(&a_res);
										goto exit_lp1;//再测一次
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
	else //异常结果直接结束 err_num
	{
		exit_lp0:
			a_res.err_num++;
			if(a_res.err_num > 5)
			{
			      sta_flg = CS1258_END_MEASURE_HR;      // 6 DRDY拉低心率测量完成
						return;
			}
			else //再测一次
			{
		exit_lp1:		
						sta_flg = CS1258_SEND_MEASURE_RES_CMD;     // 1 发送了测量电阻命令
						EquMent.STATUS = 0x80; 
						mt_status=1; //开始测电阻
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
//	    /******************测量电阻和心率******************************/		
//			if(sta_flg == CS1258_START_MEASURE_RES)  // 2 DRDY拉高开始测量电阻
//			{
//				  
//					sta_flg = CS1258_END_MEASURE_RES;    // 3 DRDY拉低电阻测量完成
//			     //EquMent.ERR_STATUST //读错误标志		
//	
//			    if(UsrBIAP.Freq1_All_Res_H==0 && UsrBIAP.Freq1_All_Res_L==0) //如果电阻异常就直接返回,并到最后一步sta_flg = 6
//					{
//			      sta_flg = CS1258_END_MEASURE_HR;      // 6 DRDY拉低心率测量完成
//						return;
//					}
//			    sta_flg = CS1258_SEND_MEASURE_HR_CMD;   // 4 发送了心率测量命令
//					EquMent.STATUS = 0x40; 
//			    mt_status=2; //开始测心率
//					return;
//			}
//		  /******************校准电阻******************************/		
//			else if(sta_flg == CS1258_START_CAL_K)  // 8 DRDY拉高开始校准K值
//			{
//					 sta_flg = CS1258_END_CAL_K;        // 9 DRDY拉低校准K值完成
//			}
//}
void update_sta_flg(void)
{			
	    /******************测量电阻和心率******************************/		
			if(sta_flg == CS1258_START_MEASURE_RES)  // 2 DRDY拉高开始测量电阻
			{
				  
					sta_flg = CS1258_END_MEASURE_RES;    // 3 DRDY拉低电阻测量完成	
	
				   queue_a_res();
//			    if(UsrBIAP.Freq1_All_Res_H==0 && UsrBIAP.Freq1_All_Res_L==0) //如果电阻异常就直接返回,并到最后一步sta_flg = 6
//					{
//			      sta_flg = CS1258_END_MEASURE_HR;      // 6 DRDY拉低心率测量完成
//						return;
//					}
//			    sta_flg = CS1258_SEND_MEASURE_HR_CMD;   // 4 发送了心率测量命令
//					EquMent.STATUS = 0x40; 
//			    mt_status=2; //开始测心率
					return;
			}
		  /******************校准电阻******************************/		
			else if(sta_flg == CS1258_START_CAL_K)  // 8 DRDY拉高开始校准K值
			{
					 sta_flg = CS1258_END_CAL_K;        // 9 DRDY拉低校准K值完成
			}
}



uint8_t bh_val=0;
uint8_t cal_err=0;	//校准错误标志

void Get_BR(void)
{
	  if(UsrBIAP.Freq1_All_Res_H==0 && UsrBIAP.Freq1_All_Res_L==0 && sta_flg == CS1258_END_MEASURE_HR) //电阻异常直接心率clr
		{
		   bh_val = 0;
			 return;
		}
		if(sta_flg == CS1258_END_MEASURE_HR) //电阻正常心率测量完成
		{			
			 bh_val = CAL_HR*Analys.BH/EquMent.HR_CAL_VAL;//注意 此处心率值处理发送
		}
		
	  if(sta_flg == CS1258_END_CAL_K)  //电阻校准完成
	  {
			  cal_err = EquMent.ERR_STATUS; //读错误标志		
//			k_res[0] = EquMent.FREQ1K_VAL_H;
//			k_res[1] = EquMent.FREQ1K_VAL_L;  
	  }		
	  if(sta_flg == CS1258_END_CAL_HR)  //心率校准完成
	  {
			 sta_flg = CS1258_END_CAL_HR+1;
			//Analys.BH = 62;
			if((Analys.BH>=(CAL_HR-10)) && (Analys.BH<=(CAL_HR+10))) //标准心率的±10则保存至flash
				{
					 if( FLASH_PageWrite_halfword(F_HR_CAL_VAL_ADDR,Analys.BH) != 0 ||\
						   ((FLASH_PageRead_halfword(F_HR_CAL_VAL_ADDR)&0xff) !=Analys.BH))
								cal_err = 0xE0; //flash 写入失败
					 else
								{cal_err = 0x00;}
				}
			else
				{
				      cal_err = 0xE1; //超过范围失败
				}
	  }	
}

