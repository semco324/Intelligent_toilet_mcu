#include "driver_cs1258.h" 

uint32_t BIA_FLAG=0;   //����BIA�ı�־λ��FunctionBIAFlag
uint8_t  BIA_Pointer=0;//��������
 
uint8_t waste_times; //����AD����
uint8_t	all_times;   //�ܹ�AD����
uint8_t	read_times;  //��¼��AD����
uint8_t	out_times;   //��ʱ����
					
uint32_t lastADC;    //�ϴ�AD
uint32_t currentADC; //��ǰAD
uint32_t sumADC;     //AD�ۼӺ�
uint16_t temp_avg;   //��ֵAD
uint16_t temp_res;   //�������

uint16_t   fk_val;     //��Ƶ��k
//uint8_t  fk_presion;  //��Ƶ����



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
			SetFATSDAInput(FAT_SDA);	//SDA ����  	
			delay_us(5);
}

/*��125x ����1 byte */
void send1byte(uint8_t data)
{
			uint8_t i;
			SetFATSDAOutput(FAT_SDA);//SDA�������
			for(i=0;i<8;i++)
			{
						if(data&0x80)          //MSB
							 SetFATSDA(FAT_SDA); //SDA=1
						else
							 ClrFATSDA(FAT_SDA); //SDA=0
						delay_us(5);
						SetFATSCL(FAT_SCL);    //SCL=1  д
						delay_us(5);
						ClrFATSCL(FAT_SCL);    //SCL=0
						data<<=1;              //data����1λ
						delay_us(5);           //cs125x ������				
			}
			delay_us(10); 		
}

/*��125x 1 byte */
uint8_t read1byte(void)
{
			uint8_t i,data=0;
			SetFATSDAInput(FAT_SDA);	//SDA ����  	
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

/*��addr��ַдdata����*/
void SendDataToAddr(uint8_t addr,uint8_t data)
{
			addr=addr|0x80; //��ַ��7λ��Ϊ1������д
			startCS125x();  //���Ϳ�ʼ
			send1byte(addr);//���͵�ַ
			send1byte(data);//��������
			stopCS125x();   //���ͽ���	
}

/*��addr��ַ�е�����*/
uint8_t ReadAddrData(uint8_t addr)
{
			uint8_t temp;
			startCS125x();  //���Ϳ�ʼ
			send1byte(addr);//���͵�ַ
			temp=read1byte();//������
			stopCS125x();   //���ͽ���	
			return	temp;	
}
/*��ADC���*/
uint32_t ReadADC(void)
{
			uint8_t tempH,tempM,tempL;
			startCS125x();  //���Ϳ�ʼ
			send1byte(ADDR_AFE_ADO);//���͵�ַADO
		
			tempH=read1byte()^0x80;
			tempM=read1byte();
			tempL=read1byte();
		
			stopCS125x();   //���ͽ���	
			return   ((tempH<<16)|(tempM<<8)|(tempL));
}


/**
  * @brief  ��λCS125x ͨѶ��
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
  * @brief  �ϵ縴λCS125x 
  * @param  None
  * @retval None
  */ 
void	Reset_CS125x(void)
{
			uint8_t temp,i=0;
			
			Rest_Communicate_Interface();         //1.��λͨѶ��
			
			while(1)
			{
					SendDataToAddr(0xEA,0x96);        //2.���͸�λָ�� 0xEA 0x96
					delay_ms(5);         //5ms
					i++;
		
					temp=ReadAddrData(ADDR_AFE_ADS);	//3.��ADS�Ĵ������ж��Ƿ�λ�ɹ�
					if(((temp&0x40)==0x40)||(i>4))
						break;
			}		
			SendDataToAddr(ADDR_AFE_TEST,0x5A);	  //4.�������ģʽ ADDR_AFE_TEST д0x5A	
			SendDataToAddr(ADDR_AFE_LDOT,0xFF);	  //5.triming ADDR_AFE_LDOT д0xFF		
			SendDataToAddr(ADDR_AFE_OSCT,0xFF);   //6.triming ADDR_AFE_OSCT д0xFF		
			SendDataToAddr(ADDR_AFE_CONFIG,0xBD);	//7.open sin ADDR_AFE_CONFIG д0xBF--->0xBD		
}
	
/**
  * @brief  ��ʼ��CS125x ��ʼ����ɵ���BIM֮��Ҫ��50msʱ����
  * @param  None
  * @retval None
  */ 	
void InitCS125x(void)
{
			
			SetFATSDAOutput(FAT_SDA);//���� FAT SDA���		
	
			SendDataToAddr(ADDR_AFE_ADC0,0x00);//ADC0 		
			SendDataToAddr(ADDR_AFE_ADC1,0x40);//ADC1 0x40  40hz 		
			SendDataToAddr(ADDR_AFE_ADC2,0x00);//ADC2 			
			SendDataToAddr(ADDR_AFE_ADC3,0x03);	//ADC3 			
			SendDataToAddr(ADDR_AFE_ADC4,0x42);	//ADC4 0x42  ���ο�=�ⲿREFP,���ο�=�ⲿREFN bimʱ��������2.8V		
			SendDataToAddr(ADDR_AFE_ADC5,0x00);	//ADC5 
			SendDataToAddr(ADDR_AFE_SYS,0xCE);	//SYS  0xCF  BIMģʽ,��ͨģʽ,VREF��LDO��ADC��BIM�ر�
			delay_ms(50);         //50ms	
}

/**
  * @brief  SCL�ڱ��ֱ��ָߵ�ƽ����172uS,����PowerDownģʽ
            power down ֮ǰҪ�ص�LDO��ADC��BIM
            power down ����֮����ʹ��LDO��ADC����ʱ50ms����ʹ��BIM
            SCL����180us 
  * @param  None
  * @retval None
  */
void PowerDownCS125x(void)
{
			//power down ֮ǰҪ�ص�LDO��ADC��BIM
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
1Ƶ	0 0     1    2     2
2Ƶ	1 3     4    5     5
3Ƶ	2 6     7    8     8
����M_PART=11ʱ����,Ĭ�ϰ�����10����ͬһ״̬(8�缫ȫ��)
�ҵ����ĸ���λ�ļ���Ƶ��*/
int8_t tab[4][5]={ -1,0,1,2,3,\
								    0,0,1,2,2,\
									  1,3,4,5,5,\
									  2,6,7,8,8};

uint8_t find_tab(uint8_t bodypart,uint8_t f_cnt)
{
	uint8_t i,p1,p2;
		for(i=1;i<5;i++)  //��
			{
					if(tab[0][i]==bodypart)
					{p1=i;break;}					
			}
		for(i=1;i<4;i++)  //��
			{
					if(tab[i][0]==f_cnt)
					{p2=i;break;}
			}
		return 	tab[p2][p1];  //����0~8 9��״̬
}

uint8_t freq[3]={0,0,0}; //�������Ƶ�Σ�ע��ֻʹ�õ�4bit����4bitΪ0
//freq[0] :��1Ƶ��Ƶ��
//freq[1] :��2Ƶ��Ƶ��
//freq[2] :��3Ƶ��Ƶ��
uint8_t f_cnt;   //��¼Ƶ�θ��� 
//0:1��Ƶ�� 
//1:2��Ƶ�� 
//2:3��Ƶ�� 
uint8_t test_sta;//����״̬
//0:leg_1 1:hand_1 2:body_1
//3:leg_2 4:hand_2 5:body_2
//6:leg_3 7:hand_3 8:body_3

//ͨ��EquMent->FREQ EquMent->STATUS
//�õ�Ƶ�θ��� f_cnt��Ƶ������ freq[f_cnt]��������λ��Ϣtest_sta
//parm EquMent->M_PART  EquMent->FREQ 
void tab_ref_test(void)
{
	    uint8_t i;
	   //Ƶ�ʸ�����Ƶ�ν���
			f_cnt=0;
	
			for(i=0;i<7;i++)  //ֻ��bit6
			{
				if((EquMent.FREQ>>i)&0x01)
				{
					 if(i==6){ freq[f_cnt]=(3<<0);f_cnt++;break; }//Ĭ��50khz	 д0x40 (bit6д1)
					 
					 if(i!=2)
					 {  		//�޳�25k bit2
							freq[f_cnt]=(i<<0);//ֻ��i��Ϊ2,����freq�и���,Ҳ�����޳�25k bit2
							f_cnt++;
					 }
				}
			  if(f_cnt==3)break;
			}	
			f_cnt--; 	
			
			
			if(!(EquMent.FREQ&0x07f)) //bit7д1��д0x80
			{
			freq[0]=(3<<0);//Ĭ��50khz
			f_cnt=0;
			}		
			
			//����Э�����EquMent->FREQ
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
		//������λ״̬���� 
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

  * @retval bim0_temp ������Ҫд��BIM0 reg val
						bim1_temp ������Ҫд��BIM1 reg val
  */
                  
void BIMRegInit(void)
{
	uint8_t i,bim1_temp=0; //init =0; 
	 ////////////////////////////////BIM1//////////////////////////////////////////
	  if(BIM_Init.BIM1_MIXorFWR==MIX)
			bim1_temp|=0x40;   //mix or fwr  fwr=0
		
		switch(BIM_Init.BIM_Mode)     //BIMģʽѡ��
		{
			case TEST_MODE:bim1_temp&=~(3<<4);break;	                  //bit5 4 clr 				  
			case RES0_MODE:bim1_temp&=~(1<<5);bim1_temp|=(1<<4);break;	//bit5 clr  bit4 set 
			case RES1_MODE:bim1_temp|=(1<<5); bim1_temp&=~(1<<4);break; //bit5 set  bit4 clr  
			case SHORT_MODE:bim1_temp|=(3<<4);break;		                //bit5 bit4 set	  
      default:       bim1_temp&=~(3<<4);break;				            //default TEST_MODE
		}
		
			for(i=0;i<(f_cnt+1);i++)  //�õ�BIM1 val
			{			
			 BIM_Init.BIM1[i]=bim1_temp|freq[i];
			}	
	 ///////////////////////////////BIM0////////////////////////////////////////////		
  if(BIM_Init.HUMANorREF)		//HUMAN=1
	{
	      uint8_t bim0_temp=0;
				switch(BIM_Init.BIM0_ISINO)  //�������
				{ 
					case ISIN0:bim0_temp&=~(3<<6);break;	                  //bit7 6 clr 			
					case ISIN1:bim0_temp&=~(1<<7);bim0_temp|=(1<<6);break;	//bit7 clr  bit 6 set		
					case ISIN2:bim0_temp|=(1<<7); bim0_temp&=~(1<<6);break;	//bit7 set  bit 6 clr	
					case ISIN3:bim0_temp|=(3<<6);break;	                    //bit7 6 set		
					default:   bim0_temp|=(1<<7); bim0_temp&=~(1<<6); break; //default ISIN2
				}
				switch(BIM_Init.BIM0_ISINI)  //��������
				{
					case ISIN0:bim0_temp&=~(3<<4);break;	                  //bit5 4 clr 			
					case ISIN1:bim0_temp&=~(1<<5);bim0_temp|=(1<<4);break;	//bit5 clr  bit 4 set		
					case ISIN2:bim0_temp|=(1<<5); bim0_temp&=~(1<<4);break;	//bit5 set  bit 4 clr	
					case ISIN3:bim0_temp|=(3<<4);break;	                    //bit5 4 set		
					default:   bim0_temp|=(3<<4);break;                     //default ISIN3
				}	
				switch(BIM_Init.BIM0_VSENP)  //��ѹ����
				{
					case VSEN0:bim0_temp&=~(3<<2);break;	                  //bit3 2 clr 			
					case VSEN1:bim0_temp&=~(1<<3);bim0_temp|=(1<<2);break;	//bit3 clr  bit 2 set		
					case VSEN2:bim0_temp|=(1<<3); bim0_temp&=~(1<<2);break;	//bit3 set  bit 2 clr	
					case VSEN3:bim0_temp|=(3<<2);break;	                    //bit3 2 set		
					default:   bim0_temp|=(3<<2);break;                     //default VSEN3
				}		
				switch(BIM_Init.BIM0_VSENN)  //��ѹ����
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
  * @brief  BIM �Ĵ�������			
  * @param  
  * @retval 
  */
void BIMRegCfg(FunctionFreq Fx)
{
	    //����BIM0 ѡ��ͨ��	
	    if(BIM_Init.HUMANorREF) //HUMAN=1
			        SendDataToAddr(ADDR_AFE_BIM0,BIM_Init.BIM0); 
			
	    //����BIM1  �����ڶ̡�ref���ⲿ;Ƶ��
			SendDataToAddr(ADDR_AFE_BIM1,BIM_Init.BIM1[Fx]);		
			
			//SYS д0xCF ��BIM
			SendDataToAddr(ADDR_AFE_SYS,0xCF);
}

/**
  * @brief  ��ʼ������
  * @param  
  * @retval 
  */
void Init_Parm1(uint8_t w,uint8_t a)
{
			waste_times=w;    //����AD����
	    all_times=a;      //�ܹ�AD����
	    read_times=0;     //��¼��AD����
			out_times=0;      //��ʱ����
	
			lastADC=0;       //�ϴ�AD
			currentADC=0;    //��ǰAD
			sumADC=0;	       //AD�ۼӺ�
			temp_avg=0;      //AD��ֵ
}
/**
  * @brief  ����ȶ�ADC��ֵ
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
//		 TIM_Cmd(TIM2, ENABLE);	//�򿪶�ʱ��5ms
	  //waiting for ready
		wait_out=0;
	  while(GetFATSDAInputStatus(FAT_SDA)==0)  //SDA==0
		 {
		    if(wait_out>10) //100ms
				{
//					BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);
//          TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//	        TIM_Cmd(TIM2, DISABLE);	//�رն�ʱ��
					SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS д0xCE �ر�BIM		
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
//	        TIM_Cmd(TIM2, DISABLE);	//�رն�ʱ��
					SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS д0xCE �ر�BIM		
					BIA_Pointer--;  //next 
					uint8_t tmpcheck=0xf1;
					USART_SendData(USART2, tmpcheck);
					while(1);
					return 0;
				}
		 }
//	  BASIC_TIM_APBxClock_FUN(RCC_APB1Periph_TIM2, DISABLE);
//    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
//	  TIM_Cmd(TIM2, DISABLE);	//�رն�ʱ��
		 
	  currentADC=ReadADC();                     //read AD
	  read_times++;    
		
		 //��ʱ����
	  if(out_times++>100)  
		{
				SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS д0xCE �ر�BIM		
			
				BIA_FLAG|=(1<<BErr_stb);  // set ���ȶ����� ��ʱ
			  BIA_Pointer=Short_Init_1;
			  EquMent.ERR_STATUS|=(1<<Err_Stb);      //ram
			  //SetDRDY(0);
				update_sta_flg();
			
				////////////����ģʽ��///////////////		
				if(((EquMent.STATUS>>ON_OFF)&0x01)) //����
				{			
				BIA_FLAG|=(1<<GetBodyDone);// �������			
				EquMent.STATUS&=~(1<<ON_OFF); //���clr				
				}		 
				////////////У׼ģʽ��///////////////
			 else                                   //У׼
			  {				
				BIA_FLAG|=(1<<CalKDone);  // У׼���				
				EquMent.CONFIG_K&=~(1<<CK);//���clr			
			  }					 
				return 0;
		}

		if(read_times>temp)       //����waste_times��
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
								 sumADC/=(all_times-waste_times);	//���ֵ			            
								 SendDataToAddr(ADDR_AFE_SYS,0xCE); //SYS д0xCE �ر�BIM		
//							 BIA_FLAG&=~(1<<Enable_init);  //�����ʼ��
								 BIA_Pointer++;  //next 
								return sumADC>>8;
							}
					 }
				 else
					 {		
						 if(!(read_times==(temp+1)))	//temp+1�β�clr	read_times		 
						 {read_times=0;} //CLR ��AD����							 
						  sumADC=0;      //CLR AD��
					 }
		}	
	}
}



//�ڶ� //Ref0 //Ref1
void Ref_Init_All(FunctionBimMode BimMode,FunctionFreq Fx)
{
			BIM_Init.BIM1_MIXorFWR  =FWR;          //FWR
			BIM_Init.BIM_Mode       =BimMode;      //�ڶ� //Ref0 //Ref1
			BIM_Init.HUMANorREF     =REF;          //�ο� 	
			BIMRegInit();                          //get BIM1 reg val

			BIMRegCfg(Fx);              //�������
			Init_Parm1(FILTER_AD_TIMES,RES_AD_TIMES); //��ʼ���ڶ��˲������������	 
}

//Z34 //Z43 //Z12 //Z21 //Z13 //Z14 //Z23 //Z24
void Res_Init_All(FunctionISIN ISINO,FunctionISIN ISINI,FunctionVSEN VSENP,FunctionVSEN VSENN,FunctionFreq Fx)
{
			BIM_Init.BIM1_MIXorFWR  =FWR;           //FWR
			BIM_Init.BIM_Mode       =TEST_MODE;     //TEST
			BIM_Init.HUMANorREF     =HUMAN;         //HUMAN
	
			BIM_Init.BIM0_ISINO     =ISINO;        //����O--->I
			BIM_Init.BIM0_ISINI     =ISINI;	       //��ѹN--->P
	    BIM_Init.BIM0_VSENP     =VSENP;
	    BIM_Init.BIM0_VSENN     =VSENN;	
	
			BIMRegInit();                    //get BIM0 BIM1 reg val

			BIMRegCfg(Fx);     						   //HUMANģʽ 			
			Init_Parm1(FILTER_AD_TIMES,BODY_AD_TIMES); //��ʼ��HUMAN�˲������������	 
}


//�����...= =
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

//������Ƿ����ظ��Ĳ���������
//1Ƶ �ο�����
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

//���16bit �ĸ�8 ��8 ��0xab
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

//���  �� a:0xab b:0xcd
//x=H return 0xac00
//x=L return 0x00bd
uint16_t get2byte(uint32_t a,uint32_t b,XHL X)
{
	if(X==H)//��ϸ�8bit
		return   ((a&0xFF00)|((b&0xFF00)>>8));
	else if(X==L) //��ϵ�8bit
		return    (((a&0xFF)<<8)|(b&0xFF));
	return 0;
}


//1Ƶ �ο�������
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
														
					BIA_Pointer=Short_Init_1;        //1Ƶ  reset pointer
					BIA_FLAG|=(1<<GetRef);           //1Ƶ  �Ѿ���������ο�	
					EquMent.Z_STATUS|=(1<<Z_REDY);  //1Ƶ set ģ�������־	ram									
									break;
					
					default:BIA_Pointer=Short_Init_1;break;
			}
}




/**
  * @brief  ��òο�
						1������BIM1 2��дBIM1 reg�ͳ�ʼ���˲����� 3�����ȶ�AD
  * @param  
  * @retval 
  */
void GetRef01ADC(FunctionFreq Fx)
{						
		if((BIA_FLAG>>GetRef) & 0x01) 			
			 return;    //�Ѿ���������ο� return
			
			EquMent.Z_STATUS&=~(1<<Z_REDY); //clr ģ�������־
			EquMent.CONFIG_K&=~(1<<CK);    //clr ����У׼ģʽ
			EquMent.STATUS&=~(1<<ON_OFF);  //clr ����ģʽ
			
			Ref_F1();//1Ƶ
		
		temp_avg=ReadAvgADC(waste_times,all_times);					//�ο�
		if(temp_avg>0)
			{
					Ref_F1_ADC();//1Ƶ
			}	
}

//123Ƶ �ڶ� ������ 
void Short_Fx_ADC(FunctionFreq Fx)
{

		ADCVal.Freq1_Short_AD_H=get1byte(temp_avg,HL);
		ADCVal.Freq1_Short_AD_L=get1byte(temp_avg,LL);
		return;
}


//123Ƶ Z34 ADC������
void Z34_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z34_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z34_AD_L=get1byte(temp_avg,LL);
		return;	
}

//123Ƶ Z12 ADC������
void Z12_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z12_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z12_AD_L=get1byte(temp_avg,LL);
	return;		
}

//123Ƶ Z13 ADC������
void Z13_Fx_ADC(FunctionFreq Fx)	
{
    ADCVal.Freq1_Z13_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z13_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S13);//ram 13 ���
	return;	
}

//123Ƶ Z14 ADC������
void Z14_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z14_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z14_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S14); //ram 14 ���
	return;	
}

//123Ƶ Z23 ADC������
void Z23_Fx_ADC(FunctionFreq Fx)	
{
    ADCVal.Freq1_Z23_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z23_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S23); //ram 23 ���
	return;	
}

//123Ƶ Z24 ADC������
void Z24_Fx_ADC(FunctionFreq Fx)	
{

    ADCVal.Freq1_Z24_AD_H=get1byte(temp_avg,HL);
    ADCVal.Freq1_Z24_AD_L=get1byte(temp_avg,LL);
		EquMent.Z_STATUS|=(1<<Z_S24); //ram 24 ���
	return;		
}


//123Ƶ Z43 ADC������  ������ڸ�8λ
void Z43_Fx_ADC(FunctionFreq Fx)	
{      
    ADCVal.Freq1_Z34_AD_H|=get1byte(temp_avg,HH);
    ADCVal.Freq1_Z34_AD_L|=get1byte(temp_avg,LH);
		EquMent.Z_STATUS|=(1<<Z_S34);//ram 34 ���			
	return;
}

//123Ƶ Z21 ADC������  ������ڸ�8λ
void Z21_Fx_ADC(FunctionFreq Fx)	
{     	
    ADCVal.Freq1_Z12_AD_H|=get1byte(temp_avg,HH);
    ADCVal.Freq1_Z12_AD_L|=get1byte(temp_avg,LH);	
		EquMent.Z_STATUS|=(1<<Z_S12); //1Ƶ ram 12 ��� 		
	return;
}




//123Ƶ Z34 RES������
void Z34_Fx_RES(FunctionFreq Fx)
{			
    RESVal.F1_Z34_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z34_RES_L=get1byte(temp_res,LL);
		return;
}

//123Ƶ Z43 RES������
void Z43_Fx_RES(FunctionFreq Fx)
{
    RESVal.F1_Z34_RES_H|=get1byte(temp_res,HH);
    RESVal.F1_Z34_RES_L|=get1byte(temp_res,LH);	
	return;
}

//123Ƶ Z12 RES������
void Z12_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z12_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z12_RES_L=get1byte(temp_res,LL);
		return;
}
//123Ƶ Z21 RES������
void Z21_Fx_RES(FunctionFreq Fx)
{
    RESVal.F1_Z12_RES_H|=get1byte(temp_res,HH);
    RESVal.F1_Z12_RES_L|=get1byte(temp_res,LH);	
	return;
}

//123Ƶ Z13 RES������
void Z13_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z13_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z13_RES_L=get1byte(temp_res,LL);
		return;
}

//123Ƶ Z14 RES������
void Z14_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z14_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z14_RES_L=get1byte(temp_res,LL);
		return;
}
//123Ƶ Z23 RES������
void Z23_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z23_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z23_RES_L=get1byte(temp_res,LL);
		return;
}
//123Ƶ Z24 RES������
void Z24_Fx_RES(FunctionFreq Fx)
{				
    RESVal.F1_Z24_RES_H=get1byte(temp_res,HL);
    RESVal.F1_Z24_RES_L=get1byte(temp_res,LL);
		return;
}

//����123Ƶ �����ֵ���
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

//վ���쳣�ж�
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
			
			////////////����ģʽ��///////////////					
			if(((EquMent.STATUS>>ON_OFF)&0x01)) //����
			{							
				EquMent.STATUS&=~(1<<ON_OFF); //���clr
				//SetDRDY(0);		
        update_sta_flg();				
			}				
			////////////У׼ģʽ��///////////////
		 else if(((EquMent.CONFIG_K>>CK)&0x01)) //У׼
			{
        EquMent.CONFIG_K&=~(1<<CK);//���clr	
				//SetDRDY(0);		
				update_sta_flg();								
			}		
		
		 return FALSE;
		}  
		 return TRUE;
}


//����Ƶ�� �������
uint16_t CalRes(uint16_t BodyAdc,FunctionFreq Fx)
{		
	  uint16_t  short_adc,ref0_adc,ref1_adc;
	  uint16_t  temp_res;
	           
			 short_adc=get2byte(ADCVal.Freq1_Short_AD_H,ADCVal.Freq1_Short_AD_L,L); 	  
			 ref0_adc =get2byte(ADCVal.Freq1_Ref0_AD_H, ADCVal.Freq1_Ref0_AD_L,L); 
			 ref1_adc =get2byte(ADCVal.Freq1_Ref1_AD_H, ADCVal.Freq1_Ref1_AD_L,L); 
			 fk_val   =get2byte(EquMent.FREQ1K_VAL_H,EquMent.FREQ1K_VAL_L,L);

		if(((EquMent.CONFIG_K>>CK)&0x01)) //У׼ʱ��k=0����
		      {fk_val=0;}
	
     BIA_FLAG&=~(1<<BErr_Short);// clr �ڶ̳���
	   BIA_FLAG&=~(1<<BErr_Hight);// clr ��������
	   BIA_FLAG&=~(1<<BErr_Low);  // clr ��������
		
		 EquMent.ERR_STATUS&=~(1<<Err_Short);      //ram
		 EquMent.ERR_STATUS&=~(1<<Err_lowlimit);   //ram
		 EquMent.ERR_STATUS&=~(1<<Err_hightlimit); //ram
			
	   if(BodyAdc<=short_adc)  //С���ڶ�
		 {
							////////////����ģʽ��///////////////		
							if(((EquMent.STATUS>>ON_OFF)&0x01)) //����
							{
							BIA_FLAG|=(1<<BErr_Short);  // set �ڶ̳���
							BIA_FLAG|=(1<<GetBodyDone);// �������
							BIA_Pointer=Short_Init_1;
						 
							EquMent.ERR_STATUS|=(1<<Err_Short);      //ram
							EquMent.STATUS&=~(1<<ON_OFF); //���clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;
							}		 
							////////////У׼ģʽ��///////////////
						 else                                   //У׼
						 {
							BIA_FLAG|=(1<<BErr_Short);  // set �ڶ̳���
							BIA_FLAG|=(1<<CalKDone);  // У׼���
							BIA_Pointer=Short_Init_1;
						 
							EquMent.ERR_STATUS|=(1<<Err_Short);      //ram
							EquMent.CONFIG_K&=~(1<<CK);//���clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;
						 }
		 }
		 else
		 {
			 
			   if(fk_val>>15){ //��
					 fk_val=fk_val-0x8000;
				   temp_res=(BodyAdc-short_adc)*REF_RES/(ref1_adc-ref0_adc+fk_val);
				 }
				 else  //��
				 {
				   temp_res=(BodyAdc-short_adc)*REF_RES/(ref1_adc-ref0_adc-fk_val);
				 }
				 
				 if(temp_res>=RES_MAX)  //��������
				 {
							////////////����ģʽ��///////////////		
							if(((EquMent.STATUS>>ON_OFF)&0x01)) //����
							{
							BIA_FLAG|=(1<<BErr_Hight);  // set ���޳���
							BIA_FLAG|=(1<<GetBodyDone);// �������
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_hightlimit); //ram
							EquMent.STATUS&=~(1<<ON_OFF); //���clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;					
							}
							////////////У׼ģʽ��///////////////
						 else                                   //У׼
							{
							BIA_FLAG|=(1<<BErr_Hight);  // set ���޳���
							BIA_FLAG|=(1<<CalKDone);// У׼���
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_hightlimit); //ram
							EquMent.CONFIG_K&=~(1<<CK);//���clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;			 
							}
				 }
				 if(temp_res<=RES_MIN)  //С������
				 {
							////////////����ģʽ��///////////////	
							if(((EquMent.STATUS>>ON_OFF)&0x01)) //����
							{
							BIA_FLAG|=(1<<BErr_Low);  // set ���޳���
							BIA_FLAG|=(1<<GetBodyDone);// �������
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_lowlimit);      //ram
							EquMent.STATUS&=~(1<<ON_OFF); //���clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;	
							}					
							////////////У׼ģʽ��///////////////
							else
							{
							BIA_FLAG|=(1<<BErr_Low);  // set ���޳���
							BIA_FLAG|=(1<<CalKDone);// У׼���
							BIA_Pointer=Short_Init_1;
							 
							EquMent.ERR_STATUS|=(1<<Err_lowlimit);      //ram
							EquMent.CONFIG_K&=~(1<<CK);//���clr
							//SetDRDY(0);
							update_sta_flg();
							return 0;	
							}					
				 }
				 return temp_res;
		 }		 
}


//Leg_1  1Ƶ
void GetLegF1(void)
{
			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //�ڶ�
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
							
							    //�������ж�
							    if(judge_posture(get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L),\
																	 get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,H)) == FALSE)
									{CLR_Z34_Fx_RES(F1);}
									  
									//replace-----------------�Ӵ�����
									mov_res(LEG_1);       //�����
									//replace-----------------
									
									EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵ��			
									
									BIA_FLAG|=(1<<GetBodyDone);// �������
									BIA_Pointer=Short_Init_1;																		
									EquMent.STATUS&=~(1<<ON_OFF); //���clr
									//SetDRDY(0);
									update_sta_flg();
									break;
							default:BIA_Pointer=Short_Init_1;break;									
						}							
			}	
}


////////////////////////////////////////////////////
//Hand_1 1Ƶ
void GetHandF1(void)
{

			switch(BIA_Pointer)
			{
				case Short_Init_1:
							Ref_Init_All(SHORT_MODE,F1);  //�ڶ�
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
							
							    //�������ж�
							    if(judge_posture(get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L),\
																	 get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,H))==FALSE)
									{CLR_Z12_Fx_RES(F1);}
									
									//replace-----------------�Ӵ����� 
									mov_res(HAND_1);       //�����
									//replace-----------------

									EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵ��

									BIA_FLAG|=(1<<GetBodyDone);// �������
									BIA_Pointer=Short_Init_1;	
									EquMent.STATUS&=~(1<<ON_OFF); //���clr
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
							Ref_Init_All(SHORT_MODE,F1);  //�ڶ�
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
							    //�������ж�
							    if(judge_posture(get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L),\
																	 get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,H))==FALSE)
									{
										CLR_Z34_Fx_RES(F1);EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵ��	
									}
									//replace-----------------�Ӵ�����
									mov_res(LEG_1);       //�����
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
							    //�������ж�
							    if(judge_posture(get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L),\
																	 get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,H))==FALSE)
									{CLR_Z12_Fx_RES(F1);EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵ��	
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
					
							    Body_Part_RES(F1); //��������ֵ���
							
									//replace-----------------�Ӵ�����
									mov_res(BODY_1);       //�����
									//replace-----------------

							    EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵ��	
							
									BIA_FLAG|=(1<<GetBodyDone);// �������
									BIA_Pointer=Short_Init_1;																
									EquMent.STATUS&=~(1<<ON_OFF); //���clr
									//SetDRDY(0);
									update_sta_flg();
									break;
							default:BIA_Pointer=Short_Init_1;break;
							}							
			}							
}


/**
  * @brief  ����������
						1������BIM1 2��дBIM1 reg�ͳ�ʼ���˲����� 3�����ȶ�AD
  * @param  
  * @retval 
  */
void GetBodyRes(void)
{
		if((BIA_FLAG>>GetRef)&0x01) //�Ѿ���������ο�
		{
					if((BIA_FLAG>>GetBodyDone)&0x01)
						return;  //������� 
					
					switch(test_sta)  //����״̬ �ҵ�����Ƶ���Ĳ�λ
					{
					  case LEG_1 :GetLegF1() ; break;
					  case HAND_1:GetHandF1(); break;
					  case BODY_1:GetBodyF1(); break;
						
						default:test_sta=BODY_1; break;
					}				
		}
}

/////////////////////////////////////У׼ģʽ///////////////////////////////////////////

//����Ƶ�� У׼k
uint16_t CalK(uint16_t BodyAdc,FunctionFreq Fx)
{		
	  uint16_t  short_adc,ref0_adc,ref1_adc;
	  uint16_t  calres; //У׼����
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
							Ref_Init_All(SHORT_MODE,F1);  //�ڶ�
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
							
							    //����0 Kֵ�������ж�
							    if(judge_posture(get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,L),\
																	 get2byte(RESVal.F1_Z34_RES_H,RESVal.F1_Z34_RES_L,H))==FALSE)
									{
									 CLR_Z34_Fx_RES(F1);
									 EquMent.F1CV_FLAG|=((1<<F1C_FLAG)|(1<<F1_FLASH));//��ӵ��費�ںϷ���Χ+дflashʧ��															
									}
									else //����K��дflash
									{   
									  fk_val=CalK(get2byte(ADCVal.Freq1_Z34_AD_H,ADCVal.Freq1_Z34_AD_L,L),F1);//����		
										
										if(Cmd_WriteTypeByteToFlash(fk_val,F_FREQ1K_VAL_H_ADDR,1)==TRUE)//дflash
										  {EquMent.FREQ1K_VAL_H=(uint8_t)(fk_val>>8);
											 EquMent.FREQ1K_VAL_L=(uint8_t)(fk_val&0xff);} //ram 							
									}
									EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵУ׼									
									/***************************************/
									BIA_FLAG|=(1<<CalKDone); // У׼���
									BIA_Pointer=Short_Init_1;
									EquMent.CONFIG_K&=~(1<<CK); //���clr
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
							Ref_Init_All(SHORT_MODE,F1);  //�ڶ�
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
							
							    //����0 Kֵ�������ж�
							    if(judge_posture(get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,L),\
																	 get2byte(RESVal.F1_Z12_RES_H,RESVal.F1_Z12_RES_L,H))==FALSE)
									{
									 CLR_Z12_Fx_RES(F1);
									 EquMent.F1CV_FLAG|=((1<<F1C_FLAG)|(1<<F1_FLASH));//��ӵ��費�ںϷ���Χ+дflashʧ��												
									}
									else //����K��дflash
									{   
									  fk_val=CalK(get2byte(ADCVal.Freq1_Z12_AD_H,ADCVal.Freq1_Z12_AD_L,L),F1);//����							     
										if(Cmd_WriteTypeByteToFlash(fk_val,F_FREQ1K_VAL_H_ADDR,1)==TRUE) //дflash
										  {EquMent.FREQ1K_VAL_H=(uint8_t)(fk_val>>8);
											 EquMent.FREQ1K_VAL_L=(uint8_t)(fk_val&0xff);}
									}
									EquMent.F_STATUS=0x01;//�Ѿ�ִ�����һƵУ׼
									/***************************************/
									BIA_FLAG|=(1<<CalKDone); // У׼���
									BIA_Pointer=Short_Init_1;
									EquMent.CONFIG_K&=~(1<<CK); //���clr
									//SetDRDY(0);
									update_sta_flg();
									break;	
							default:BIA_Pointer=Short_Init_1;break;									
						}							
			}	
}


void Parm_Rational(void)
{
	 if((BIA_FLAG>>GetRef)&0x01) //�Ѿ���������ο� 
	 {
				switch(test_sta)  //У׼״̬ �ҵ�����Ƶ���Ĳ�λ
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
	 if((BIA_FLAG>>GetRef)&0x01) //�Ѿ���������ο�
	 {
		 if((BIA_FLAG>>CalKDone)&0x01)
						return;  //У׼��� 
					switch(test_sta)  //У׼״̬ �ҵ�����Ƶ���Ĳ�λ
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
		Ref_Init_All(SHORT_MODE,F1);  //�ڶ�
		SendDataToAddr(ADDR_AFE_SYS,0xCE);
		Res_Init_All(ISIN2,ISIN3,VSEN3,VSEN2,F1);	//Z34
		SendDataToAddr(ADDR_AFE_SYS,0xCE);
}
