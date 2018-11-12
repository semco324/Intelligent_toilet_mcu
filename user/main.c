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
    delay_init();	  	 																//��ʱ��ʼ��
	
	  no_use_peripherals();                             //�رղ�������
	
	  lib_init_queue(&btrx_fifo.fifo,btrx_arr,BTRXSIZE);//�����������ڶ��г�ʼ��
	
	  lib_init_queue(&ebrx_fifo.fifo,ebrx_arr,EBRXSIZE);//���յ�ذ崮�ڶ��г�ʼ��
	
                              
	  uart_init(BLE_USED_UART,115200);									 //����uart ��ʼ��  PA6--rx  PA7--tx  
                            
	  uart_init(EB_USED_UART,115200);										//��ذ� uart ��ʼ��
	  //usart_rx_mode(EB_USED_UART,DISABLE);                //ʧ�ܵ�ش���1 tx
		Reset_CS125x();																		//cs1258��ʼ��
	
		InitCS125x(); 
	
	  User_FMC_Init();                                  //flash �洢��ʼ��
	
	  Clr_All_Block_Ram();                              //���������
	
	  load_fmc_data();                                  //����Ĭ������
	
	  //SetDRDY(0);  
		sta_flg = CS1258_IDLE;                             //0 CS1258 ����״̬
		
		tab_ref_test();
	
		delay_ms(100);

	  User_WDT_Init();																	 //���Ź���ʼ��

    timer3_init();     			                           //��ʱ��ѯCSM37F58 ��ʼ��
	
// 		timer2_init();                                     //CS1258 drdy��ʱ��ʱ����ʼ��
	
//  IIC_Init();             													 //iic��ʼ��
	
		 
		static uint8_t m=1,n=1,k=1; //m,�迹������ʼ�� nУ׼��ʼ�� k,���ʳ�ʼ��	

	  while(1)
		{
			/******************��òο�**********************/
			 GetRef01ADC(f_cnt);  
			

			/*************��ѯBLE�����·�������****************/
			measure();
			
			
			/******************����ģʽ**********************/
			 if(((EquMent.STATUS>>ON_OFF)&0x01))  //�����������
			 {
						EquMent.CONFIG_K&=~(1<<CK);//�����������ǽ�ֹʹ��У׼
						EquMent.STATUS&=~(1<<H_BR);//�����������ǽ�ֹʹ������
						 
					 if(m) 
						{
							    m=0;
									 //����豸������
									EquMent.Z_STATUS&=0xC0;//clr Z12,34,14,13,23,24
									EquMent.ERR_STATUS=0;
									EquMent.C_STATUS=0;
									EquMent.F_STATUS=0;  //Ƶ��״̬
									Clr_UserBIAPart_Ram();//�û�����BIA
									Clr_Analysis_Ram();   //�ɷּ�����
									Clr_ADCVal_Ram();     //������������
									Clr_RESVal_Ram();     //��ʵ����
									Clr_UsrInfo_ResF1();
									BIA_FLAG&=~(1<<GetBodyDone); //
						}	
					 //SetDRDY(1); 
					 if(sta_flg == CS1258_SEND_MEASURE_RES_CMD)      // 1 �����˲�����������
											sta_flg = CS1258_START_MEASURE_RES;  // 2 DRDY���߿�ʼ��������
					_CS1258bug();							
				 GetBodyRes();					
			 }
			 else{m=1;}	 
		
			/******************����ģʽ**********************/
			 if(((EquMent.CONFIG_K>>CK)&0x01))//����У׼ģʽ
			 {
						 
						EquMent.STATUS&=~(1<<ON_OFF);//У׼�����н�ֹʹ�ܲ���
						EquMent.STATUS&=~(1<<H_BR);  //У׼�����н�ֹʹ������
				 
						if(n)
							{
								   n=0;		
									 EquMent.Z_STATUS&=0xC0;//clr Z12,34,14,13,23,24
									 EquMent.ERR_STATUS=0;
									 EquMent.F_STATUS=0;  //Ƶ��״̬
									 EquMent.F1CV_FLAG=0;					 
									 Clr_UserBIAPart_Ram();//�û�����BIA					     
									 Clr_ADCVal_Ram();     //������������
									 Clr_RESVal_Ram();     //��ʵ����
									 BIA_FLAG&=~(1<<CalKDone); //
									 Parm_Rational();          //������������
						   }
						if(!((EquMent.ERR_STATUS>>Err_Parm)&0x01))
						{			
									//SetDRDY(1);	
									if(sta_flg == CS1258_SEND_CAL_K_CMD)           // 7 ������У׼Kֵ����
														sta_flg = CS1258_START_CAL_K;        // 8 DRDY���߿�ʼУ׼Kֵ
									_CS1258bug();						
									Calibrate_K();
						}
			 }
			 else{n=1;}

			/******************����ģʽ**********************/
			 if(((EquMent.STATUS>>H_BR)&0x01))  //�����������
			 {
					 EquMent.CONFIG_K&=~(1<<CK);   //clr У׼
					 EquMent.STATUS&=~(1<<ON_OFF); //clr ��֬
				 
					 if(k)
			      {
								 k=0;
								 Analys.BH=0;  
								 CS125X_XLCL_Initial_Per();//25k,80hz,14		
								 CS125X_XLCL_Initial();		 
								 delay_ms(5);		
								 //SetDRDY(1);
								 if(sta_flg == CS1258_SEND_MEASURE_HR_CMD)     // 4 ���������ʲ�������
													sta_flg = CS1258_START_MEASURE_HR;   // 5 DRDY���߿�ʼ��������
								 else if(sta_flg == CS1258_SEND_CAL_HR_CMD)    // 10����������У׼����
													sta_flg = CS1258_START_CAL_HR;       // 11DRDY���߿�ʼУ׼����
						}
						 Analys.BH = HRI();
								
						if(Analys.BH !=0)	//�õ����
						{					 				 
								 EquMent.STATUS&=~(1<<H_BR);
								 SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS д0xCE �ر�BIM		
								 InitCS125x();   //�ָ�1258		
								 //SetDRDY(0);
								 if(sta_flg == CS1258_START_MEASURE_HR)      // 5 DRDY���߿�ʼ��������
													sta_flg = CS1258_END_MEASURE_HR;   // 6 DRDY�������ʲ������
								 else if(sta_flg == CS1258_START_CAL_HR)     // 11DRDY���߿�ʼУ׼����
													sta_flg = CS1258_END_CAL_HR;       // 12DRDY����У׼�������
						}										
			 }
			 else{k=1;}
			
       Get_BR();	         //�õ�������洢����
			 
       ble_dealwith_queue();//ble��������		

			 eb_dealwith_queue(); //eb ��������	
			 
       McuToBle_Proc(); //��������

       check_sleep();   //˯�ߴ���			 
		}
		
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
