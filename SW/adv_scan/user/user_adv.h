#ifndef _USER_ADV_H_
#define _USER_ADV_H_
#include "user_head.h"

#define ADV_OPEN         true
#define ADV_CLOSE        false
#define SCAN_OPEN        true
#define SCAN_CLOSE       false 
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
	uint8_t data1[18];
	uint8_t data2[18];
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
void ADV_Data_Init(void);
void ADV_Data_Set(U8 *data, _dat_blk dat_blk);
void ADV_Data_Get(U8 *data, _dat_blk dat_blk);

void ADV_Status_Set(bool sta);
bool ADV_Status_Get(void);
void Scan_Status_Set(bool sta);
bool Scan_Status_Get(void);
void ADV_Two_Set(_adv_two *sta);
void ADV_Two_Get(_adv_two *sta);
void ADV_Param_Init(void);
bool ADV_Set_Param(_adv_param *prm);
bool ADV_Get_Param(_adv_param *prm);

void ADV_Set_Name(void);
void ADV_Store_Name(char *name_buf);
char* ADV_Get_Name(void);

void ADV_Set_Interval(char *itv_buf);
char* ADV_Get_Interval(void);

void ADV_Set_Delay(char *tim_buf);
char* ADV_Get_Delay(void);

void ADV_Set_Timeout(char *timeout_buf);
char* ADV_Get_Timeout(void);

void ADV_Set_TxPwr(char *tim_buf);
char* ADV_Get_TxPwr(void);

void ADV_Update_Data(void);

void advertising_init(void);
#endif
