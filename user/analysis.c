
#include "analysis.h"

//单频50k算法
//1.手部---普通
//2.手部---运动
//3.脚部---普通
//4.脚部---运动
//5.全身---普通
//6.全身---运动
//7.心率算法


		//输入参数
	 uint8_t B_SP_Mode; //模式
	 uint8_t B_SEX; //性别 1 man 0 weman
	 uint8_t B_Age; //年龄
	 uint16_t  B_H;   //身高 cm
	 uint16_t  B_Wt;  //体重 kg   
	 uint16_t  B_Z_12;
	 uint16_t  B_Z_13;
	 uint16_t  B_Z_14;
	 uint16_t  B_Z_23;
	 uint16_t  B_Z_24;
	 uint16_t  B_Z_34;

uint8_t analysis_parm(void)
{	
	if((addr_flag>>b_analysis)&0x01)  //if 1
	{
			addr_flag&=~(1<<b_analysis);
			
		//1.分别获得身高、体重、年龄、性别、阻抗输入参数
		  B_SEX=(UsrInfo.SAGE&0x80)>>7; 
			B_Age=UsrInfo.SAGE&0x7F;
			B_H=(uint16_t)UsrInfo.HT; 
			B_Wt=(uint16_t)get2byte(UsrInfo.WTH,UsrInfo.WTL,L); //kg 		
      B_SP_Mode=0x00;
		
//      B_Z_12=get2byte(RESVal->F1_Z12_RES_H,RESVal->F1_Z12_RES_L,L)/10.0;
//		  B_Z_13=get2byte(RESVal->F1_Z13_RES_H,RESVal->F1_Z13_RES_L,L)/10.0;
//		  B_Z_14=get2byte(RESVal->F1_Z14_RES_H,RESVal->F1_Z14_RES_L,L)/10.0;
//		  B_Z_23=get2byte(RESVal->F1_Z23_RES_H,RESVal->F1_Z23_RES_L,L)/10.0;
//		  B_Z_34=get2byte(RESVal->F1_Z34_RES_H,RESVal->F1_Z34_RES_L,L)/10.0;
//		  B_Z_24=get2byte(RESVal->F1_Z24_RES_H,RESVal->F1_Z24_RES_L,L)/10.0;
      B_Z_12=get2byte(UsrBIAP.Freq1_Z12_Res_H,UsrBIAP.Freq1_Z12_Res_L,L)/10.0;
		  B_Z_13=get2byte(UsrBIAP.Freq1_Z13_Res_H,UsrBIAP.Freq1_Z13_Res_L,L)/10.0;
		  B_Z_14=get2byte(UsrBIAP.Freq1_Z14_Res_H,UsrBIAP.Freq1_Z14_Res_L,L)/10.0;
		  B_Z_23=get2byte(UsrBIAP.Freq1_Z23_Res_H,UsrBIAP.Freq1_Z23_Res_L,L)/10.0;
		  B_Z_34=get2byte(UsrBIAP.Freq1_Z34_Res_H,UsrBIAP.Freq1_Z34_Res_L,L)/10.0;
		  B_Z_24=get2byte(UsrBIAP.Freq1_Z24_Res_H,UsrBIAP.Freq1_Z24_Res_L,L)/10.0;
			
		//2.输入参数范围判断 身高 体重 年龄 阻抗 频率(50k)    
	  if(B_Wt>WEIGHT_MAX || B_Wt<WEIGHT_MIN ||\
			 B_H>HT_MAX || B_H<HT_MIN ||\
		   B_Age>AGE_MAX || B_Age<AGE_MIN ||\
		   B_Z_34>(RES_MAX/10) || B_Z_34<(RES_MIN/10) ||\
			 B_Z_24>(RES_MAX/10) || B_Z_24<(RES_MIN/10) ||\
		   B_Z_23>(RES_MAX/10) || B_Z_23<(RES_MIN/10) ||\
		   B_Z_14>(RES_MAX/10) || B_Z_14<(RES_MIN/10) ||\
		   B_Z_13>(RES_MAX/10) || B_Z_13<(RES_MIN/10) ||\
		   B_Z_12>(RES_MAX/10) || B_Z_12<(RES_MIN12/10) ||\
		   freq[0]!=__50KHz
			) //W H Age Z f 范围	   		   		
		  {
				EquMent.ERR_STATUS|=(1<<Err_Parm);//输入参数非法
				EquMent.C_STATUS|=(1<<C_Ok);   //执行失败
				EquMent.C_STATUS&=~(1<<C_Exe); //完成
				return FALSE;
			}
			
		//3.算法选择
		if(test_sta==HAND_1)
		{
			 //普通
			 //运动员	
			return TRUE;
		}
		else if(test_sta==LEG_1)
		{
			 //普通
			 //运动员	
			return TRUE;		
		}
		else if(test_sta==BODY_1)
		{
			   //C_BIA_C432(B_SP_Mode,B_SEX,B_Age,B_H,B_Wt,B_Z_12,B_Z_13,B_Z_14,B_Z_23,B_Z_24,B_Z_34); //全身普通
         if( C_BIAS_V433(B_SEX,B_Age,B_H,B_Wt,B_Z_12,B_Z_13,B_Z_14,B_Z_23,B_Z_24,B_Z_34) >0 )     //; //全身普通
				 {
						EquMent.ERR_STATUS|=(1<<Err_Parm);//输入参数非法
						EquMent.C_STATUS|=(1<<C_Ok);   //执行失败
						EquMent.C_STATUS&=~(1<<C_Exe); //完成
						return FALSE;				 
				 }
				 
			   EquMent.C_STATUS&=~((1<<C_Exe)|(1<<C_Ok));//完成+执行成功
				 //输出参数 
			
			   //成分组成 6
				 Analys.BFP_H = get1byte((uint32_t)(B_BFP*10),HL);
				 Analys.BFP_L = get1byte((uint32_t)(B_BFP*10),LL);     //体脂率*10  

				 Analys.SLM_H = get1byte((uint32_t)(B_SLM*10),HL);  
				 Analys.SLM_L = get1byte((uint32_t)(B_SLM*10),LL);  	//肌肉重 *

				 Analys.BMC_H = get1byte((uint32_t)(B_BMC*10),HL);  
				 Analys.BMC_L = get1byte((uint32_t)(B_BMC*10),LL);	  //骨盐量 *		

				 Analys.BWP_H = get1byte((uint32_t)(B_BWP*10),HL);  
				 Analys.BWP_L = get1byte((uint32_t)(B_BWP*10),LL);  	//体水分率
				 
				 Analys.PP_H = get1byte((uint32_t)(B_PP*10),HL);  
				 Analys.PP_L = get1byte((uint32_t)(B_PP*10),LL);		 //蛋白质率 蛋白质重用户自行计算 蛋白质 = 体重 * B_PP

				 Analys.SMM_H = get1byte((uint32_t)(B_SMM*10),HL);  
				 Analys.SMM_L = get1byte((uint32_t)(B_SMM*10),LL);      //骨骼肌重

				 //Analys->TF_H= get1byte((uint32_t)(B_TF*10),HL);  
				 //Analys->TF_L= get1byte((uint32_t)(B_TF*10),LL); 		//含水量 用户自行计算 总水重 = 体重 * B_BWP			
			
				 //Analys->LBM_H = get1byte((uint32_t)(B_LBM*10),HL);  
				 //Analys->LBM_L = get1byte((uint32_t)(B_LBM*10),LL);	 //去脂体重 用户自行计算 去脂体重 = 体重 - 体重 * B_BFR
			
				 //Analys->FM_H = get1byte((uint32_t)(B_FM*10),HL);  
				 //Analys->FM_L = get1byte((uint32_t)(B_FM*10),LL);  	  //脂肪重  用户自行计算 脂肪重 = 体重 * B_BFR
			
			
         //身体机能参数 9
				 Analys.BMR_H = get1byte(B_BMR,HL);  
				 Analys.BMR_L = get1byte(B_BMR,LL);				            //基础代谢

				 Analys.VFR_H = get1byte((uint32_t)(B_VFR*10),HL);  
				 Analys.VFR_L = get1byte((uint32_t)(B_VFR*10),LL);    //内脏脂肪等级	

				 Analys.MA = B_MA;                                    //身体年龄	

				 Analys.BMI_H = get1byte((uint32_t)(B_BMI*10),HL);  
				 Analys.BMI_L = get1byte((uint32_t)(B_BMI*10),LL);    //体质指数
							
				 Analys.SBC = B_SBC;                                  //身体评分

				 Analys.BW_H= get1byte((uint32_t)(B_BW*10),HL);  
				 Analys.BW_L= get1byte((uint32_t)(B_BW*10),LL);	     //标准体重	

				 if(B_WC>0)                                            //体重控制
				 {
				 Analys.WC_H= get1byte((uint32_t)(B_WC*10),HL);  
				 Analys.WC_L= get1byte((uint32_t)(B_WC*10),LL);			 
				 }
				 else
				 {
				 Analys.WC_H= get1byte((uint32_t)(B_WC*(-10)),HL)+0x80;  
				 Analys.WC_L= get1byte((uint32_t)(B_WC*(-10)),LL);			 
				 }

				 if(B_MC>0)                                         //肌肉控制	
				 {
				 Analys.MC_H= get1byte((uint32_t)(B_MC*10),HL);  
				 Analys.MC_L= get1byte((uint32_t)(B_MC*10),LL);			 
				 }
				 else
				 {
				 Analys.MC_H= get1byte((uint32_t)(B_MC*(-10)),HL)+0x80;  
				 Analys.MC_L= get1byte((uint32_t)(B_MC*(-10)),LL);			 
         }

				 if(B_FC>0)                                         //脂肪控制
				 {
				 Analys.FC_H= get1byte((uint32_t)(B_FC*10),HL);  
				 Analys.FC_L= get1byte((uint32_t)(B_FC*10),LL);			 
				 }
				 else
				 {
				 Analys.FC_H= get1byte((uint32_t)(B_FC*(-10)),HL)+0x80;  
				 Analys.FC_L= get1byte((uint32_t)(B_FC*(-10)),LL);			 
				 }
				 
         //节段部分 10
				 Analys.RAFP_H= get1byte((uint32_t)(B_RAFP*10),HL);  
				 Analys.RAFP_L= get1byte((uint32_t)(B_RAFP*10),LL);//Z2 体脂率

				 Analys.LAFP_H= get1byte((uint32_t)(B_LAFP*10),HL);  
				 Analys.LAFP_L= get1byte((uint32_t)(B_LAFP*10),LL);//Z1 体脂率			 

				 Analys.RLFP_H= get1byte((uint32_t)(B_RLFP*10),HL);  
				 Analys.RLFP_L= get1byte((uint32_t)(B_RLFP*10),LL);//Z4 体脂率

				 Analys.LLFP_H= get1byte((uint32_t)(B_LLFP*10),HL);  
				 Analys.LLFP_L= get1byte((uint32_t)(B_LLFP*10),LL);//Z3 体脂率
				 
				 Analys.TFP_H= get1byte((uint32_t)(B_TFP*10),HL);  
				 Analys.TFP_L= get1byte((uint32_t)(B_TFP*10),LL);//Z5 体脂率	 


				 Analys.RAMM_H= get1byte((uint32_t)(B_RAMM*10),HL);  
				 Analys.RAMM_L= get1byte((uint32_t)(B_RAMM*10),LL);//Z2 肌肉量

				 Analys.LAMM_H= get1byte((uint32_t)(B_LAMM*10),HL);  
				 Analys.LAMM_L= get1byte((uint32_t)(B_LAMM*10),LL);//Z1 肌肉量
				 
				 Analys.RLMM_H= get1byte((uint32_t)(B_RLMM*10),HL);  
				 Analys.RLMM_L= get1byte((uint32_t)(B_RLMM*10),LL);//Z4 肌肉量
				 
				 Analys.LLMM_H= get1byte((uint32_t)(B_LLMM*10),HL);  
				 Analys.LLMM_L= get1byte((uint32_t)(B_LLMM*10),LL);//Z3 肌肉量
				 
				 Analys.TMM_H= get1byte((uint32_t)(B_TMM*10),HL);  
				 Analys.TMM_L= get1byte((uint32_t)(B_TMM*10),LL);//Z5 肌肉量
									 
				 Analys.WHR_H= get1byte((uint32_t)(B_WHR*100),HL);  
				 Analys.WHR_L= get1byte((uint32_t)(B_WHR*100),LL);//腰臀比				 


				 //Analys->EXF_H= get1byte((uint32_t)(B_EXF*10),HL);  
				 //Analys->EXF_L= get1byte((uint32_t)(B_EXF*10),LL);	 //细胞外液	
				 
				 //Analys->INF_H= get1byte((uint32_t)(B_INF*10),HL);  
				 //Analys->INF_L= get1byte((uint32_t)(B_INF*10),LL);   //细胞内液	  
				 			 
				 
				 //Analys->EE_H= get1byte((uint32_t)(B_EE*10),HL);  
				 //Analys->EE_L= get1byte((uint32_t)(B_EE*10),LL);	   //水肿度	
				 
				 //Analys->OD_H= get1byte((uint32_t)(B_OD*10),HL);  
				 //Analys->OD_L= get1byte((uint32_t)(B_OD*10),LL);	   //肥胖度		 
				 
				 
				 //Analys->BM_H= get1byte((uint32_t)(B_BM*10),HL);  
				 //Analys->BM_L= get1byte((uint32_t)(B_BM*10),LL);    //标准肌肉 
				 
				 
				 	 
				 //Analys->TW_H= get1byte((uint32_t)(B_TW*10),HL);  
				 //Analys->TW_L= get1byte((uint32_t)(B_TW*10),LL);        //目标体重
				 
				 		 
				 //Analys->JW_H= get1byte((uint32_t)(B_JW*10),HL);  
				 //Analys->JW_L= get1byte((uint32_t)(B_JW*10),LL);			 //颈围
				 
				 //Analys->WL_H= get1byte((uint32_t)(B_WL*10),HL);  
				 //Analys->WL_L= get1byte((uint32_t)(B_WL*10),LL);			 //腰围
				 
				 //Analys->HL_H= get1byte((uint32_t)(B_HL*10),HL);  
				 //Analys->HL_L= get1byte((uint32_t)(B_HL*10),LL);			 //臀围
				 
				 
				 
				 //Analys->XW_H= get1byte((uint32_t)(B_XW*10),HL);  
				 //Analys->XW_L= get1byte((uint32_t)(B_XW*10),LL);			 //胸围
				 
				 //Analys->RAW_H= get1byte((uint32_t)(B_RAW*10),HL);  
				 //Analys->RAW_L= get1byte((uint32_t)(B_RAW*10),LL);			 //Z2 右上臂围
				 
				 //Analys->LAW_H= get1byte((uint32_t)(B_LAW*10),HL);  
				 //Analys->LAW_L= get1byte((uint32_t)(B_LAW*10),LL);			 //Z1 左上臂围
				 
				 //Analys->RLW_H= get1byte((uint32_t)(B_RLW*10),HL);  
				 //Analys->RLW_L= get1byte((uint32_t)(B_RLW*10),LL);			 //Z4 右大腿围
				 
				 //Analys->LLW_H= get1byte((uint32_t)(B_LLW*10),HL);  
				 //Analys->LLW_L= get1byte((uint32_t)(B_LLW*10),LL);			 //Z3 右大腿围
				 
				 UsrBIAP.Freq1_Z12_Res_H=UsrBIAP.Freq1_Z12_Res_L=0;
				 UsrBIAP.Freq1_Z34_Res_H=UsrBIAP.Freq1_Z34_Res_L=0;	

				 return TRUE;		 	 
		}	 
	}
  return TRUE;	
}


#ifdef HR_MODE

uint8_t CS125X_XLCL_MODE(uint32_t B_HR_ADC);

//Tx
#ifdef UART_HR
void simuart_sendbyte(uint8_t dat)
{
	     uint8_t j=0;
	
	     SIM_TX_L; //st
	     delay_us(5);
	    
	     for(;j<8;j++)
	     {
						if(((dat>>j)&0x01)) {SIM_TX_H;}
					else {SIM_TX_L;}
					delay_us(5);
			 }
	     
        SIM_TX_H;  //end
			  delay_us(5);
}
#endif

void CS125X_XLCL_Initial_Per(void)
{
			SetFATSDAOutput(FAT_SDA);//设置 FAT SDA输出		
			SendDataToAddr(ADDR_AFE_ADC1,0x60);//ADC1 0x60  80hz 
	
	    if(test_sta == HAND_1)				
				{
					SendDataToAddr(ADDR_AFE_BIM0,0x14);//12	  
				}				
			else
			  {
					SendDataToAddr(ADDR_AFE_BIM0,0xBE);//34	  
				} 
			SendDataToAddr(ADDR_AFE_BIM1,0x02);//25k	
	
			SendDataToAddr(ADDR_AFE_SYS,0xCF);//SYS 写0xCF 打开BIM
			B_temp_XLCL = 0;
}

uint8_t HRI(void)
{
//		 BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, ENABLE);
//     TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//		 TIM_Cmd(TIM2, ENABLE);	//打开定时器5ms	 
	  //waiting for ready
		wait_out=0;
	  while(GetFATSDAInputStatus(FAT_SDA)==0)  //SDA==0
		 {
		    if(wait_out>10) //100ms
				{
					SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
					uint8_t tmpcheck=0xf0;
					USART_SendData(USART2, tmpcheck);
					while(1);
					return 0;
				}
		 }
		wait_out=0;
	  while(GetFATSDAInputStatus(FAT_SDA)==1)  //SDA==1
		 {
		    if(wait_out>10) //100ms
				{
					SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
					uint8_t tmpcheck=0xf0;
					USART_SendData(USART2, tmpcheck);
					while(1);
					return 0;
				}
		 } 
//		BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//		TIM_Cmd(TIM2, DISABLE);	//关闭定时器
		 
	  currentADC=ReadADC();                     //read AD
    ///////////		 
		#ifdef UART_HR
		simuart_sendbyte(0x5a); 
		simuart_sendbyte(((currentADC>>16)&0xff)); //H
		simuart_sendbyte(((currentADC>>8)&0xff)); //M
		simuart_sendbyte((currentADC&0xff)); //L
		#endif
		///////////
    return  CS125X_XLCL_MODE(currentADC);
}

#endif
