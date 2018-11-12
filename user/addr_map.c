
#include "addr_map.h"
#include <string.h>



EquipmentConfig_TypeDef EquMent; //�豸������
EquipmentInfo_TypeDef   EquInfo; //�豸��Ϣ��
UserInfo_TypeDef        UsrInfo; //�û���Ϣ��
UserBIAPart_TypeDef     UsrBIAP; //�û�BIA������
Analysis_TypeDef        Analys;  //���������
ADCVal_TypeDef          ADCVal;  //ADCֵ��
RESVal_TypeDef          RESVal;  //��ʵ������



/**
  * @brief  ��ַӳ�� �����û�����ĸߵ�ַ�͵͵�ַ�ҵ��ڲ���Ӧ�ĵ�ַ
  * @param  uint16_t user_addr
	          �û�����ߵ�ַ���͵�ַ
  * @retval �ڲ���ַ
  */
uint8_t addr_flag=0;




//���ram
void Clr_All_Block_Ram(void)
{
	memset(&EquMent,0,sizeof(EquMent));
	memset(&EquInfo,0,sizeof(EquInfo));
	memset(&UsrInfo,0,sizeof(UsrInfo));
	memset(&UsrBIAP,0,sizeof(UsrBIAP));
	memset(&Analys,0,sizeof(Analys));
	memset(&ADCVal,0,sizeof(ADCVal));
	memset(&RESVal,0,sizeof(RESVal));
}


void Clr_UserBIAPart_Ram(void)
{
		memset(&UsrBIAP,0,sizeof(UsrBIAP));
}

void Clr_Analysis_Ram(void)
{
		memset(&Analys,0,sizeof(Analys));
}

//ע��123Ƶ���ڶ� r0 r1������
void Clr_ADCVal_Ram(void)
{
		memset(&(ADCVal.Freq1_Z12_AD_H),0,sizeof(ADCVal)-12);
}

void Clr_RESVal_Ram(void)
{
		memset(&RESVal,0,sizeof(RESVal));
}


void Clr_UsrInfo_ResF1(void)
{
		//memset(&UsrInfo,0,sizeof(UsrInfo));
}




