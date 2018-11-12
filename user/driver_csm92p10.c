#include "driver_csm92p10.h"

const uint8_t check_bt_cmd[4]={0x01,0x00,0xFC,0x00}; //检查蓝牙处于什么状态
const uint8_t reset_bt_cmd[4]={0xC5,0x01,0x2C,0xE8}; //蓝牙复位命令
const uint8_t adv_bt_cmd[5]={0xC5,0x02,0x26,0x01,0xE0}; //打开蓝牙广播命令
const uint8_t sleep_bt_cmd[5]={0xC5,0x02,0x20,0x00,0xE7}; //打开蓝牙睡眠命令
uint8_t TxBleData[30]={0};
uint8_t blestatus=0;
uint8_t done=0;
uint8_t app_mes=0;
uint8_t app_calres=0;
uint8_t app_calhr=0;
uint8_t ack_flg=0;        
uint8_t result_ack_flg=0; //结果应答
uint8_t all_result_pkg=0; //总包数
uint8_t cur_result_pkg=0; //当前包数


/*输出,断上拉,低 延时7ms*/
void Ble_UartEnable() 
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = BLEWakeup_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BLEWakeup_GPIO, &GPIO_InitStructure);	
  
  GPIO_WriteBit(BLEWakeup_GPIO,BLEWakeup_Pin,(BitAction)0);
	
	delay_ms(7);
}	

														
/*延时1ms,输出,短上拉,高*/
void Ble_UartDisable()
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
  delay_ms(1);
	
  GPIO_InitStructure.GPIO_Pin = BLEWakeup_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BLEWakeup_GPIO, &GPIO_InitStructure);	
  
  GPIO_WriteBit(BLEWakeup_GPIO,BLEWakeup_Pin,(BitAction)1);
}	

														
														
														
//1.初始化蓝牙
//2.启动广播
//3.设置广播内容,检查蓝牙状态
//4.(蓝牙连接上)发生数据

//头+长度+命令+数据+校验
//   长度=命令+数据
//                  校验=头^长度^命令^数据
void SendDateToAPP(uint8_t *arr)
{
  uint8_t i=0;
	uint8_t tmpcheck=0;
	for(i=0;i<(arr[1]+2);i++)
	{
		 USART_SendData(USART1, arr[i]);
		 tmpcheck=tmpcheck^arr[i];	
		 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
	 USART_SendData(USART1, tmpcheck);
	 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

//从arr+n地址开始xor
uint8_t cal_xor(uint8_t *arr,uint8_t n)
{
   uint8_t i=0;uint8_t tmpcheck=0;
	 for(i=n;i<(arr[n+1]+2+n);i++)
	 {
	   tmpcheck=tmpcheck^arr[i];	
	 }
	 return tmpcheck;
}



void SendCmdToBle(uint8_t *arr,uint8_t n)
{
	  uint8_t i;
		Ble_UartEnable();
	
	  for(i=0;i<n;i++)
	  {
	     USART_SendData(USART1, arr[i]);
	     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	  }
		Ble_UartDisable();
}
/*
	byte			description
  0-1       FFF0(芯海标识)
    2       广播版本号 （当前版本02）
    3       消息体属性
  4-5       体重
  6-9       产品ID
  10-11     蓝牙版本
  12-13     秤算法版本
  14-19     MAC地址
c5 0f 24 ff f0 03 00 00 00 01 02 02 02 01 02 01 02 E1
*/
void Ble_SetAdvData(void)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=15;
	  TxBleData[2]=SETADVCMD;
	  TxBleData[3]=0xff;
    TxBleData[4]=0xf0;
    TxBleData[5]=0x03;
    TxBleData[6]=0;        //消息体属性           
    TxBleData[7]=0;        //APP_WeightL               
    TxBleData[8]=0;        //APP_WeightH
    TxBleData[9]=0x01;	   //产品ID
    TxBleData[10]=0x02;
    TxBleData[11]=0x02;
    TxBleData[12]=0x02;
    TxBleData[13]=0x01;   //蓝牙版本
    TxBleData[14]=0x02;
    TxBleData[15]=0x01;  //秤算法版本
    TxBleData[16]=0x02;	
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}
/*
void Ble_SendAppResult(void)
{
    Ble_UartEnable();
	
	  TxBleData[0]=0xC5;
	  TxBleData[1]=12;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
    TxBleData[5]=0xCC;       //数据头
    TxBleData[6]=6;          //长度       
    TxBleData[7]=0x65;       //指令   上传结果指令
    TxBleData[8]=0;          //体重H
    TxBleData[9]=0;	         //体重L
    TxBleData[10]=UsrBIAP.Freq1_All_Res_H;//电阻H	
    TxBleData[11]=UsrBIAP.Freq1_All_Res_L;//电阻L
    TxBleData[12]=bh_val;    //心率	 
    TxBleData[13]=cal_xor(TxBleData,5);//校验值 
	
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}*/

void Ble_SendAppResult(void)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=20-3+6;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
    TxBleData[5]=0xCC;       //数据头
    TxBleData[6]=20-3;       //长度       
    TxBleData[7]=0x57;       //指令   上传结果指令
	
    TxBleData[8]=all_result_pkg;  //总包数
	  TxBleData[9]=cur_result_pkg;	//当前包数
		if(cur_result_pkg == 0)
	  {
			TxBleData[10]=UsrInfo.WTH;//体重H
			TxBleData[11]=UsrInfo.WTL;//体重L
      TxBleData[12]=UsrBIAP.Freq1_All_Res_H;          //电阻H	 
	    TxBleData[13]=UsrBIAP.Freq1_All_Res_L;          //电阻L   
	    TxBleData[14]=bh_val;               //心率	
			memcpy(&TxBleData[15],&ble_rtc,sizeof(ble_rtc)); //RTC
			TxBleData[21]=0;
			TxBleData[22]=0;
			TxBleData[23]=0;	
		}
		else if(cur_result_pkg >0 && cur_result_pkg < all_result_pkg)
		{
			char tmp = ((sizeof(Analys) - (cur_result_pkg-1)*VAILE_PKG_RESULT_LEN ) >= VAILE_PKG_RESULT_LEN ) ? VAILE_PKG_RESULT_LEN:(sizeof(Analys) - (cur_result_pkg-1)*VAILE_PKG_RESULT_LEN );
				 
			memcpy(&TxBleData[10],(uint8_t*)&Analys+(cur_result_pkg-1)*VAILE_PKG_RESULT_LEN,tmp); //10~23 
			
			if(tmp < VAILE_PKG_RESULT_LEN)
			{
				    memset(&TxBleData[10+tmp],0,VAILE_PKG_RESULT_LEN-tmp);
			}
		}
		
	  TxBleData[24]=cal_xor(TxBleData,5);//校验值
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}

void passthrough_to_ble(uint8_t * src)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=*(src+1)+6;
	  //TxBleData[1]=20-3+6;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
	  memcpy(&TxBleData[5],src,*(src+1)+3);//passthrough data
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}


void Ble_SendAppCalResResult(void)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=10;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
    TxBleData[5]=0xCC;       //数据头
    TxBleData[6]=4;          //长度       
    TxBleData[7]=0xAC;       //指令  
    TxBleData[8]=EquMent.FREQ1K_VAL_H;   //校准KH
    TxBleData[9]=EquMent.FREQ1K_VAL_L;   //校准KL
    TxBleData[10]=cal_err;   //错误标志 
    TxBleData[13]=cal_xor(TxBleData,5);//校验值 
	
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}

void Ble_SendAppCalHRResult(void)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=10;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
    TxBleData[5]=0xCC;       //数据头
    TxBleData[6]=4;          //长度       
    TxBleData[7]=0xBC;       //指令   上传校准结果指令
    TxBleData[8]=0;      //
    TxBleData[9]=Analys.BH;   //校准k_hr
    TxBleData[10]=cal_err;   //错误标志 
    TxBleData[13]=cal_xor(TxBleData,5);//校验值 
	
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}

#define B_WF    0
#define B_OTA   1
#define B_RES_S 2
#define B_HR_S  3

void Ble_SendAppStatus(void)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=4+6;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
    TxBleData[5]=0xCC;       //数据头
    TxBleData[6]=4;          //长度       
    TxBleData[7]=0x56;       //指令   上传状态指令
	
	 //**************STATUS1**********************
	  TxBleData[8]=0;
	  //wf from
	  
	if(EquInfo.SW_VER_H >0 && EquInfo.SW_VER_L>0) //ota
	   {
			 TxBleData[8]|=(1<<B_OTA);
		 }
	  
	if(mt_status == 0)           //RES_S  B_HR_S
	   {
	    TxBleData[8]&=~(1<<B_RES_S);
			TxBleData[8]&=~(1<<B_HR_S);
	   }
	else if(mt_status == 1)
	   {
	    TxBleData[8]|=(1<<B_RES_S);
			TxBleData[8]&=~(1<<B_HR_S);
	   }
	else if(mt_status == 2)
	   {
	    TxBleData[8]&=~(1<<B_RES_S);
			TxBleData[8]|=(1<<B_HR_S);
	   }		                             
	
	 //**************STATUS2**********************
	  TxBleData[9]=0;   
		 
	 //**************STATUS3**********************
	  TxBleData[10]=0;  
		 
    TxBleData[11]=cal_xor(TxBleData,5);//校验值 
	
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}

//回应开始测量ACK
void Ble_SendAppAck(void)
{
    Ble_UartEnable();
	  TxBleData[0]=0xC5;
	  TxBleData[1]=0x05+6;
	  TxBleData[2]=SENDAPPDATACMD;
		TxBleData[3]=HANDLE_2A9C_L;
    TxBleData[4]=HANDLE_2A9C_H;
	//---------------------------------------------------------
    TxBleData[5]=0xCC;       //数据头
    TxBleData[6]=0x05;       //长度       
    TxBleData[7]=0x55;       //指令   马桶回应app ack
		TxBleData[8] =EquInfo.SW_VER_H;
		TxBleData[9] =EquInfo.SW_VER_L; //软件版本号
		TxBleData[10]=EquInfo.HW_VER_H;
		TxBleData[11]=EquInfo.HW_VER_L; //硬件版本号
    TxBleData[12]=cal_xor(TxBleData,5);//校验值 
	
    SendDateToAPP(TxBleData);
    Ble_UartDisable();
}

void Ble_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);//open GPIO clock
	
  GPIO_InitStructure.GPIO_Pin = BLEWakeup_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BLEWakeup_GPIO, &GPIO_InitStructure);	
	
  GPIO_InitStructure.GPIO_Pin = BLEStatus_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BLEStatus_GPIO, &GPIO_InitStructure);	
	
	blestatus=0;
	Ble_UartEnable();
}


uint8_t x=0,x1=0;
void McuToBle_Proc(void)
{
		switch(blestatus)
		{
			case BLESTATUS_REST: Ble_Init();
													 blestatus=BLESTATUS_STANDY;break;
			
			case BLESTATUS_STANDY: SendCmdToBle((uint8_t *)adv_bt_cmd,(sizeof(adv_bt_cmd)/sizeof(adv_bt_cmd[0])));//启动广播
														blestatus=BLESTATUS_ADV;break;
			
			case BLESTATUS_ADV:  Ble_SetAdvData(); //设置广播内容 
			                    if(READ_STATUS()==0)
																blestatus=BLESTATUS_CONNECT;//检测蓝牙状态 
													break;
			
			case BLESTATUS_CONNECT: 
				  									 if(app_mes==1 && tim_cnt>=INTERVAL_SEND && done==0) 
														 {
																app_mes=0;tim_cnt=0;
																mes_flg=1;
																premt_status=mt_status=0;sta_flg=CS1258_IDLE;
																Ble_SendAppAck(); //回应app开始测量命令
																break;		
														 }	
				  									 if(app_calres==1 && tim_cnt>=INTERVAL_SEND && done==0) 
														 {
																app_calres=0;tim_cnt=0;
																calres_flg=1;
																premt_status=mt_status=0;sta_flg=CS1258_IDLE;
																Ble_SendAppAck(); //回应app开始校准电阻命令
																break;		
														 }
				  									 if(app_calhr==1 && tim_cnt>=INTERVAL_SEND && done==0) 
														 {
																app_calhr=0;tim_cnt=0;
																calhr_flg=1;
																premt_status=mt_status=0;sta_flg=CS1258_IDLE;
																Ble_SendAppAck(); //回应app开始校准心率命令
																break;		
														 }			
														 
														 if(ack_flg==0 && tim_cnt>=INTERVAL_SEND && done==0 && sta_flg!=CS1258_END_MEASURE_HR)
														 {
//														     tim_cnt=0;
//															   if(sta_flg==CS1258_END_MEASURE_HR) 
//																	  {Ble_SendAppResult();x1++;}
//																 else if(sta_flg==CS1258_END_CAL_K)
//																	  {Ble_SendAppCalResResult();}
//																 else if(sta_flg==(CS1258_END_CAL_HR+1))
//																	  {Ble_SendAppCalHRResult();}
//															   else if(premt_status!=mt_status) 
//																	  {Ble_SendAppStatus();x++;}
														     tim_cnt=0;
																 if(sta_flg==CS1258_END_CAL_K)
																	  {Ble_SendAppCalResResult();}
																 else if(sta_flg==(CS1258_END_CAL_HR+1))
																	  {Ble_SendAppCalHRResult();}
															   else if(premt_status!=mt_status) 
																	  {Ble_SendAppStatus();x++;}
														 }
														 					 
														 if(ack_flg==1)
														 {
//														     ack_flg=0;
//															   if(sta_flg==CS1258_END_MEASURE_HR || sta_flg==CS1258_END_CAL_K || sta_flg==(CS1258_END_CAL_HR+1))  
//																			{
//																					done=1;premt_status=mt_status=0;sta_flg=CS1258_IDLE;
//																			}
//															   else 
//																	premt_status=mt_status;
														     ack_flg=0;
															   if( sta_flg==CS1258_END_CAL_K || sta_flg==(CS1258_END_CAL_HR+1))  
																			{
																					done=1;premt_status=mt_status=0;sta_flg=CS1258_IDLE;
																			}
															   else 
																	premt_status=mt_status;
														 }		

														 if(result_ack_flg==0 && tim_cnt>=INTERVAL_SEND && done==0 )
														 {
														     tim_cnt=0;
															   if(sta_flg==CS1258_END_MEASURE_HR) 
																	  {Ble_SendAppResult();x1++;}													 
														 } 
														 
														 if(result_ack_flg==1)
														 {
														    result_ack_flg=0;
															  if(sta_flg==CS1258_END_MEASURE_HR && cur_result_pkg == (all_result_pkg-1)) //最后一包
																 {
																	 done=1;premt_status=mt_status=0;sta_flg=CS1258_IDLE;cur_result_pkg=0;
																 }
																 else //下一包
																 {
																     cur_result_pkg++;
																	  if(cur_result_pkg == all_result_pkg) 
																		 cur_result_pkg = all_result_pkg-1;
																 }
														 }
														 
														 
			                       if(READ_STATUS()==1)
															     blestatus=BLESTATUS_ADV;//检测蓝牙状态 
			                       break;
			
			//case BLESTATUS_SLEEP: //if wakeup  退出睡眠,唤醒蓝牙 else break
			default: break; //if sleep break;//case BLESTATUS_SLEEP:
		}
}




/*
if 在连上蓝牙后 心率已经测完，就把结果直接发给APP
else 发送当前状态给APP

无ACK就重发
*/


