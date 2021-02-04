#ifndef _USER_FDS_H_
#define _USER_FDS_H_
#include "user_head.h"

#define FDS_FILE_ID                 (0x1000)
#define FDS_PARAMS_KEY              (0x1001)


typedef struct
{
	char adv_nam[22];
	U32 adv_int;
	U32 adv_dly;
	U32 adv_tmo;
	U8 adv_tx_pwr;
}
__attribute__((aligned(4)))fds_params_t;
typedef struct
{
	bool params_record_update;
	bool read;       				//读记录标志
	bool gc;         				//碎片收集标志
	bool busy;       				//FDS忙标志
}my_fds_info_t;

void FDS_Init(void);
void FDS_Evt_Handler(fds_evt_t const * p_evt);
void FDS_Wait_Ready(void);
void FDS_GC(void);
void FDS_Set_Params(_adv_param pam);
fds_params_t* FDS_Get_Params(void);

void FDS_Set_Update_Status(bool sta);
bool FDS_Get_Update_Status(void);
void FDS_Read_Params_Record(void);
void FDS_Update_Params(void);

void FDS_Run(void);


#endif
