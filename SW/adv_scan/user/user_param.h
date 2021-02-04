#ifndef _USER_PARAM_H_
#define _USER_PARAM_H_
#include "user_head.h"

#define ADV_OPEN         true
#define ADV_CLOSE        false
#define SCAN_OPEN        true
#define SCAN_CLOSE       false 
	
#define PA_PIN     17
#define LNA_PIN    16
typedef struct
{
	char adv_name[22];
	uint32_t adv_interval;            //广播间隙interval
	uint32_t adv_delay;            //广播持续时间
	uint32_t adv_timeout;
	int8_t tx_power;
}_adv_param;

typedef struct
{
	uint8_t data1[20];
	uint8_t data2[20];
}_adv_dat;

typedef struct
{
	bool who_block;           //true 代表第一条消息， false 代表第二条消息
	bool adv_two_status;      //true 代表广播两条消息， false 代表广播一条消息
}_adv_two;

typedef enum
{
	ADV_DATA1 = 0,
	ADV_DATA2,
}_dat_blk;

void Param_ADV_Data_Init(void);
void Param_ADV_Data_Set(U8 *data, _dat_blk dat_blk);
void Param_ADV_Data_Get(U8 *data, _dat_blk dat_blk);
void Param_ADV_Two_Set(_adv_two *sta);
void Param_ADV_Two_Get(_adv_two *sta);
void Param_ADV_Param_Init(void);
bool Param_ADV_Set_Param(_adv_param *prm);
bool Param_ADV_Get_Param(_adv_param *prm);
void Param_ADV_Set_Name(void);
void Param_ADV_Store_Name(char *name_buf);
char* Param_ADV_Get_Name(void);
void Param_ADV_Set_Interval(char *itv_buf);
char* Param_ADV_Get_Interval(void);
void Param_ADV_Set_Delay(char *tim_buf);
char* Param_ADV_Get_Delay(void);
void Param_ADV_Set_Timeout(char *timeout_buf);
char* Param_ADV_Get_Timeout(void);
void Param_ADV_Set_TxPwr(char *tim_buf);
char* Param_ADV_Get_TxPwr(void);
void Param_ADV_Update_Data(void);
void Param_ADV_Status_Set(bool sta);
bool Param_ADV_Status_Get(void);
void Param_Scan_Status_Set(bool sta);
bool Param_Scan_Status_Get(void);
void Param_Set_Ble_Connect_Status(bool status);
bool Param_Get_Ble_Connect_Status(void);
//void Param_Update_Who_Data(void);
void Param_Update_One(void);
void Param_Update_Two(void);
void Param_Get_MacAddr(void);
void Param_Set_PA_LNA(U32 gpio_pa_pin, U32 gpio_lna_pin);
void Param_PA_Test(void);
#endif
