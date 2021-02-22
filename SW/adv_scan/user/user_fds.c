#include "user_head.h"

static fds_params_t fds_params = 
{
	.adv_nam = "JX15N2C1051111AAA300",
	.adv_int = 32,
	.adv_dly = 32,
	.adv_tmo = 5000,
	.adv_tx_pwr = 12,
};


static fds_record_t const m_params_record = 
{
	.file_id = FDS_FILE_ID,
	.key = FDS_PARAMS_KEY,
	.data.p_data = &fds_params,
	.data.length_words = (sizeof(fds_params) + 3) / sizeof(uint32_t),
};
my_fds_info_t my_fds_info;

void FDS_Evt_Handler(fds_evt_t const * p_evt)
{
    //判断事件类型
    switch (p_evt->id)
    {
        case FDS_EVT_INIT://FDS初始化事件
					  //初始化成功
            if (p_evt->result == NRF_SUCCESS)
            {
                my_fds_info.busy = false;
            }
			break;
        //FDS写记录事件
        case FDS_EVT_WRITE:
       
			if (p_evt->result == NRF_SUCCESS)
			{
				my_fds_info.busy = false;
			}
			break;
				//FDS更新记录事件
        case FDS_EVT_UPDATE:
        
			//写记录成功
			if (p_evt->result == NRF_SUCCESS)
			{
				my_fds_info.busy = false;
			}
			break;
				//FDS碎片整理事件
        case FDS_EVT_GC:
        
            
			if (p_evt->result == NRF_SUCCESS)
			{
				my_fds_info.busy = false;
			}
			break;
        default:
            break;
    }
}
void FDS_Wait_Ready(void)
{
	while(my_fds_info.busy)
	{
		(void) sd_app_evt_wait();
	}
}
void FDS_GC(void)
{
	ret_code_t rc;

	if((my_fds_info.gc == true) && (my_fds_info.busy == false))
	{
		my_fds_info.gc = false;
		my_fds_info.busy = true;
		rc = fds_gc();//FDS碎片回收
		APP_ERROR_CHECK(rc);//用错误处理模块检查函数返回值
		FDS_Wait_Ready();
		
	}
}

void FDS_Set_Params(_adv_param pam)
{
	memcpy(&fds_params, &pam, sizeof(fds_params));
}
fds_params_t* FDS_Get_Params(void)
{
	return &fds_params;
}

void FDS_Set_Update_Status(bool sta)
{
	my_fds_info.params_record_update = sta;
}
bool FDS_Get_Update_Status(void)
{
	return my_fds_info.params_record_update;
}

void FDS_Read_Params_Record(void)
{
	ret_code_t rc;
	fds_record_desc_t desc = {0};
	fds_find_token_t tok = {0};
	
	memset(&tok, 0x00, sizeof(fds_find_token_t));
	rc = fds_record_find(FDS_FILE_ID, FDS_PARAMS_KEY, &desc, &tok);

	if(rc == NRF_SUCCESS)
	{
		fds_flash_record_t temp = {0};
		rc = fds_record_open(&desc, &temp);
		APP_ERROR_CHECK(rc);

		//static fds_params_t temp_params;
		//memcpy(&temp_params, temp.p_data, sizeof(fds_params_t));
		memcpy(&fds_params, temp.p_data,  sizeof(fds_params_t));
		rc = fds_record_close(&desc);
		APP_ERROR_CHECK(rc);
	//	NRF_LOG_INFO("FDS name is %s", fds_params.adv_nam);
	}
	
}

void FDS_Init(void)
{
	ret_code_t rc;
	fds_record_desc_t desc = {0};
	fds_find_token_t tok = {0};

	fds_register(FDS_Evt_Handler);
	my_fds_info.busy = true;
	rc = fds_init();
	APP_ERROR_CHECK(rc);
	FDS_Wait_Ready();

	memset(&tok, 0x00, sizeof(fds_find_token_t));
	rc = fds_record_find(FDS_FILE_ID, FDS_PARAMS_KEY, &desc, &tok);
	if(rc != NRF_SUCCESS)
	{
		my_fds_info.busy = true;
		rc = fds_record_write(&desc, &m_params_record);
		APP_ERROR_CHECK(rc);
		FDS_Wait_Ready();
	}
	FDS_Read_Params_Record();
}

void FDS_Update_Params(void)
{
	ret_code_t rc;
	fds_record_desc_t desc = {0};
	fds_find_token_t tok = {0};
	
	if((my_fds_info.params_record_update == true)&&(my_fds_info.busy == false))
	{
		memset(&tok, 0x00, sizeof(fds_find_token_t));
		my_fds_info.busy = true;
		rc = fds_record_find(FDS_FILE_ID, FDS_PARAMS_KEY, &desc, &tok);

		if(rc == NRF_SUCCESS)
		{
			NRF_LOG_INFO(" updata fds");
			my_fds_info.params_record_update = false;
			rc = fds_record_update(&desc, &m_params_record);
			APP_ERROR_CHECK(rc);
			FDS_Wait_Ready();
		}
	}
}


void FDS_Run(void)
{
	FDS_Update_Params();
	FDS_GC();
}
