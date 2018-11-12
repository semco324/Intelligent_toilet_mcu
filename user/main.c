/**
  ******************************************************************************
  * @file    ADC/ADC_AnalogWatchdog/main.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "lib_delay.h"
#include "usart.h"
#include "lib_myiic.h"
#include "timer.h"
#include "driver_csm92p10.h"
#include "driver_cs1258.h"
#include "sleep.h"
#include "wdt.h"
#include "ble_parse.h"	
#include "eb_parse.h"	
#include "flash.h"
#include "analysis.h"
#include "csm37f59.h"
#include <string.h>
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{	
    delay_init();	  	 																//延时初始化
	
	  no_use_peripherals();                             //关闭不用外设
	
	  lib_init_queue(&btrx_fifo.fifo,btrx_arr,BTRXSIZE);//接收蓝牙串口队列初始化
	
	  lib_init_queue(&ebrx_fifo.fifo,ebrx_arr,EBRXSIZE);//接收电控板串口队列初始化
	
                              
	  uart_init(BLE_USED_UART,115200);									 //蓝牙uart 初始化  PA6--rx  PA7--tx  
                            
	  uart_init(EB_USED_UART,115200);										//电控板 uart 初始化
	  //usart_rx_mode(EB_USED_UART,DISABLE);                //失能电控串口1 tx
		Reset_CS125x();																		//cs1258初始化
	
		InitCS125x(); 
	
	  User_FMC_Init();                                  //flash 存储初始化
	
	  Clr_All_Block_Ram();                              //清楚数据区
	
	  load_fmc_data();                                  //加载默认数据
	
	  //SetDRDY(0);  
		sta_flg = CS1258_IDLE;                             //0 CS1258 空闲状态
		
		tab_ref_test();
	
		delay_ms(100);

	  User_WDT_Init();																	 //看门狗初始化

    timer3_init();     			                           //定时查询CSM37F58 初始化
	
// 		timer2_init();                                     //CS1258 drdy超时定时器初始化
	
//  IIC_Init();             													 //iic初始化
	
		 
		static uint8_t m=1,n=1,k=1; //m,阻抗分析初始化 n校准初始化 k,心率初始化	

	  while(1)
		{
			/******************获得参考**********************/
			 GetRef01ADC(f_cnt);  
			

			/*************查询BLE对列下发的命令****************/
			measure();
			
			
			/******************测量模式**********************/
			 if(((EquMent.STATUS>>ON_OFF)&0x01))  //允许测量电阻
			 {
						EquMent.CONFIG_K&=~(1<<CK);//测量过程中是禁止使能校准
						EquMent.STATUS&=~(1<<H_BR);//测量过程中是禁止使能心率
						 
					 if(m) 
						{
							    m=0;
									 //清除设备配置区
									EquMent.Z_STATUS&=0xC0;//clr Z12,34,14,13,23,24
									EquMent.ERR_STATUS=0;
									EquMent.C_STATUS=0;
									EquMent.F_STATUS=0;  //频率状态
									Clr_UserBIAPart_Ram();//用户测试BIA
									Clr_Analysis_Ram();   //成分计算结果
									Clr_ADCVal_Ram();     //测量过程数据
									Clr_RESVal_Ram();     //真实电阻
									Clr_UsrInfo_ResF1();
									BIA_FLAG&=~(1<<GetBodyDone); //
						}	
					 //SetDRDY(1); 
					 if(sta_flg == CS1258_SEND_MEASURE_RES_CMD)      // 1 发送了测量电阻命令
											sta_flg = CS1258_START_MEASURE_RES;  // 2 DRDY拉高开始测量电阻
					_CS1258bug();							
				 GetBodyRes();					
			 }
			 else{m=1;}	 
		
			/******************工程模式**********************/
			 if(((EquMent.CONFIG_K>>CK)&0x01))//工厂校准模式
			 {
						 
						EquMent.STATUS&=~(1<<ON_OFF);//校准过程中禁止使能测量
						EquMent.STATUS&=~(1<<H_BR);  //校准过程中禁止使能心率
				 
						if(n)
							{
								   n=0;		
									 EquMent.Z_STATUS&=0xC0;//clr Z12,34,14,13,23,24
									 EquMent.ERR_STATUS=0;
									 EquMent.F_STATUS=0;  //频率状态
									 EquMent.F1CV_FLAG=0;					 
									 Clr_UserBIAPart_Ram();//用户测试BIA					     
									 Clr_ADCVal_Ram();     //测量过程数据
									 Clr_RESVal_Ram();     //真实电阻
									 BIA_FLAG&=~(1<<CalKDone); //
									 Parm_Rational();          //检查参数合理性
						   }
						if(!((EquMent.ERR_STATUS>>Err_Parm)&0x01))
						{			
									//SetDRDY(1);	
									if(sta_flg == CS1258_SEND_CAL_K_CMD)           // 7 发送了校准K值命令
														sta_flg = CS1258_START_CAL_K;        // 8 DRDY拉高开始校准K值
									_CS1258bug();						
									Calibrate_K();
						}
			 }
			 else{n=1;}

			/******************心率模式**********************/
			 if(((EquMent.STATUS>>H_BR)&0x01))  //允许测量心率
			 {
					 EquMent.CONFIG_K&=~(1<<CK);   //clr 校准
					 EquMent.STATUS&=~(1<<ON_OFF); //clr 测脂
				 
					 if(k)
			      {
								 k=0;
								 Analys.BH=0;  
								 CS125X_XLCL_Initial_Per();//25k,80hz,14		
								 CS125X_XLCL_Initial();		 
								 delay_ms(5);		
								 //SetDRDY(1);
								 if(sta_flg == CS1258_SEND_MEASURE_HR_CMD)     // 4 发送了心率测量命令
													sta_flg = CS1258_START_MEASURE_HR;   // 5 DRDY拉高开始测量心率
								 else if(sta_flg == CS1258_SEND_CAL_HR_CMD)    // 10发送了心率校准命令
													sta_flg = CS1258_START_CAL_HR;       // 11DRDY拉高开始校准心率
						}
						 Analys.BH = HRI();
								
						if(Analys.BH !=0)	//得到结果
						{					 				 
								 EquMent.STATUS&=~(1<<H_BR);
								 SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
								 InitCS125x();   //恢复1258		
								 //SetDRDY(0);
								 if(sta_flg == CS1258_START_MEASURE_HR)      // 5 DRDY拉高开始测量心率
													sta_flg = CS1258_END_MEASURE_HR;   // 6 DRDY拉低心率测量完成
								 else if(sta_flg == CS1258_START_CAL_HR)     // 11DRDY拉高开始校准心率
													sta_flg = CS1258_END_CAL_HR;       // 12DRDY拉低校准心率完成
						}										
			 }
			 else{k=1;}
			
       Get_BR();	         //得到心率与存储心率
			 
       ble_dealwith_queue();//ble解析队列		

			 eb_dealwith_queue(); //eb 解析队列	
			 
       McuToBle_Proc(); //蓝牙处理

       check_sleep();   //睡眠处理			 
		}
		
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
