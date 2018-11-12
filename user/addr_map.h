#ifndef __ADDR_MAP_H
#define __ADDR_MAP_H

#include <stdint.h>                   /* Include standard types */

//设备配置区rw    23(64)  0x1000
typedef struct
{
	uint8_t STATUS;       //
	uint8_t FREQ;
	uint8_t M_PART;
	
	uint8_t FREQ1_CalRes_H;
	uint8_t FREQ1_CalRes_L;	 
	
	uint8_t HR_CAL_VAL;
	
	uint8_t CONFIG_K;
	uint8_t F1CV_FLAG;	

	uint8_t FREQ1K_VAL_H;//r
	uint8_t FREQ1K_VAL_L;

	uint8_t Z_STATUS;  //r		
	uint8_t ERR_STATUS;//r		
	uint8_t C_STATUS;  //r		
	uint8_t F_STATUS;  //r		
}EquipmentConfig_TypeDef;



//设备信息区 r  20(24)  0x1040
typedef struct
{
//	uint8_t MID_HH;
//	uint8_t MID_H;
//	uint8_t MID_L;
//	uint8_t MID_LL;
//	uint8_t SID_HH;
//	uint8_t SID_H;
//	uint8_t SID_L;
//	uint8_t SID_LL;
//	
//	uint8_t S_HAND;      // 单频手部算法 1048
//	uint8_t SA_HAND;     // 单频手部运动员算法
//	uint8_t S_LEG;       // 单频脚步算法
//	uint8_t SA_LEG;      // 单频脚步运动员算法
//	uint8_t S_BODY;      // 单频全身算法
//	uint8_t SA_BODY;     // 单频全身运动员算法
//	uint8_t M_HEART;     // 心率算法
//	uint8_t HW_VER;      // 硬件版本号
//	
//	uint8_t APROM_H;     
//	uint8_t APROM_L;
//	uint8_t BOOT_VER;
//	uint8_t CHECK_SUM; 
  
  uint8_t SW_VER_H;
  uint8_t SW_VER_L;
  uint8_t HW_VER_H;
  uint8_t HW_VER_L;	
	
}EquipmentInfo_TypeDef;

//用户信息区rw      77-18(128)  0x1058
typedef struct
{
	uint8_t WTH;
	uint8_t WTL;
	uint8_t HT;
	uint8_t SAGE;
//	uint8_t MODE;
}UserInfo_TypeDef;


//用户测试BIA区 r   72+2(最后2个)(128) 0x10D8
typedef struct
{
	//参与成分计算的电阻
	uint16_t Freq1_All_Res_H;  //0
	uint16_t Freq1_All_Res_L;  //1
	uint16_t Freq1_Z1_Res_H;   //2
	uint16_t Freq1_Z1_Res_L;	 //3
	uint16_t Freq1_Z2_Res_H;   //4
	uint16_t Freq1_Z2_Res_L;	 //5
	uint16_t Freq1_Z3_Res_H;   //6
	uint16_t Freq1_Z3_Res_L;	 //7
	uint16_t Freq1_Z4_Res_H;   //8
	uint16_t Freq1_Z4_Res_L;	 //9
	uint16_t Freq1_Z5_Res_H;   //a
	uint16_t Freq1_Z5_Res_L;   //b
	
	//这部分用户不可见
	uint16_t Freq1_Z34_Res_H;  //24
	uint16_t Freq1_Z34_Res_L;	 //25
	uint16_t Freq1_Z12_Res_H;  //26
	uint16_t Freq1_Z12_Res_L;	 //27
	uint16_t Freq1_Z13_Res_H;  //28
	uint16_t Freq1_Z13_Res_L;	 //29
	uint16_t Freq1_Z14_Res_H;  //3a
	uint16_t Freq1_Z14_Res_L;	 //3b
	uint16_t Freq1_Z23_Res_H;  //3c
	uint16_t Freq1_Z23_Res_L;	 //3d
	uint16_t Freq1_Z24_Res_H;  //3e
	uint16_t Freq1_Z24_Res_L;	 //3f
	
	uint16_t diff_h;
	uint16_t diff_l;
}UserBIAPart_TypeDef;


//成分分析结果区 r      (84)128     0x1158
typedef struct
{	
    uint8_t FM_H;          //脂肪重 1 				0x00
		uint8_t FM_L;          //         				0x01
		uint8_t SLM_H;         //肌肉重 2 				0x02   2
		uint8_t SLM_L;	        //         				0x03
		uint8_t TF_H;          //含水量 3 				0x04
		uint8_t TF_L;          //				 				0x05
		uint8_t EXF_H;         //细胞外液 4			0x06
		uint8_t EXF_L;		      //								0x07
		uint8_t INF_H;         //细胞内液 5      0x08
		uint8_t INF_L;		      //                0x09
		uint8_t LBM_H;         //去脂体重 6      0x0a
		uint8_t LBM_L;			    //                0x0b
		uint8_t BM_H;          //标准肌肉 7      0x0c
		uint8_t BM_L;          //                0x0d
		
	  uint8_t BMC_H;         //骨盐量 8        0x0e MSW_H 3
		uint8_t BMC_L;	        //                0x0f  
		uint8_t SMM_H;         //骨骼肌重 9      0x10      6
		uint8_t SMM_L;	     		//                0x11
		uint8_t PP_H;          //蛋白质率 10     0x12 PM_H 5
		uint8_t PP_L;					//                0x13 PM_L
		uint8_t BMI_H;         //体质指数 11     0x14       10
		uint8_t BMI_L;					//                0x15
		uint8_t BMR_H;         //基础代谢 12     0x16     7
		uint8_t BMR_L;					//                0x17
    uint8_t BFP_H;         //体脂百分比 13   0x18  BFR_H  1
    uint8_t BFP_L;					//                0x19  BFR_L
		uint8_t BWP_H;         //体水分率 14     0x1a  TFR_H  4
		
		uint8_t BWP_L; 				//                0x1b  TFR_L
		uint8_t BW_H;          //标准体重 15     0x1c         12
		uint8_t BW_L;					//                0x1d
		uint8_t VFR_H;         //内脏脂肪等级 16 0x1e       8
		uint8_t VFR_L;					//                0x1f
		uint8_t MA;            //身体年龄 17     0x20  Body_Age  9
		uint8_t SBC;           //身体评分 18     0x21  Score   11
		uint8_t EE_H;          //水肿度 19       0x22
		uint8_t EE_L;					//                0x23
		uint8_t OD_H;          //肥胖度 20       0x24
		uint8_t OD_L;					//                0x25
		uint8_t MC_H;          //肌肉控制 21     0x26       14
		uint8_t MC_L; 					//                0x27
		uint8_t WC_H;          //体重控制 22     0x28       13
		
		uint8_t WC_L;					//                0x29
		uint8_t FC_H;          //脂肪控制 23     0x2a       15
		uint8_t FC_L;					//                0x2b
    uint8_t TW_H;          //目标体重 24     0x2c
    uint8_t TW_L;          //                0x2d
    //--------------------------------------------以上属于4、8电极共有参数
    //下面属于8电极独有参数
		uint8_t TFP_H;         //Z5 体脂率 25    0x2e  BFR_TR_H   20
		uint8_t TFP_L;  		    //                0x2f
		uint8_t RAFP_H;        //Z2 体脂率 26    0x30  BFR_RA_H   16
		uint8_t RAFP_L;		    //                0x31
		uint8_t LAFP_H;        //Z1 体脂率 27    0x32  BFR_LA_H   17
		uint8_t LAFP_L;		    //                0x33
		uint8_t RLFP_H;        //Z4 体脂率 28    0x34  BFR_RL_H   18
		uint8_t RLFP_L;        //                0x35
		uint8_t LLFP_H;        //Z3 体脂率 29    0x36  BFR_LL_H   19
		
		uint8_t LLFP_L;		    //                0x37
		uint8_t TMM_H;         //Z5 肌肉量 30    0x38  SLM_TR_H    21
		uint8_t TMM_L;         //                0x39
		uint8_t RAMM_H;        //Z2 肌肉量 31    0x3a  SLM_RA_H    22
		uint8_t RAMM_L;		    //                0x3b
		uint8_t LAMM_H;        //Z1 肌肉量 32    0x3c  SLM_LA_H    23
		uint8_t LAMM_L;        //                0x3d
		uint8_t RLMM_H;        //Z4 肌肉量 33    0x3e  SLM_RL_H    24
		uint8_t RLMM_L;        //                0x3f
		uint8_t LLMM_H;        //Z3 肌肉量 24    0x40  SLM_LL_L    25
		uint8_t LLMM_L;        //                0x41
		uint8_t JW_H;          //颈围 35         0x42
		uint8_t JW_L;          //                0x43
		
		uint8_t WL_H;          //腰围 36         0x44
		uint8_t WL_L;          //                0x45
		uint8_t HL_H;          //臀围 37         0x46
		uint8_t HL_L;          //                0x47
		uint8_t XW_H;          //胸围 38         0x48
		uint8_t XW_L;          //                0x49
		uint8_t WHR_H;         // 腰臀比  39     0x4a      26
		uint8_t WHR_L;         //                0x4b
		uint8_t RAW_H;         //Z2 右上臂围 40  0x4c
		uint8_t RAW_L;         //                0x4d
		uint8_t LAW_H;         //Z1 左上臂围 41  0x4e
		uint8_t LAW_L;         //                0x4f
		uint8_t RLW_H;         //Z4 右大腿围 42  0x50
		uint8_t RLW_L;         //                0x51
		
		uint8_t LLW_H;         //Z3 左大腿围 43  0x52
		uint8_t LLW_L;         //                0x53
				
		uint8_t BH;            //心率            0x54

}Analysis_TypeDef;



//测试过程数据 r     54(128)   0x11D8
typedef struct
{
	uint16_t Freq1_Short_AD_H;  //00
	uint16_t Freq1_Short_AD_L;  //04
	uint16_t Freq1_Ref0_AD_H;   //08
	uint16_t Freq1_Ref0_AD_L;	  //0c
	uint16_t Freq1_Ref1_AD_H;   //10
	uint16_t Freq1_Ref1_AD_L;	  //14	
  uint16_t Freq1_Z12_AD_H;
  uint16_t Freq1_Z12_AD_L;		
  uint16_t Freq1_Z34_AD_H;
  uint16_t Freq1_Z34_AD_L;		
  uint16_t Freq1_Z13_AD_H;
  uint16_t Freq1_Z13_AD_L;			
  uint16_t Freq1_Z23_AD_H;
  uint16_t Freq1_Z23_AD_L;		
  uint16_t Freq1_Z24_AD_H;
  uint16_t Freq1_Z24_AD_L;	
  uint16_t Freq1_Z14_AD_H;
  uint16_t Freq1_Z14_AD_L;	
}ADCVal_TypeDef;



//真实电阻            66(128)   0x1270
typedef struct          
{                           //F1组合
  uint16_t F1_Z12_RES_H;
  uint16_t F1_Z12_RES_L;	
  uint16_t F1_Z34_RES_H;
  uint16_t F1_Z34_RES_L;	
  uint16_t F1_Z13_RES_H;
  uint16_t F1_Z13_RES_L;
  uint16_t F1_Z23_RES_H;
  uint16_t F1_Z23_RES_L;
  uint16_t F1_Z24_RES_H;
  uint16_t F1_Z24_RES_L;
  uint16_t F1_Z14_RES_H;
  uint16_t F1_Z14_RES_L;	
	uint16_t F1_Z1_RES_H;    //F1分开
	uint16_t F1_Z1_RES_L;	
	uint16_t F1_Z2_RES_H;
	uint16_t F1_Z2_RES_L;
	uint16_t F1_Z3_RES_H;
	uint16_t F1_Z3_RES_L;
	uint16_t F1_Z4_RES_H;
	uint16_t F1_Z4_RES_L;	
	uint16_t F1_Z5_RES_H;
	uint16_t F1_Z5_RES_L;		
		
}RESVal_TypeDef;



typedef struct 
{				
    uint32_t paddr;
}AddrMap_TypeDef;

typedef enum 
{		
	ISCMD  = 0,   // 0 是地址 1是命令 
  b_sleep=1,
	b_flash=2,
	b_analysis=3,
	b_unlock=4,
	ISMODE=5,   //查模式
	b_wakeup=6,
}Functionaddrflag;

extern AddrMap_TypeDef AddrMap;
extern uint8_t addr_flag;



/******************************************************************************/
/*                         Block declaration                             */
/******************************************************************************/
extern EquipmentConfig_TypeDef EquMent; //设备配置区
extern EquipmentInfo_TypeDef   EquInfo; //设备信息区
extern UserInfo_TypeDef        UsrInfo; //用户信息区
extern UserBIAPart_TypeDef     UsrBIAP; //用户BIA测试区
extern Analysis_TypeDef        Analys;  //分析结果区
extern ADCVal_TypeDef          ADCVal;  //ADC值区
extern RESVal_TypeDef          RESVal;  //真实电阻区


#define  ON_OFF 7    //STATUS
#define  H_BR   6    //心率


#define  CK     7    //CONFIG_K

#define  F1C_FLAG   7  //F1CV_FLAG
#define  F1V_FLAG_6 6
#define  F1V_FLAG_5 5
#define  F1_FLASH   4



#define  Z_REDY       7  //Z_STATUS
#define  Z_selfcheck  6
#define  Z_S12        5
#define  Z_S34        4
#define  Z_S14        3
#define  Z_S13        2
#define  Z_S23        1
#define  Z_S24        0

#define  Err_Short       7 //ERR_STATUS
#define  Err_lowlimit    6
#define  Err_hightlimit  5
#define  Err_exchange    4
#define  Err_Parm        3
#define  Err_Stb         2

#define  C_Recv         7 //C_STATUS
#define  C_Parm         6
#define  C_Exe          5
#define  C_Ok           4

//F_STATUS

//操作命令
#define	 USER_CMD     0x3F5A
#define	 MODE_CMD     0xF000
#define	 SLEEP_PARM     0x35
#define	 WAKEUP_PARM    0x53
#define	 ANALYSIS_PARM  0x04
#define	 FLASH_PARM     0x05
#define	 UNLOCK_PARM    0x06




void addr_map(uint16_t user_addr);
void Clr_All_Block_Ram(void);
void Clr_RESVal_Ram(void);
void Clr_UserBIAPart_Ram(void);
void Clr_Analysis_Ram(void);
void Clr_ADCVal_Ram(void);
void Clr_UsrInfo_ResF1(void);

#endif


/*
//测量结果

    uint32_t BFR_H;        //体脂百分比
    uint32_t BFR_L;
    uint32_t FM_H;         //脂肪重
		uint32_t FM_L;
		uint32_t SLM_H;        //肌肉重
		uint32_t SLM_L;	
		
		uint32_t TFR_H;        //含水比
		uint32_t TFR_L; 
		uint32_t TF_H;         //含水量
		uint32_t TF_L; 		
		uint32_t BMR_H;      //基础代谢
		uint32_t BMR_L;		
		uint32_t MSW_H;        //骨盐量
		uint32_t MSW_L;		
		
		uint32_t VFR_H;        //内脏脂肪等级
		uint32_t VFR_L;		
		uint32_t Body_Age;  //身体年龄
		uint32_t B_Score;     //身体评分
		uint32_t BW_H;         //标准体重
		uint32_t BW_L;		
		uint32_t EXF_H;         //细胞外液
		uint32_t EXF_L;		
		uint32_t INF_H;         //细胞内液
		uint32_t INF_L;		
		uint32_t LBM_H;         //去脂体重
		uint32_t LBM_L;		
		uint32_t PM_H;          //蛋白质
		uint32_t PM_L;	
		uint32_t EE_H;          //水肿度
		uint32_t EE_L;		
		uint32_t OD_H;          //肥胖度
		uint32_t OD_L;		
		uint32_t BIM_H;         //体质指数
		uint32_t BIM_L;		
		uint32_t BM_H;          //标准肌肉
		uint32_t BM_L;		
		uint32_t MC_H;          //肌肉控制
		uint32_t MC_L; 		
		uint32_t WC_H;          //体重控制
		uint32_t WC_L;		
		uint32_t FC_H;          //脂肪控制
		uint32_t FC_L;		
		uint32_t TW_H;          //目标体重
		uint32_t TW_L;		
		uint32_t SMM_H;         //骨骼肌重
		uint32_t SMM_L;		
		uint32_t BFR_RA_H;      //Z2 体脂率
		uint32_t BFR_RA_L;		
		uint32_t BFR_LA_H;      //Z1 体脂率
		uint32_t BFR_LA_L;		
		uint32_t BFR_RL_H;      //Z4 体脂率
		uint32_t BFR_RL_L;
		uint32_t BFR_LL_H;      //Z3 体脂率
		uint32_t BFR_LL_L;		
		uint32_t BFR_TR_H;      //Z5 体脂率
		uint32_t BFR_TR_L;
		uint32_t SLM_RA_H;      //Z2 肌肉量
		uint32_t SLM_RA_L;		
		uint32_t SLM_LA_H;      //Z1 肌肉量
		uint32_t SLM_LA_L;
		uint32_t SLM_RL_H;      //Z4 肌肉量
		uint32_t SLM_RL_L;
		uint32_t SLM_LL_H;      //Z3 肌肉量
		uint32_t SLM_LL_L;
		uint32_t SLM_TR_H;      //Z5 肌肉量
		uint32_t SLM_TR_L;
		uint32_t JW_H;          //颈围
		uint32_t JW_L;
		uint32_t WL_H;          //腰围
		uint32_t WL_L;          
		uint32_t HL_H;          //臀围
		uint32_t HL_L;
		uint32_t XW_H;          //胸围
		uint32_t XW_L;
		uint32_t RAW_H;         //Z2 右上臂围
		uint32_t RAW_L;
		uint32_t LAW_H;         //Z1 左上臂围
		uint32_t LAW_L;
		uint32_t RLW_H;         //Z4 右大腿围
		uint32_t RLW_L;
		uint32_t LLW_H;         //Z3 右大腿围
		uint32_t LLW_L;
		
typedef struct
{
               BFR       //体脂百分比
		uint16_t	fat_wgt;   //体脂重
	  uint8_t   fat_per;   //体脂率
	  uint16_t  muscle_wgt;//肌肉重
	  uint16_t  water_wgt; //水重
	  uint16_t  water_per; //水率
	  uint16_t  BMR;       //基础代谢
	  uint16_t  bone_salt; //骨盐量
	  uint8_t   VFG;       //内脏脂肪等级
    uint8_t   body_age;  //身体年龄
	  uint8_t   body_score;//身体分数
	  uint8_t   std_wgt;   //标准体重
	  uint16_t  cell_out;  //细胞外液
	  uint16_t  cell_in;   //细胞内液
	  
    uint16_t   LBM;      //瘦体重
	  uint16_t  protein;   //蛋白质
	  uint16_t  edema_degree;//水肿度
	  uint16_t  obesity_degree;//肥胖度
	  uint16_t  BMI;
	  uint16_t  std_muscle;    //标准肌肉
	  uint16_t  muscle_control; //肌肉控制
	  uint16_t  weight_control; //体重控制
	  uint16_t  fat_control; //脂肪控制
		
		uint16_t  rh_fat_per;  //右手脂肪率
		uint16_t  lh_fat_per;  //左手脂肪率
		uint16_t  rl_fat_per;  //右脚脂肪率
		uint16_t  ll_fat_per;  //左脚脂肪率
		uint16_t  trunk_fat_per;  //躯干脂肪率
		
		uint16_t  rh_muscle;//右手肌肉
		uint16_t  lh_muscle;//左手肌肉
		uint16_t  rl_muscle;//右脚肌肉
		uint16_t  ll_muscle;//左脚肌肉
		uint16_t  trunk_muscle;  //躯干肌肉
		
		uint16_t  neck_perimeter;  //脖围
		uint16_t  waist_perimeter; //腰围
		uint16_t  hip_perimeter;   //臀围
		uint16_t  chest_perimeter; //胸围
		uint16_t  r_hip_perimeter; //右上臀围
		uint16_t  l_hip_perimeter; //左上臀围
		uint16_t  r_leg_perimeter; //右大腿围
		uint16_t  l_leg_perimeter; //左大腿围
		uint8_t   waist_hip_ratio; //腰臀比
    uint8_t   heart_rate;      //心率
		uint8_t   breathing_rate;  //呼吸率
		uint8_t   breathing_times;  //呼吸次数
}Measure_Results_TypeDef;
*/
