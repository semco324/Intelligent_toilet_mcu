#ifndef __ANALYSIS_H
#define __ANALYSIS_H
#include "stm32f0xx.h"
#include "addr_map.h"
#include "driver_cs1258.h"

//人体信息范围
#define WEIGHT_MAX   1500 //kg*10
#define WEIGHT_MIN   200  //kg*10

#define HT_MAX   220  //cm
#define HT_MIN   90  //cm

#define AGE_MAX   99  //year
#define AGE_MIN   18  //year

//#define   UART_HR
#define HR_MODE

//	//输入参数
//extern uint8_t B_SEX; //性别 1 man 0 weman
//extern uint8_t B_Age; //年龄
//extern double  B_H;   //身高 mm
//extern double  B_Wt;  //体重 kg
//extern double  B_Z;   //双脚阻抗 ohm
//extern double  B_Z_RA;//Z2
//extern double  B_Z_LA;//Z1
//extern double  B_Z_RL;//Z4
//extern double  B_Z_LL;//Z3
//extern double  B_Z_TR;//Z5

//输出参数 24

//成分组成 6
extern double B_BFP;        //体脂率
extern double B_SLM;        //肌肉重**
extern double B_BMC;        //骨盐量**
extern double B_BWP;        //体水分率
extern double B_PP;         //蛋白质率
extern double B_SMM;        //骨骼肌重* 4-8 **

/*
总水重 = 体重 * B_BWP
去脂体重 = 体重 - 体重 * B_BFR
脂肪重 = 体重 * B_BFR
蛋白质 = 体重 * B_PP
extern double B_TF;         //总水重*
extern double B_LBM;        //去脂体重*
extern double B_FM;         //脂肪重*
extern double B_PM;         //蛋白质*
*/

//身体机能参数 9
extern uint16_t B_BMR;      //基础代谢* 
extern double B_VFR;        //内脏脂肪等级*
extern uint8_t B_MA;        //身体年龄*  B_Body_Age--->B_MA
extern double B_BMI;        //体质指数***************
extern uint8_t B_SBC;        //身体得分   B_Score-->B_SBC
extern double B_BW;         //标准体重
extern double B_WC;         //体重控制
extern double B_MC;         //肌肉控制
extern double B_FC;         //脂肪控制


//节段部分 11
extern double B_RAFP;      //Z2 体脂率 8
extern double B_LAFP;      //Z1 体脂率
extern double B_RLFP;      //Z4 体脂率
extern double B_LLFP;      //Z3 体脂率
extern double B_TFP;       //Z5 体脂率

extern double B_RAMM;      //Z2 肌肉量
extern double B_LAMM;      //Z1 肌肉量
extern double B_RLMM;      //Z4 肌肉量
extern double B_LLMM;      //Z3 肌肉量
extern double B_TMM;       //Z5 肌肉量

extern double B_WHR;    //腰臀比






//extern double B_BW;         //标准体重*
//extern double B_EXF;         //细胞外液*
//extern double B_INF;         //细胞内液*


//extern double B_EE;          //水肿度*
//extern double B_OD;          //肥胖度*

//extern double B_BM;          //标准肌肉*
//extern double B_MC;          //肌肉控制*
//extern double B_WC;          //体重控制*
//extern double B_FC;          //脂肪控制*
//extern double B_TW;          //目标体重



//extern double B_JW;          //颈围
//extern double B_WL;          //腰围
//extern double B_HL;          //臀围
//extern double B_XW;          //胸围
//extern double B_LAW;         //Z1 左上臂围
//extern double B_RLW;         //Z4 右大腿围
//extern double B_LLW;         //Z3 右大腿围 2.3.4

uint8_t analysis_parm(void);
void CS125X_XLCL_Initial(void);
void CS125X_XLCL_Initial_Per(void);
uint8_t HRI(void);


extern uint32_t B_temp_XLCL;
//IO 
#define SIM_TX_L  GPIO_WriteBits(GPIO_PA11,0) //GPIOA->PA_DOUT&=~SIM_TX_PIN// //Tx 0
#define SIM_TX_H  GPIO_WriteBits(GPIO_PA11,1) //GPIOA->PA_DOUT|=SIM_TX_PIN////Tx 1


#endif
