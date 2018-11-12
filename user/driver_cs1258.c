#include "driver_cs1258.h" 

uint32_t BIA_FLAG=0;   //关于BIA的标志位见FunctionBIAFlag
uint8_t  BIA_Pointer=0;//操作步骤
 
uint8_t waste_times; //丢掉AD次数
uint8_t	all_times;   //总共AD次数
uint8_t	read_times;  //记录读AD次数
uint8_t	out_times;   //超时计数
					
uint32_t lastADC;    //上次AD
uint32_t currentADC; //当前AD
uint32_t sumADC;     //AD累加和
uint16_t temp_avg;   //均值AD
uint16_t temp_res;   //计算电阻

uint16_t   fk_val;     //该频段k
//uint8_t  fk_presion;  //该频精度



BIM_InitTypeDef  BIM_Init; 

/*start */
void startCS125x(void)
{
			ClrFATSCL(FAT_SCL);  //SCL=0
			delay_us(5);
			ClrFATCS(FAT_CS);    //CS=0
			delay_us(5);
}

/*stop */
void stopCS125x(void)
{
			SetFATCS(FAT_CS);    //CS=1
			delay_us(5);	
			SetFATSCL(FAT_SCL);  //SCL=1
			SetFATSDAInput(FAT_SDA);	//SDA 输入  	
			delay_us(5);
}

/*向125x 发送1 byte */
void send1byte(uint8_t data)
{
			uint8_t i;
			SetFATSDAOutput(FAT_SDA);//SDA设置输出
			for(i=0;i<8;i++)
			{
						if(data&0x80)          //MSB
							 SetFATSDA(FAT_SDA); //SDA=1
						else
							 ClrFATSDA(FAT_SDA); //SDA=0
						delay_us(5);
						SetFATSCL(FAT_SCL);    //SCL=1  写
						delay_us(5);
						ClrFATSCL(FAT_SCL);    //SCL=0
						data<<=1;              //data左移1位
						delay_us(5);           //cs125x 读数据				
			}
			delay_us(10); 		
}

/*读125x 1 byte */
uint8_t read1byte(void)
{
			uint8_t i,data=0;
			SetFATSDAInput(FAT_SDA);	//SDA 输入  	
			for(i=0;i<8;i++)
			{
						data<<=1;
						SetFATSCL(FAT_SCL);    //SCL=1
						delay_us(5);
						ClrFATSCL(FAT_SCL);    //SCL=0
						if(GetFATSDAInputStatus(FAT_SDA)) //SDA=1
							data=data+1;
						delay_us(5);
			}
			 return   data;
}

/*向addr地址写data数据*/
void SendDataToAddr(uint8_t addr,uint8_t data)
{
			addr=addr|0x80; //地址第7位置为1，代表写
			startCS125x();  //发送开始
			send1byte(addr);//发送地址
			send1byte(data);//发送数据
			stopCS125x();   //发送结束	
}

/*读addr地址中的数据*/
uint8_t ReadAddrData(uint8_t addr)
{
			uint8_t temp;
			startCS125x();  //发送开始
			send1byte(addr);//发送地址
			temp=read1byte();//读数据
			stopCS125x();   //发送结束	
			return	temp;	
}
/*读ADC结果*/
uint32_t ReadADC(void)
{
			uint8_t tempH,tempM,tempL;
			startCS125x();  //发送开始
			send1byte(ADDR_AFE_ADO);//发送地址ADO
		
			tempH=read1byte()^0x80;
			tempM=read1byte();
			tempL=read1byte();
		
			stopCS125x();   //发送结束	
			return   ((tempH<<16)|(tempM<<8)|(tempL));
}


/**
  * @brief  复位CS125x 通讯口
  * @param  None
  * @retval None
  */ 
void	Rest_Communicate_Interface(void)
{
		
			ClrFATCS(FAT_CS);    //CS=0
			SetFATSDA(FAT_SDA);  //SDA=1
			ClrFATSCL(FAT_SCL);  //SCL=0
		
	
			GPIO_InitTypeDef        GPIO_InitStructure;
	
			RCC_AHBPeriphClockCmd(CS1258_GPIO_CLK, ENABLE);
	
			/* Configure PA9 and PA10 in output pushpull mode */
			GPIO_InitStructure.GPIO_Pin = FAT_SDA | FAT_SCL | FAT_CS;
	
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(CS1258_GPIO, &GPIO_InitStructure);	
	
			delay_ms(1);         //1ms
	
	
	
}
/**
  * @brief  上电复位CS125x 
  * @param  None
  * @retval None
  */ 
void	Reset_CS125x(void)
{
			uint8_t temp,i=0;
			
			Rest_Communicate_Interface();         //1.复位通讯口
			
			while(1)
			{
					SendDataToAddr(0xEA,0x96);        //2.发送复位指令 0xEA 0x96
					delay_ms(5);         //5ms
					i++;
		
					temp=ReadAddrData(ADDR_AFE_ADS);	//3.读ADS寄存器。判断是否复位成功
					if(((temp&0x40)==0x40)||(i>4))
						break;
			}		
			SendDataToAddr(ADDR_AFE_TEST,0x5A);	  //4.进入测试模式 ADDR_AFE_TEST 写0x5A	
			SendDataToAddr(ADDR_AFE_LDOT,0xFF);	  //5.triming ADDR_AFE_LDOT 写0xFF		
			SendDataToAddr(ADDR_AFE_OSCT,0xFF);   //6.triming ADDR_AFE_OSCT 写0xFF		
			SendDataToAddr(ADDR_AFE_CONFIG,0xBD);	//7.open sin ADDR_AFE_CONFIG 写0xBF--->0xBD		
}
	
/**
  * @brief  初始化CS125x 初始化完成到打开BIM之间要有50ms时间间隔
  * @param  None
  * @retval None
  */ 	
void InitCS125x(void)
{
			
			SetFATSDAOutput(FAT_SDA);//设置 FAT SDA输出		
	
			SendDataToAddr(ADDR_AFE_ADC0,0x00);//ADC0 		
			SendDataToAddr(ADDR_AFE_ADC1,0x40);//ADC1 0x40  40hz 		
			SendDataToAddr(ADDR_AFE_ADC2,0x00);//ADC2 			
			SendDataToAddr(ADDR_AFE_ADC3,0x03);	//ADC3 			
			SendDataToAddr(ADDR_AFE_ADC4,0x42);	//ADC4 0x42  正参考=外部REFP,负参考=外部REFN bim时建议设置2.8V		
			SendDataToAddr(ADDR_AFE_ADC5,0x00);	//ADC5 
			SendDataToAddr(ADDR_AFE_SYS,0xCE);	//SYS  0xCF  BIM模式,普通模式,VREF、LDO、ADC打开BIM关闭
			delay_ms(50);         //50ms	
}

/**
  * @brief  SCL口保持保持高电平超过172uS,进入PowerDown模式
            power down 之前要关掉LDO、ADC、BIM
            power down 唤醒之后先使能LDO、ADC再延时50ms才能使能BIM
            SCL拉高180us 
  * @param  None
  * @retval None
  */
void PowerDownCS125x(void)
{
			//power down 之前要关掉LDO、ADC、BIM
			SendDataToAddr(ADDR_AFE_SYS,0xC8);
			
			ClrFATCS(FAT_CS);    //CS=0
			ClrFATSCL(FAT_SCL);  //SCL=0
			delay_us(1); 	
			SetFATSCL(FAT_SCL);  //SCL=1
			delay_us(1000);
			SetFATCS(FAT_CS);    //CS=1
}

/////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
/*   leg  hand  body  body
	 -1	0		  1		 2		 3
1频	0 0     1    2     2
2频	1 3     4    5     5
3频	2 6     7    8     8
由于M_PART=11时保留,默认把他和10当作同一状态(8电极全身)
找到是哪个部位的几个频段*/
int8_t tab[4][5]={ -1,0,1,2,3,\
								    0,0,1,2,2,\
									  1,3,4,5,5,\
									  2,6,7,8,8};

uint8_t find_tab(uint8_t bodypart,uint8_t f_cnt)
{
	uint8_t i,p1,p2;
		for(i=1;i<5;i++)  //列
			{
					if(tab[0][i]==bodypart)
					{p1=i;break;}					
			}
		for(i=1;i<4;i++)  //行
			{
					if(tab[i][0]==f_cnt)
					{p2=i;break;}
			}
		return 	tab[p2][p1];  //返回0~8 9个状态
}

uint8_t freq[3]={0,0,0}; //存放所有频段，注意只使用低4bit，高4bit为0
//freq[0] :第1频段频率
//freq[1] :第2频段频率
//freq[2] :第3频段频率
uint8_t f_cnt;   //记录频段个数 
//0:1个频段 
//1:2个频段 
//2:3个频段 
uint8_t test_sta;//测试状态
//0:leg_1 1:hand_1 2:body_1
//3:leg_2 4:hand_2 5:body_2
//6:leg_3 7:hand_3 8:body_3

//通过EquMent->FREQ EquMent->STATUS
//得到频段个数 f_cnt、频段内容 freq[f_cnt]、测量部位信息test_sta
//parm EquMent->M_PART  EquMent->FREQ 
void tab_ref_test(void)
{
	    uint8_t i;
	   //频率个数和频段解析
			f_cnt=0;
	
			for(i=0;i<7;i++)  //只到bit6
			{
				if((EquMent.FREQ>>i)&0x01)
				{
					 if(i==6){ freq[f_cnt]=(3<<0);f_cnt++;break; }//默认50khz	 写0x40 (bit6写1)
					 
					 if(i!=2)
					 {  		//剔除25k bit2
							freq[f_cnt]=(i<<0);//只有i不为2,才向freq中更新,也就是剔除25k bit2
							f_cnt++;
					 }
				}
			  if(f_cnt==3)break;
			}	
			f_cnt--; 	
			
			
			if(!(EquMent.FREQ&0x07f)) //bit7写1，写0x80
			{
			freq[0]=(3<<0);//默认50khz
			f_cnt=0;
			}		
			
			//按照协议更新EquMent->FREQ
			EquMent.FREQ=0;
			for(i=0;i<(f_cnt+1);i++)
			{			
				/*BIM1[3:0]
				         5---250k
				         4---100k
				         3--- 50k
				         2--- 25k
				         1--- 10k		
				         0---  5k				
				*/
				switch (freq[i])
				{
					case 0: EquMent.FREQ|=(1<<0);break;
          case 1: EquMent.FREQ|=(1<<1);break;					
				  case 2: EquMent.FREQ|=(1<<2);break;
					case 3: EquMent.FREQ|=(1<<3);break;
					case 4: EquMent.FREQ|=(1<<4);break;
					case 5: EquMent.FREQ|=(1<<5);break;
					default:EquMent.FREQ|=(1<<3);break;
				}		
			}		
		//测量部位状态解析 
			test_sta=find_tab(EquMent.M_PART&0x03,f_cnt);
}





/**
  * @brief  get BIM0 BIM1  reg val					
  * @param  FunctionISIN  isino
            FunctionISIN  isini
            FunctionVSEN  vsenp
            FunctionVSEN  vsenn

            FunctionDemodulate       MIXorFWR
            FunctionBimMode          bim_mode
            uint8_t                  freq_select 

		  BIM_InitTypeDef * BIM_InitStruct

  * @retval bim0_temp 解析到要写入BIM0 reg val
						bim1_temp 解析到要写入BIM1 reg val
  */
                  
void BIMRegInit(void)
{
	uint8_t i,bim1_temp=0; //init =0; 
	 ////////////////////////////////BIM1//////////////////////////////////////////
	  if(BIM_Init.BIM1_MIXorFWR==MIX)
			bim1_temp|=0x40;   //mix or fwr  fwr=0
		
		switch(BIM_Init.BIM_Mode)     //BIM模式选择
		{
			case TEST_MODE:bim1_temp&=~(3<<4);break;	                  //bit5 4 clr 				  
			case RES0_MODE:bim1_temp&=~(1<<5);bim1_temp|=(1<<4);break;	//bit5 clr  bit4 set 
			case RES1_MODE:bim1_temp|=(1<<5); bim1_temp&=~(1<<4);break; //bit5 set  bit4 clr  
			case SHORT_MODE:bim1_temp|=(3<<4);break;		                //bit5 bit4 set	  
      default:       bim1_temp&=~(3<<4);break;				            //default TEST_MODE
		}
		
			for(i=0;i<(f_cnt+1);i++)  //得到BIM1 val
			{			
			 BIM_Init.BIM1[i]=bim1_temp|freq[i];
			}	
	 ///////////////////////////////BIM0////////////////////////////////////////////		
  if(BIM_Init.HUMANorREF)		//HUMAN=1
	{
	      uint8_t bim0_temp=0;
				switch(BIM_Init.BIM0_ISINO)  //电流输出
				{ 
					case ISIN0:bim0_temp&=~(3<<6);break;	                  //bit7 6 clr 			
					case ISIN1:bim0_temp&=~(1<<7);bim0_temp|=(1<<6);break;	//bit7 clr  bit 6 set		
					case ISIN2:bim0_temp|=(1<<7); bim0_temp&=~(1<<6);break;	//bit7 set  bit 6 clr	
					case ISIN3:bim0_temp|=(3<<6);break;	                    //bit7 6 set		
					default:   bim0_temp|=(1<<7); bim0_temp&=~(1<<6); break; //default ISIN2
				}
				switch(BIM_Init.BIM0_ISINI)  //电流输入
				{
					case ISIN0:bim0_temp&=~(3<<4);break;	                  //bit5 4 clr 			
					case ISIN1:bim0_temp&=~(1<<5);bim0_temp|=(1<<4);break;	//bit5 clr  bit 4 set		
					case ISIN2:bim0_temp|=(1<<5); bim0_temp&=~(1<<4);break;	//bit5 set  bit 4 clr	
					case ISIN3:bim0_temp|=(3<<4);break;	                    //bit5 4 set		
					default:   bim0_temp|=(3<<4);break;                     //default ISIN3
				}	
				switch(BIM_Init.BIM0_VSENP)  //电压正端
				{
					case VSEN0:bim0_temp&=~(3<<2);break;	                  //bit3 2 clr 			
					case VSEN1:bim0_temp&=~(1<<3);bim0_temp|=(1<<2);break;	//bit3 clr  bit 2 set		
					case VSEN2:bim0_temp|=(1<<3); bim0_temp&=~(1<<2);break;	//bit3 set  bit 2 clr	
					case VSEN3:bim0_temp|=(3<<2);break;	                    //bit3 2 set		
					default:   bim0_temp|=(3<<2);break;                     //default VSEN3
				}		
				switch(BIM_Init.BIM0_VSENN)  //电压负端
				{
					case VSEN0:bim0_temp&=~(3<<0);break;	                  //bit1 0 clr 			
					case VSEN1:bim0_temp&=~(1<<1);bim0_temp|=(1<<0);break;	//bit1 clr  bit 0 set		
					case VSEN2:bim0_temp|=(1<<1); bim0_temp&=~(1<<0);break;	//bit1 set  bit 0 clr	
					case VSEN3:bim0_temp|=(3<<0);break;	                    //bit1 0 set		
					default:   bim0_temp|=(1<<1); bim0_temp&=~(1<<0);break;  //default VSEN2
				}			
					 BIM_Init.BIM0=bim0_temp;
		}
}



/**
  * @brief  BIM 寄存器配置			
  * @param  
  * @retval 
  */
void BIMRegCfg(FunctionFreq Fx)
{
	    //配置BIM0 选择通道	
	    if(BIM_Init.HUMANorREF) //HUMAN=1
			        SendDataToAddr(ADDR_AFE_BIM0,BIM_Init.BIM0); 
			
	    //配置BIM1  测量内短、ref、外部;频率
			SendDataToAddr(ADDR_AFE_BIM1,BIM_Init.BIM1[Fx]);		
			
			//SYS 写0xCF 打开BIM
			SendDataToAddr(ADDR_AFE_SYS,0xCF);
}

/**
  * @brief  初始化参数
  * @param  
  * @retval 
  */
void Init_Parm1(uint8_t w,uint8_t a)
{
			waste_times=w;    //丢掉AD次数
	    all_times=a;      //总共AD次数
	    read_times=0;     //记录读AD次数
			out_times=0;      //超时计数
	
			lastADC=0;       //上次AD
			currentADC=0;    //当前AD
			sumADC=0;	       //AD累加和
			temp_avg=0;      //AD均值
}
/**
  * @brief  获得稳定ADC均值
  * @param  
  * @retval 
  */

uint8_t  wait_out=0;
uint16_t ReadAvgADC(uint8_t waste_times,uint8_t all_times)
{
	  uint32_t tempADC;
	
		uint8_t  temp=waste_times-0;
	
	  BIA_FLAG&=~(1<<BErr_stb); 
		EquMent.ERR_STATUS&=~(1<<Err_Stb); //ram
	
	 while(1)
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
//					BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);
//          TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//	        TIM_Cmd(TIM2, DISABLE);	//关闭定时器
					SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
					BIA_Pointer--;  //next 
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
//			 	  BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);
//          TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//	        TIM_Cmd(TIM2, DISABLE);	//关闭定时器
					SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
					BIA_Pointer--;  //next 
					uint8_t tmpcheck=0xf1;
					USART_SendData(USART2, tmpcheck);
					while(1);
					return 0;
				}
		 }
//	  BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//	  TIM_Cmd(TIM2, DISABLE);	//关闭定时器
		 
	  currentADC=ReadADC();                     //read AD
	  read_times++;    
		
		 //超时处理
	  if(out_times++>100)  
		{
				SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
			
				BIA_FLAG|=(1<<BErr_stb);  // set 判稳定出错 超时
			  BIA_Pointer=Short_Init_1;
			  EquMent.ERR_STATUS|=(1<<Err_Stb);      //ram
			  //SetDRDY(0);
				update_sta_flg();
			
				////////////测量模式下///////////////		
				if(((EquMent.STATUS>>ON_OFF)&0x01)) //测量
				{			
				BIA_FLAG|=(1<<GetBodyDone);// 测人完成			
				EquMent.STATUS&=~(1<<ON_OFF); //完成clr				
				}		 
				////////////校准模式下///////////////
			 else                                   //校准
			  {				
				BIA_FLAG|=(1<<CalKDone);  // 校准完成				
				EquMent.CONFIG_K&=~(1<<CK);//完成clr			
			  }					 
				return 0;
		}

		if(read_times>temp)       //丢弃waste_times笔
		{
				 if(lastADC>=currentADC) 
					 tempADC=lastADC-currentADC;
				 else
					 tempADC=currentADC-lastADC;
				 
				 lastADC=currentADC;  //update
				 
				 if(tempADC<=1400)
					 {							 
						 sumADC+=lastADC; 
						 if(read_times>all_times)
							{
								 sumADC/=(all_times-waste_times);	//求均值			            
								 SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS 写0xCE 关闭BIM		
//							 BIA_FLAG&=~(1<<Enable_init);  //允许初始化
								 BIA_Pointer++;  //next 
								return sumADC>>8;
							}
					 }
				 else
					 {		
						 if(!(read_times==(temp+1)))	//temp+1次不clr	read_times		 
						 {read_times=0;} //CLR 读AD次数							 
						  sumADC=0;      //CLR AD和
					 }
		}	
	}
}



//内短 //Ref0 //Ref1
void Ref_Init_All(FunctionBimMode BimMode,FunctionFreq Fx)
{
			BIM_Init.BIM1_MIXorFWR  =FWR;          //FWR
			BIM_Init.BIM_Mode       =BimMode;      //内短 //Ref0 //Ref1
			BIM_Init.HUMANorREF     =REF;          //参考 	
			BIMRegInit();                          //get BIM1 reg val

			BIMRegCfg(Fx);              //输出波形
			Init_Parm1(FILTER_AD_TIMES,RES_AD_TIMES); //初始化内短滤波参数和清变量	 
}

//Z34 //Z43 //Z12 //Z21 //Z13 //Z14 //Z23 //Z24
void Res_Init_All(FunctionISIN ISINO,FunctionISIN ISINI,FunctionVSEN VSENP,FunctionVSEN VSENN,FunctionFreq Fx)
{
			BIM_Init.BIM1_MIXorFWR  =FWR;           //FWR
			BIM_Init.BIM_Mode       =TEST_MODE;     //TEST
			BIM_Init.HUMANorREF     =HUMAN;         //HUMAN
	
			BIM_Init.BIM0_ISINO     =ISINO;        //电流O--->I
			BIM_Init.BIM0_ISINI     =ISINI;	       //电压N--->P
	    BIM_Init.BIM0_VSENP     =VSENP;
	    BIM_Init.BIM0_VSENN     =VSENN;	
	
			BIMRegInit();                    //get BIM0 BIM1 reg val

			BIMRegCfg(Fx);     						   //HUMAN模式 			
			Init_Parm1(FILTER_AD_TIMES,BODY_AD_TIMES); //初始化HUMAN滤波参数和清变量	 
}


//搬电阻...= =
void mov_res(FunctionTestSta part)
{
  switch(part)
	{
		case LEG_1:                                      
		           UsrBIAP.Freq1_Z34_Res_H=UsrBIAP.Freq1_All_Res_H=RESVal.F1_Z34_RES_H;
		           UsrBIAP.Freq1_Z34_Res_L=UsrBIAP.Freq1_All_Res_L=RESVal.F1_Z34_RES_L;break;
	
		case HAND_1:
		            UsrBIAP.Freq1_Z12_Res_H=UsrBIAP.Freq1_All_Res_H=RESVal.F1_Z12_RES_H;
		            UsrBIAP.Freq1_Z12_Res_L=UsrBIAP.Freq1_All_Res_L=RESVal.F1_Z12_RES_L;break;		
		
		case BODY_1:
								UsrBIAP.Freq1_Z34_Res_H=UsrBIAP.Freq1_All_Res_H=RESVal.F1_Z34_RES_H;
			          UsrBIAP.Freq1_Z34_Res_L=UsrBIAP.Freq1_All_Res_L=RESVal.F1_Z34_RES_L;
		            UsrBIAP.Freq1_Z12_Res_H=RESVal.F1_Z12_RES_H;
		            UsrBIAP.Freq1_Z12_Res_L=RESVal.F1_Z12_RES_L;
								UsrBIAP.Freq1_Z13_Res_H=RESVal.F1_Z13_RES_H;
								UsrBIAP.Freq1_Z13_Res_L=RESVal.F1_Z13_RES_L;
								UsrBIAP.Freq1_Z14_Res_H=RESVal.F1_Z14_RES_H;
								UsrBIAP.Freq1_Z14_Res_L=RESVal.F1_Z14_RES_L;	
							  UsrBIAP.Freq1_Z23_Res_H=RESVal.F1_Z23_RES_H;
		            UsrBIAP.Freq1_Z23_Res_L=RESVal.F1_Z23_RES_L;	 
								UsrBIAP.Freq1_Z24_Res_H=RESVal.F1_Z24_RES_H;
								UsrBIAP.Freq1_Z24_Res_L=RESVal.F1_Z24_RES_L;	 
		
								UsrBIAP.Freq1_Z1_Res_H=get1byte(   
																									(((RESVal.F1_Z14_RES_H<<8)|RESVal.F1_Z14_RES_L + 
																									  (RESVal.F1_Z12_RES_H<<8)|RESVal.F1_Z12_RES_L -
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L)>>1)
																							,HL);
								UsrBIAP.Freq1_Z1_Res_L=get1byte(   
																									(((RESVal.F1_Z14_RES_H<<8)|RESVal.F1_Z14_RES_L + 
																									  (RESVal.F1_Z12_RES_H<<8)|RESVal.F1_Z12_RES_L -
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L)>>1)
																							,LL);
																							
								UsrBIAP.Freq1_Z2_Res_H=get1byte(   
																									(((RESVal.F1_Z12_RES_H<<8)|RESVal.F1_Z12_RES_L + 
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L -
																									  (RESVal.F1_Z14_RES_H<<8)|RESVal.F1_Z14_RES_L)>>1)
																							,HL);
								
								UsrBIAP.Freq1_Z2_Res_L=get1byte(   
																									(((RESVal.F1_Z12_RES_H<<8)|RESVal.F1_Z12_RES_L + 
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L -
																									  (RESVal.F1_Z14_RES_H<<8)|RESVal.F1_Z14_RES_L)>>1)
																							,LL);
																							
								UsrBIAP.Freq1_Z3_Res_H=get1byte(   
																									(((RESVal.F1_Z23_RES_H<<8)|RESVal.F1_Z23_RES_L + 
																									  (RESVal.F1_Z34_RES_H<<8)|RESVal.F1_Z34_RES_L -
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L)>>1)
																							,HL);
																							
								UsrBIAP.Freq1_Z3_Res_L=get1byte(   
																									(((RESVal.F1_Z23_RES_H<<8)|RESVal.F1_Z23_RES_L + 
																									  (RESVal.F1_Z34_RES_H<<8)|RESVal.F1_Z34_RES_L -
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L)>>1)
																							,LL);
								
								UsrBIAP.Freq1_Z4_Res_H=get1byte(   
																									(((RESVal.F1_Z34_RES_H<<8)|RESVal.F1_Z34_RES_L + 
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L -
																									  (RESVal.F1_Z23_RES_H<<8)|RESVal.F1_Z23_RES_L)>>1)
																							,HL);
								

								UsrBIAP.Freq1_Z4_Res_L=get1byte(   
																									(((RESVal.F1_Z34_RES_H<<8)|RESVal.F1_Z34_RES_L + 
																									  (RESVal.F1_Z24_RES_H<<8)|RESVal.F1_Z24_RES_L -
																									  (RESVal.F1_Z23_RES_H<<8)|RESVal.F1_Z23_RES_L)>>1)
																							,LL);
								////////////////////////////////////////////////////////////////////
								//UsrBIAP->Freq1_Z5_Res_H=get1byte(((F1DWRes.Z13+F1DWRes.Z14+F1DWRes.Z23+F1DWRes.Z24-(F1DWRes.Z34<<1)-(F1DWRes.Z12<<1))>>2),HL);
								//UsrBIAP->Freq1_Z5_Res_L=get1byte(((F1DWRes.Z13+F1DWRes.Z14+F1DWRes.Z23+F1DWRes.Z24-(F1DWRes.Z34<<1)-(F1DWRes.Z12<<1))>>2),LL);
								UsrBIAP.Freq1_Z5_Res_H=get1byte(   
																									(((RESVal.F1_Z14_RES_H<<8)|RESVal.F1_Z14_RES_L + 
																									  (RESVal.F1_Z23_RES_H<<8)|RESVal.F1_Z23_RES_L )/50)
																							,HL);
								
								UsrBIAP.Freq1_Z5_Res_L=get1byte(   
																									(((RESVal.F1_Z14_RES_H<<8)|RESVal.F1_Z14_RES_L + 
																									  (RESVal.F1_Z23_RES_H<<8)|RESVal.F1_Z23_RES_L )/50)
																							,LL);
								////////////////////////////////////////////////////////////////////
								break;     
			
			default:
								UsrBIAP.Freq1_All_Res_H = RESVal.F1_Z34_RES_H;
			          UsrBIAP.Freq1_All_Res_L = RESVal.F1_Z34_RES_L;			
								UsrBIAP.Freq1_Z1_Res_H  = RESVal.F1_Z1_RES_H;
			          UsrBIAP.Freq1_Z1_Res_L  = RESVal.F1_Z1_RES_L;
								UsrBIAP.Freq1_Z2_Res_H  = RESVal.F1_Z2_RES_H;
			          UsrBIAP.Freq1_Z2_Res_L  = RESVal.F1_Z2_RES_L;
								UsrBIAP.Freq1_Z3_Res_H  = RESVal.F1_Z3_RES_H;
			          UsrBIAP.Freq1_Z3_Res_L  = RESVal.F1_Z3_RES_L;
								UsrBIAP.Freq1_Z4_Res_H  = RESVal.F1_Z4_RES_H;
			          UsrBIAP.Freq1_Z4_Res_L  = RESVal.F1_Z4_RES_L;
								UsrBIAP.Freq1_Z5_Res_H  = RESVal.F1_Z5_RES_H;     
								UsrBIAP.Freq1_Z5_Res_L  = RESVal.F1_Z5_RES_L;break;     
	}
}

//下面就是繁琐重复的操作。。。
//1频 参考流程
void Ref_F1(void)
{
	switch(BIA_Pointer)
			{ 
					case Short_Init_1:
									Ref_Init_All(SHORT_MODE,F1);break;
									
					case Ref0_Init_1: 
									Ref_Init_All(RES0_MODE,F1) ;break;
									
					case Ref1_Init_1: 
									Ref_Init_All(RES1_MODE,F1) ;break;
					
					default:BIA_Pointer=Short_Init_1;break;
			}				
}

//获得16bit 的高8 低8 如0xab
// x=HH return  0xa0;
// x=HL return  0x0a;
// x=LL return  0x0b;
// x=LH return  0xb0;
uint16_t get1byte(uint32_t temp,HLX X)
{
	       if(X==HH) return  (temp&0xFF00);
		else if(X==HL) return  ((temp&0xFF00)>>8);
    else if(X==LL) return   (temp&0xFF);
		else if(X==LH) return  ((temp&0xFF)<<8);				
		return 0;
}

//组合  如 a:0xab b:0xcd
//x=H return 0xac00
//x=L return 0x00bd
uint16_t get2byte(uint32_t a,uint32_t b,XHL X)
{
	if(X==H)//组合高8bit
		return   ((a&0xFF00)|((b&0xFF00)>>8));
	else if(X==L) //组合低8bit
		return    (((a&0xFF)<<8)|(b&0xFF));
	return 0;
}


//1频 参考输出结果
void Ref_F1_ADC(void)
{
		switch(BIA_Pointer-1)
			{ 
					case Short_Init_1:
									ADCVal.Freq1_Short_AD_H=get1byte(temp_avg,HL);
									ADCVal.Freq1_Short_AD_L=get1byte(temp_avg,LL);
									break;
					case Ref0_Init_1: 
									ADCVal.Freq1_Ref0_AD_H=get1byte(temp_avg,HL);
									ADCVal.Freq1_Ref0_AD_L=get1byte(temp_avg,LL);
									break;
					case Ref1_Init_1: 
									ADCVal.Freq1_Ref1_AD_H=get1byte(temp_avg,HL);
									ADCVal.Freq1_Ref1_AD_L=get1byte(temp_avg,LL);
														
					BIA_Pointer=Short_Init_1;        //1频  reset pointer
					BIA_FLAG|=(1<<GetRef);           //1频  已经获得完整参考	
					EquMent.Z_STATUS|=(1<<Z_REDY);  //1频 set 模块就绪标志	ram									
									break;
					
					default:BIA_Pointer=Short_Init_1;break;
			}
}




/**
  * @brief  获得参考
						1、配置BIM1 2、写BIM1 reg和初始化滤波参数 3、读稳定AD
  * @param  
  * @retval 
  */
void GetRef01ADC(FunctionFreq Fx)
{						
		if((BIA_FLAG>>GetRef) & 0x01) 			
			 return;    //已经获得完整参考 return
			
			EquMent.Z_STATUS&=~(1<<Z_REDY); //clr 模块就绪标志
			EquMent.CONFIG_K&=~(1<<CK);    //clr 工厂校准模式
			EquMent.STATUS&=~(1<<ON_OFF);  //clr 测试模式
			
			Ref_F1();//1频
		
		temp_avg=ReadAvgADC(waste_times,all_times);					//参考
		if(temp_avg>0)
			{
					Ref_F1_ADC();//1频
			}	
}

//123频 内短 输出结果 
void Short_Fx_ADC(FunctionFreq Fx)
{

		ADCVal.Freq1_Short_AD_H=get1byte(temp_avg,HL);
		ADCVal.Freq1_Short_AD_L=get1byte(temp_avg,LL);
		return;
}


//123频 Z34 ADC输出结果
void Z34_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z34_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z34_AD_L=get1byte(temp_avg,LL);
		return;	
}

//123频 Z12 ADC输出结果
void Z12_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z12_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z12_AD_L=get1byte(temp_avg,LL);
	return;		
}

//123频 Z13 ADC输出结果
void Z13_Fx_ADC(FunctionFreq Fx)	
{
    ADCVal.Freq1_Z13_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z13_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S13);//ram 13 完成
	return;	
}

//123频 Z14 ADC输出结果
void Z14_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z14_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z14_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S14); //ram 14 完成
	return;	
}

//123频 Z23 ADC输出结果
void Z23_Fx_ADC(FunctionFreq Fx)	
{
    ADCVal.Freq1_Z23_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z23_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S23); //ram 23 完成
	return;	
}

//123频 Z24 ADC输出结果
void Z24_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z24_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z24_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S24); //ram 24 完成
	return;		
}


//123频 Z43 ADC输出结果  反测放在高8位
void Z43_Fx_ADC(FunctionFreq Fx)	
{      
    ADCVal.Freq1_Z34_AD_H|=get1byte(temp_avg,HH);
    ADCVal.Freq1_Z34_AD_L|=get1byte(temp_avg,LH);
		EquMent.Z_STATUS|=(1<<Z_S34);//ram 34 完成			
	return;
}

//123频 Z21 ADC输出结果  反测放在高8位
void Z21_Fx_ADC(FunctionFreq Fx)	
{     	
    ADCVal.Freq1_Z12_AD_H|=get1byte(temp_avg,HH);
    ADCVal.Freq1_Z12_AD_L|=get1byte(temp_avg,LH);	
		EquMent.Z_STATUS|=(1<<Z_S12); //1频 ram 12 完成 		
	return;
}




//123频 Z34 RES输出结果
void Z34_Fx_RES(FunctionFreq Fx)
{			
    RESVal.F1_Z34_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z34_RES_L=get1byte(temp_res,LL);
		return;
}

//123频 Z43 RES输出结果
void Z43_Fx_RES(FunctionFreq Fx)
{
    RESVal.F1_Z34_RES_H|=get1byte(temp_res,HH);
    RESVal.F1_Z34_RES_L|=get1byte(temp_res,LH);	
	return;
}

//123频 Z12 RES输出结果
void Z12_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z12_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z12_RES_L=get1byte(temp_res,LL);
		return;
}
//123频 Z21 RES输出结果
void Z21_Fx_RES(FunctionFreq Fx)
{
    RESVal.F1_Z12_RES_H|=get1byte(temp_res,HH);
    RESVal.F1_Z12_RES_L|=get1byte(temp_res,LH);	
	return;
}

//123频 Z13 RES输出结果
void Z13_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z13_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z13_RES_L=get1byte(temp_res,LL);
		return;
}

//123频 Z14 RES输出结果
void Z14_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z14_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z14_RES_L=get1byte(temp_res,LL);
		return;
}
//123频 Z23 RES输出结果
void Z23_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z23_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z23_RES_L=get1byte(temp_res,LL);
		return;
}
//123频 Z24 RES输出结果
void Z24_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z24_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z24_RES_L=get1byte(temp_res,LL);
		return;
}

//计算123频 各部分电阻
void Body_Part_RES(FunctionFreq Fx)
{
	uint16_t Z13,Z12,Z14,Z23,Z24,Z34;
	
			Z13=get2byte(RESVal.F1_Z13_RES_H,RESVal.F1_Z13_RES_L,L);
		  Z12=get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L);
		  Z14=get2byte(RESVal.F1_Z14_RES_H,RESVal.F1_Z14_RES_L,L);
		  Z23=get2byte(RESVal.F1_Z23_RES_H,RESVal.F1_Z23_RES_L,L);
		  Z24=get2byte(RESVal.F1_Z24_RES_H,RESVal.F1_Z24_RES_L,L);
		  Z34=get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L);
		
			RESVal.F1_Z1_RES_H=get1byte(((Z14+Z12-Z24)>>1),HL);
			RESVal.F1_Z1_RES_L=get1byte(((Z14+Z12-Z24)>>1),LL);
			
			RESVal.F1_Z2_RES_H=get1byte(((Z12+Z24-Z14)>>1),HL);
			RESVal.F1_Z2_RES_L=get1byte(((Z12+Z24-Z14)>>1),LL);
			
			RESVal.F1_Z3_RES_H=get1byte(((Z23+Z34-Z24)>>1),HL);
			RESVal.F1_Z3_RES_L=get1byte(((Z23+Z34-Z24)>>1),LL);
			
			RESVal.F1_Z4_RES_H=get1byte(((Z34+Z24-Z23)>>1),HL);
			RESVal.F1_Z4_RES_L=get1byte(((Z34+Z24-Z23)>>1),LL);	
				
//	  RESVal->F1_Z5_RES_H=get1byte(((Z13+Z14+Z23+Z24-(Z34<<1)-(Z12<<1))>>2),HL);
//		RESVal->F1_Z5_RES_L=get1byte(((Z13+Z14+Z23+Z24-(Z34<<1)-(Z12<<1))>>2),LL);
	    RESVal.F1_Z5_RES_H=get1byte(((Z14+Z23)/50),HL);
		  RESVal.F1_Z5_RES_L=get1byte(((Z14+Z23)/50),LL);  
			return;		
}
//clr Z34
void CLR_Z34_Fx_RES(FunctionFreq Fx)
{
  RESVal.F1_Z34_RES_H=RESVal.F1_Z34_RES_L=0;
	return;
}

//clr Z12
void CLR_Z12_Fx_RES(FunctionFreq Fx)
{
  RESVal.F1_Z12_RES_H=RESVal.F1_Z12_RES_L=0;	
	return;
}

//clr Z13
void CLR_Z13_Fx_RES(FunctionFreq Fx)
{
  RESVal.F1_Z13_RES_H=RESVal.F1_Z13_RES_L=0;
	return;
}

//clr Z14
void CLR_Z14_Fx_RES(FunctionFreq Fx)
{
  RESVal.F1_Z14_RES_H=RESVal.F1_Z14_RES_L=0;	
	return;
}

//clr Z23
void CLR_Z23_Fx_RES(FunctionFreq Fx)
{
  RESVal.F1_Z23_RES_H=RESVal.F1_Z23_RES_L=0;	
	return;
}

//clr Z24
void CLR_Z24_Fx_RES(FunctionFreq Fx)
{
  RESVal.F1_Z24_RES_H=RESVal.F1_Z24_RES_L=0;
	return;
}

//站姿异常判断
uint8_t judge_posture(uint16_t pos,uint16_t neg)
{
	  uint16_t temp;
		BIA_FLAG&=~(1<<BErr_post); //clr BErr_post flag
	  EquMent.ERR_STATUS&=~(1<<Err_exchange); //ram
	
	  if(pos>=neg)
			 temp=pos-neg;
		else
			 temp=neg-pos;
		
  		UsrBIAP.diff_h = temp>>8 & 0xFF;		
  		UsrBIAP.diff_l = temp & 0xFF;
		
		if(temp>POS_NEG_VAL)
		{
		 BIA_FLAG|=(1<<BErr_post);   //set BErr_post flag
		 BIA_FLAG|=(1<<GetBodyDone); //set  GetBodyDone
		 BIA_Pointer=Short_Init_1;   //reset BIA_Pointer

		 EquMent.ERR_STATUS|=(1<<Err_exchange); //ram
			
			////////////测量模式下///////////////					
			if(((EquMent.STATUS>>ON_OFF)&0x01)) //测量
			{							
				EquMent.STATUS&=~(1<<ON_OFF); //完成clr
				//SetDRDY(0);		
        update_sta_flg();				
			}				
			////////////校准模式下///////////////
		 else if(((EquMent.CONFIG_K>>CK)&0x01)) //校准
			{
        EquMent.CONFIG_K&=~(1<<CK);//完成clr	
				//SetDRDY(0);		
				update_sta_flg();								
			}		
		
		 return FALSE;
		}  
		 return TRUE;
}


//根据频段 计算电阻
uint16_t CalRes(uint16_t BodyAdc,FunctionFreq Fx)
{		
	  uint16_t  short_adc,ref0_adc,ref1_adc;
	  uint16_t  temp_res;
	           
			 short_adc=get2byte(ADCVal.Freq1_Short_AD_H,ADCVal.Freq1_Short_AD_L,L); 	  
			 ref0_adc =get2byte(ADCVal.Freq1_Ref0_AD_H, ADCVal.Freq1_Ref0_AD_L,L); 
			 ref1_adc =get2byte(ADCVal.Freq1_Ref1_AD_H, ADCVal.Freq1_Ref1_AD_L,L); 
			 fk_val   =get2byte(EquMent.FREQ1K_VAL_H,EquMent.FREQ1K_VAL_L,L);

		if(((EquMent.CONFIG_K>>CK)&0x01)) //校准时用k=0计算
		      {fk_val=0;}
	
     BIA_FLAG&=~(1<<BErr_Short);// clr 内短出错
	   BIA_FLAG&=~(1<<BErr_Hight);// clr 超过上限
	   BIA_FLAG&=~(1<<BErr_Low);  // clr 低于下限
		
		 EquMent.ERR_STATUS&=~(1<<Err_Short);      //ram
		 EquMent.ERR_STATUS&=~(1<<Err_lowlimit);   //ram
		 EquMent.ERR_STATUS&=~(1<<Err_hightlimit); //ram
			
	   if(BodyAdc<=short_adc)  //小与内短
		 {
							////////////测量模式下///////////////		
							if(((EquMent.STATUS>>ON_OFF)&0x01)) //测量
							{
							BIA_FLAG|=(1<<BErr_Short);  // set 内短出错
							BIA_FLAG|=(1<<GetBodyDone);// 测人完成
							BIA_Pointer=Short_Init_1;
						 
							EquMent.ERR_STATUS|=(1<<Err_Short);      //ram
							EquMent.STATUS&=~(1<<ON_OFF); //完成clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;
							}		 
							////////////校准模式下///////////////
						 else                                   //校准
						 {
							BIA_FLAG|=(1<<BErr_Short);  // set 内短出错
							BIA_FLAG|=(1<<CalKDone);  // 校准完成
							BIA_Pointer=Short_Init_1;
						 
							EquMent.ERR_STATUS|=(1<<Err_Short);      //ram
							EquMent.CONFIG_K&=~(1<<CK);//完成clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;
						 }
		 }
		 else
		 {
			 
			   if(fk_val>>15){ //负
					 fk_val=fk_val-0x8000;
				   temp_res=(BodyAdc-short_adc)*REF_RES/(ref1_adc-ref0_adc+fk_val);
				 }
				 else  //正
				 {
				   temp_res=(BodyAdc-short_adc)*REF_RES/(ref1_adc-ref0_adc-fk_val);
				 }
				 
				 if(temp_res>=RES_MAX)  //大于上限
				 {
							////////////测量模式下///////////////		
							if(((EquMent.STATUS>>ON_OFF)&0x01)) //测量
							{
							BIA_FLAG|=(1<<BErr_Hight);  // set 上限出错
							BIA_FLAG|=(1<<GetBodyDone);// 测人完成
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_hightlimit); //ram
							EquMent.STATUS&=~(1<<ON_OFF); //完成clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;					
							}
							////////////校准模式下///////////////
						 else                                   //校准
							{
							BIA_FLAG|=(1<<BErr_Hight);  // set 上限出错
							BIA_FLAG|=(1<<CalKDone);// 校准完成
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_hightlimit); //ram
							EquMent.CONFIG_K&=~(1<<CK);//完成clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;			 
							}
				 }
				 if(temp_res<=RES_MIN)  //小雨下限
				 {
							////////////测量模式下///////////////	
							if(((EquMent.STATUS>>ON_OFF)&0x01)) //测量
							{
							BIA_FLAG|=(1<<BErr_Low);  // set 下限出错
							BIA_FLAG|=(1<<GetBodyDone);// 测人完成
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_lowlimit);      //ram
							EquMent.STATUS&=~(1<<ON_OFF); //完成clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;	
							}					
							////////////校准模式下///////////////
							else
							{
							BIA_FLAG|=(1<<BErr_Low);  // set 下限出错
							BIA_FLAG|=(1<<CalKDone);// 校准完成
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_lowlimit);      //ram
							EquMent.CONFIG_K&=~(1<<CK);//完成clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;	
							}					
				 }
				 return temp_res;
		 }		 
}


//Leg_1  1频
void GetLegF1(void)
{
			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //内短
							break;
				case Z34_Init_1:
							Res_Init_All(ISIN2,ISIN3,VSEN3,VSEN2,F1);	//Z34
							break;
				case Z43_Init_1:
							Res_Init_All(ISIN3,ISIN2,VSEN2,VSEN3,F1);	//Z43
							break;
				default:BIA_Pointer=Short_Init_1;break;							
			}
	
			temp_avg=ReadAvgADC(waste_times,all_times);					//LEG_1
			if(temp_avg>0)
			{
						switch(BIA_Pointer-1)
						{
							case Short_Init_1: 
									Short_Fx_ADC(F1); 
									break;
							case Z34_Init_1: 
									Z34_Fx_ADC(F1);		//Z34 ADC
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,L),F1);
							    Z34_Fx_RES(F1);   //Z34 RES
									break;
							case Z43_Init_1: 
									Z43_Fx_ADC(F1);   //Z43 ADC						
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,H),F1);
							    Z43_Fx_RES(F1);   //Z43 RES
							
							    //合理性判断
							    if(judge_posture(get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L),\
																	 get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,H)) == FALSE)
									{CLR_Z34_Fx_RES(F1);}
									  
									//replace-----------------加窗处理
									mov_res(LEG_1);       //搬电阻
									//replace-----------------
									
									EquMent.F_STATUS=0x01;//已经执行完第一频率			
									
									BIA_FLAG|=(1<<GetBodyDone);// 测人完成
									BIA_Pointer=Short_Init_1;																		
									EquMent.STATUS&=~(1<<ON_OFF); //完成clr
									//SetDRDY(0);
									update_sta_flg();
									break;
							default:BIA_Pointer=Short_Init_1;break;									
						}							
			}	
}


////////////////////////////////////////////////////
//Hand_1 1频
void GetHandF1(void)
{

			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //内短
							break;
				case Z12_Init_1:
							Res_Init_All(ISIN0,ISIN1,VSEN1,VSEN0,F1);	//Z12
							break;
				case Z21_Init_1:
							Res_Init_All(ISIN1,ISIN0,VSEN0,VSEN1,F1);	//Z21
							break;
				default:BIA_Pointer=Short_Init_1;break;
			}

			temp_avg=ReadAvgADC(waste_times,all_times);			//HAND_1
			if(temp_avg>0)
			{
						switch(BIA_Pointer-1)
						{
							case Short_Init_1: 
									Short_Fx_ADC(F1); 
									break;
							case Z12_Init_1: 
									Z12_Fx_ADC(F1);		//Z12 ADC
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,L),F1);
							    Z12_Fx_RES(F1);   //Z12 RES
									break;
							case Z21_Init_1: 
									Z21_Fx_ADC(F1);   //Z21 ADC						
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,H),F1);
							    Z21_Fx_RES(F1);   //Z21 RES
							
							    //合理性判断
							    if(judge_posture(get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L),\
																	 get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,H))==FALSE)
									{CLR_Z12_Fx_RES(F1);}
									
									//replace-----------------加窗处理 
									mov_res(HAND_1);       //搬电阻
									//replace-----------------

									EquMent.F_STATUS=0x01;//已经执行完第一频率

									BIA_FLAG|=(1<<GetBodyDone);// 测人完成
									BIA_Pointer=Short_Init_1;	
									EquMent.STATUS&=~(1<<ON_OFF); //完成clr
									//SetDRDY(0);
									update_sta_flg();
									break;	
							default:BIA_Pointer=Short_Init_1;break;									
						}							
			}	
}



void GetBodyF1(void)
{
			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //内短
							break;
				case B34_Init_1:
							Res_Init_All(ISIN2,ISIN3,VSEN3,VSEN2,F1);	//Z34
							break;
				case B43_Init_1:
							Res_Init_All(ISIN3,ISIN2,VSEN2,VSEN3,F1);	//Z43
							break;
				case B12_Init_1:
							Res_Init_All(ISIN0,ISIN1,VSEN1,VSEN0,F1);	//Z12
							break;
				case B21_Init_1:
							Res_Init_All(ISIN1,ISIN0,VSEN0,VSEN1,F1);	//Z21
							break;	
				case B13_Init_1:
							Res_Init_All(ISIN0,ISIN2,VSEN2,VSEN0,F1);	//Z13
							break;
				case B14_Init_1:
							Res_Init_All(ISIN0,ISIN3,VSEN3,VSEN0,F1);	//Z14
							break;	
				case B23_Init_1:
							Res_Init_All(ISIN1,ISIN2,VSEN2,VSEN1,F1);	//Z23
							break;
				case B24_Init_1:
							Res_Init_All(ISIN1,ISIN3,VSEN3,VSEN1,F1);	//Z24
							break;
				default:BIA_Pointer=Short_Init_1;break;							
			}	
			temp_avg=ReadAvgADC(waste_times,all_times);			//BODY_1
			if(temp_avg>0)
			{
						switch(BIA_Pointer-1)
						{
							case Short_Init_1: 
									Short_Fx_ADC(F1); 
									break;
							case B34_Init_1: 
									Z34_Fx_ADC(F1);		//Z34 ADC
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,L),F1);
							    Z34_Fx_RES(F1);   //Z34 RES
									break;
							case B43_Init_1: 
									Z43_Fx_ADC(F1);   //Z43 ADC						
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,H),F1);
							    Z43_Fx_RES(F1);   //Z43 RES						
							    //合理性判断
							    if(judge_posture(get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L),\
																	 get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,H))==FALSE)
									{
										CLR_Z34_Fx_RES(F1);EquMent.F_STATUS=0x01;//已经执行完第一频率	
									}
									//replace-----------------加窗处理
									mov_res(LEG_1);       //搬电阻
									//replace-----------------
								  break;								
							case B12_Init_1: 
									Z12_Fx_ADC(F1);		//Z12 ADC
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,L),F1);
							    Z12_Fx_RES(F1);   //Z12 RES
									break;
							case B21_Init_1: 
									Z21_Fx_ADC(F1);   //Z21 ADC						
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,H),F1);
							    Z21_Fx_RES(F1);   //Z21 RES						
							    //合理性判断
							    if(judge_posture(get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L),\
																	 get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,H))==FALSE)
									{CLR_Z12_Fx_RES(F1);EquMent.F_STATUS=0x01;//已经执行完第一频率	
									}
								  break;	
              case B13_Init_1: 
								  Z13_Fx_ADC(F1);
									temp_res=CalRes(get2byte(ADCVal.Freq1_Z13_AD_H,ADCVal.Freq1_Z13_AD_L,L),F1);
							    Z13_Fx_RES(F1);   //Z13 RES
									break;
							case B14_Init_1:
								  Z14_Fx_ADC(F1);
									temp_res=CalRes(get2byte(ADCVal.Freq1_Z14_AD_H,ADCVal.Freq1_Z14_AD_L,L),F1);
							    Z14_Fx_RES(F1);   //Z14 RES
									break;
							case B23_Init_1:
								  Z23_Fx_ADC(F1);
									temp_res=CalRes(get2byte(ADCVal.Freq1_Z23_AD_H,ADCVal.Freq1_Z23_AD_L,L),F1);
							    Z23_Fx_RES(F1);   //Z23 RES
									break;
							case B24_Init_1:
								  Z24_Fx_ADC(F1);
									temp_res=CalRes(get2byte(ADCVal.Freq1_Z24_AD_H,ADCVal.Freq1_Z24_AD_L,L),F1);
							    Z24_Fx_RES(F1);   //Z24 RES
					
							    Body_Part_RES(F1); //计算各部分电阻
							
									//replace-----------------加窗处理
									mov_res(BODY_1);       //搬电阻
									//replace-----------------

							    EquMent.F_STATUS=0x01;//已经执行完第一频率	
							
									BIA_FLAG|=(1<<GetBodyDone);// 测人完成
									BIA_Pointer=Short_Init_1;																
									EquMent.STATUS&=~(1<<ON_OFF); //完成clr
									//SetDRDY(0);
									update_sta_flg();
									break;
							default:BIA_Pointer=Short_Init_1;break;
							}							
			}							
}


/**
  * @brief  获得人体电阻
						1、配置BIM1 2、写BIM1 reg和初始化滤波参数 3、读稳定AD
  * @param  
  * @retval 
  */
void GetBodyRes(void)
{
		if((BIA_FLAG>>GetRef)&0x01) //已经获得完整参考
		{
					if((BIA_FLAG>>GetBodyDone)&0x01)
						return;  //测人完成 
					
					switch(test_sta)  //测试状态 找到是哪频段哪部位
					{
					  case LEG_1 :GetLegF1() ; break;
					  case HAND_1:GetHandF1(); break;
					  case BODY_1:GetBodyF1(); break;
						
						default:test_sta=BODY_1; break;
					}				
		}
}

/////////////////////////////////////校准模式///////////////////////////////////////////

//根据频段 校准k
uint16_t CalK(uint16_t BodyAdc,FunctionFreq Fx)
{		
	  uint16_t  short_adc,ref0_adc,ref1_adc;
	  uint16_t  calres; //校准电阻
	  uint16_t   temp_k;
          
			 short_adc = get2byte(ADCVal.Freq1_Short_AD_H,ADCVal.Freq1_Short_AD_L,L); 	  
			 ref0_adc  = get2byte(ADCVal.Freq1_Ref0_AD_H, ADCVal.Freq1_Ref0_AD_L,L); 
			 ref1_adc  = get2byte(ADCVal.Freq1_Ref1_AD_H, ADCVal.Freq1_Ref1_AD_L,L); 
			 calres    = get2byte(EquMent.FREQ1_CalRes_H, EquMent.FREQ1_CalRes_L,L); 

			 if((ref1_adc-ref0_adc)>=((BodyAdc-short_adc)*REF_RES/calres))
					{
						temp_k=ref1_adc-ref0_adc-(BodyAdc-short_adc)*REF_RES/calres;
					  temp_k=temp_k&0x7fff;
					}
			 else
			    {
						temp_k=(BodyAdc-short_adc)*REF_RES/calres - (ref1_adc-ref0_adc);
						temp_k=temp_k|0x8000;
					}
			 return temp_k;		 
}


void CalKLegF1(void)
{
			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //内短
							break;
				case C34_Init_1:
							Res_Init_All(ISIN2,ISIN3,VSEN3,VSEN2,F1);	//Z34
							break;
				case C43_Init_1:
							Res_Init_All(ISIN3,ISIN2,VSEN2,VSEN3,F1);	//Z43
							break;
				default:BIA_Pointer=Short_Init_1;break;
			}

			temp_avg=ReadAvgADC(waste_times,all_times);			//CALLge_1		
			if(temp_avg>0)
			{
						switch(BIA_Pointer-1)
						{
							case Short_Init_1: 
									Short_Fx_ADC(F1); 
									break;
							case C34_Init_1: 
									Z34_Fx_ADC(F1);		//Z34 ADC
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,L),F1);
							    Z34_Fx_RES(F1);   //Z34 RES
									break;
							case C43_Init_1: 
									Z43_Fx_ADC(F1);   //Z43 ADC						
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,H),F1);
							    Z43_Fx_RES(F1);   //Z43 RES
							
							    //先用0 K值合理性判断
							    if(judge_posture(get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L),\
																	 get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,H))==FALSE)
									{
									 CLR_Z34_Fx_RES(F1);
									 EquMent.F1CV_FLAG|=((1<<F1C_FLAG)|(1<<F1_FLASH));//外接电阻不在合法范围+写flash失败															
									}
									else //计算K并写flash
									{   
									  fk_val=CalK(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,L),F1);//计算		
										
										if(Cmd_WriteTypeByteToFlash(fk_val,F_FREQ1K_VAL_H_ADDR,1)==TRUE)//写flash
										  {EquMent.FREQ1K_VAL_H=(uint8_t)(fk_val>>8);
											 EquMent.FREQ1K_VAL_L=(uint8_t)(fk_val&0xff);} //ram 							
									}
									EquMent.F_STATUS=0x01;//已经执行完第一频校准									
									/***************************************/
									BIA_FLAG|=(1<<CalKDone); // 校准完成
									BIA_Pointer=Short_Init_1;
									EquMent.CONFIG_K&=~(1<<CK); //完成clr
									//SetDRDY(0);
									update_sta_flg();
									break;
							default:BIA_Pointer=Short_Init_1;break;									
						}							
			}	
}

void CalKHandF1(void)
{
			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //内短
							break;
				case C12_Init_1:
							Res_Init_All(ISIN0,ISIN1,VSEN1,VSEN0,F1);	//Z12
							break;
				case C21_Init_1:
							Res_Init_All(ISIN1,ISIN0,VSEN0,VSEN1,F1);	//Z21
							break;
				default:BIA_Pointer=Short_Init_1;break;
				
			}
			
			temp_avg=ReadAvgADC(waste_times,all_times);			//CAL_Hand1		
			if(temp_avg>0)
			{
						switch(BIA_Pointer-1)
						{
							case Short_Init_1: 
									Short_Fx_ADC(F1); 
									break;
							case C12_Init_1: 
									Z12_Fx_ADC(F1);		//Z12 ADC
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,L),F1);
							    Z12_Fx_RES(F1);   //Z12 RES
									break;
							case C21_Init_1: 
									Z21_Fx_ADC(F1);   //Z21 ADC						
							    temp_res=CalRes(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,H),F1);
							    Z21_Fx_RES(F1);   //Z21 RES
							
							    //先用0 K值合理性判断
							    if(judge_posture(get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L),\
																	 get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,H))==FALSE)
									{
									 CLR_Z12_Fx_RES(F1);
									 EquMent.F1CV_FLAG|=((1<<F1C_FLAG)|(1<<F1_FLASH));//外接电阻不在合法范围+写flash失败												
									}
									else //计算K并写flash
									{   
									  fk_val=CalK(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,L),F1);//计算							     
										if(Cmd_WriteTypeByteToFlash(fk_val,F_FREQ1K_VAL_H_ADDR,1)==TRUE) //写flash
										  {EquMent.FREQ1K_VAL_H=(uint8_t)(fk_val>>8);
											 EquMent.FREQ1K_VAL_L=(uint8_t)(fk_val&0xff);}
									}
									EquMent.F_STATUS=0x01;//已经执行完第一频校准
									/***************************************/
									BIA_FLAG|=(1<<CalKDone); // 校准完成
									BIA_Pointer=Short_Init_1;
									EquMent.CONFIG_K&=~(1<<CK); //完成clr
									//SetDRDY(0);
									update_sta_flg();
									break;	
							default:BIA_Pointer=Short_Init_1;break;									
						}							
			}	
}


void Parm_Rational(void)
{
	 if((BIA_FLAG>>GetRef)&0x01) //已经获得完整参考 
	 {
				switch(test_sta)  //校准状态 找到是哪频段哪部位
				{					
					case LEG_1 :
					case HAND_1:							
					case BODY_1:
								if(get2byte(EquMent.FREQ1_CalRes_H,EquMent.FREQ1_CalRes_L,L)<=K_RES_MIN ||\
									 get2byte(EquMent.FREQ1_CalRes_H,EquMent.FREQ1_CalRes_L,L)>=K_RES_MAX)
								 {
										EquMent.ERR_STATUS|=(1<<Err_Parm);EquMent.CONFIG_K&=~(1<<CK);
										//SetDRDY(0);
										update_sta_flg();
										break;
								 }		
								 
									EquMent.ERR_STATUS&=~(1<<Err_Parm);break;	
					default:test_sta=BODY_1;break;
				}		 
	 }
}

void Calibrate_K(void)
{
	 if((BIA_FLAG>>GetRef)&0x01) //已经获得完整参考
	 {
		 if((BIA_FLAG>>CalKDone)&0x01)
						return;  //校准完成 
					switch(test_sta)  //校准状态 找到是哪频段哪部位
					{
					  case LEG_1 :													
					  case BODY_1:CalKLegF1(); break;
					  case HAND_1:CalKHandF1(); break;		//2					
						
						default:test_sta=BODY_1;break;
					}					  
	 }
}



void _CS1258bug(void)
{
		Ref_Init_All(SHORT_MODE,F1);  //内短
		SendDataToAddr(ADDR_AFE_SYS,0xCE);
		Res_Init_All(ISIN2,ISIN3,VSEN3,VSEN2,F1);	//Z34
		SendDataToAddr(ADDR_AFE_SYS,0xCE);
}
