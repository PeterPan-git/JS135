#include "user_head.h"

static U8 app_data[20];
void APP_Data_Init(void)
{
	memset(app_data, 0x00, sizeof(app_data));
}
void APP_Data_Set(U8 *data)
{
	#if (DEBUG_BLE_APP)
	U8 loop;
	#endif
	
	memcpy(app_data, data, sizeof(app_data));
	
	#if (DEBUG_BLE_APP)
	NRF_LOG_RAW_INFO("App_data:0x");
	for(loop = 0; loop < 20; loop++)
	{
		
		NRF_LOG_RAW_INFO(" %x", app_data[loop]);
		
	}
	NRF_LOG_RAW_INFO("\n");
	#endif
}
U8* APP_Data_Get(void)
{
	return app_data;
}

U32 APP_Data_Receice(const ble_gap_evt_adv_report_t *p_adv_report)
{
	uint32_t  index = 0;
	uint8_t * p_data;

	p_data = p_adv_report->data.p_data;
   //检索广播数据
   while (index < p_adv_report->data.len)
   {
      uint8_t field_length = p_data[index];
      uint8_t field_type   = p_data[index + 1];
      //如果ADV TYPE是完整的设备名称或者裁剪的设备名称，打印出设备名称
  	 switch(field_type)
	  {
	  	case BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE:

			break;
			
		case BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME:
			//NRF_LOG_RAW_INFO("\n name:");
		  	#if 0
		  	for(i=0;i<field_length-1;i++)
		  	NRF_LOG_RAW_INFO("%c", p_data[index+2+i]);
			#endif
			break;
		
		case BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
			
			if((p_data[5] == 0x13) && (p_data[6] == 0xEE))
			{
				APP_Data_Set(&p_data[5]);
				
				NRF_LOG_RAW_INFO("data:0x");
				for(U8 i=0;i<field_length-1;i++)
		  		{
					NRF_LOG_RAW_INFO("%x", app_data[i]);

				}
				NRF_LOG_RAW_INFO("\n");
			}
			
			break;
	  }
      index += field_length + 1;
    }
	 return NRF_ERROR_NOT_FOUND;
}

