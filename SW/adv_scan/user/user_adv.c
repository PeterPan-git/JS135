#include "user_head.h"

//char ble_name[20] = "JS_BLE";
char send_str[30] = "send_name";
static _adv_dat adv_dat;
static _adv_param adv_prm;
static _adv_two adv_two;
static bool adv_status;
static bool scan_status;
#if 0
{
	.adv_name = "JS_123456789",
	.adv_interval = 64,
	.adv_delay = 100,
	.adv_timeout = 0,
};
#endif

void ADV_Data_Init(void)
{
	adv_two.adv_two_status = true;
	adv_two.who_block = false;
	memset(adv_dat.data1, 0x11, sizeof(adv_dat.data1));
	memset(adv_dat.data2, 0x22, sizeof(adv_dat.data2));
}

void ADV_Data_Set(U8 *data, _dat_blk dat_blk)
{
	uint8_t loop;

	switch(dat_blk)
	{
		case ADV_DATA1:
			for(loop = 0; loop < sizeof(adv_dat.data1); loop++)
			{
				adv_dat.data1[loop] = data[loop];
			}
			break;

		case ADV_DATA2:
			for(loop = 0; loop < sizeof(adv_dat.data2); loop++)
			{
				adv_dat.data2[loop] = data[loop];
			}
			break;

		default:
			break;
	}
}
void ADV_Data_Get(U8 *data, _dat_blk dat_blk)
{
	uint8_t loop;

	switch(dat_blk)
	{
		case ADV_DATA1:
			for(loop = 0; loop < sizeof(adv_dat.data1); loop++)
			{
				data[loop] = adv_dat.data1[loop];
			}
			break;

		case ADV_DATA2:
			for(loop = 0; loop < sizeof(adv_dat.data2); loop++)
			{
				data[loop] = adv_dat.data2[loop];
			}
			break;

		default:
			break;
	}
}
void ADV_Status_Set(bool sta)
{
	adv_status = sta;
}
bool ADV_Status_Get(void)
{
	return adv_status;
}
void Scan_Status_Set(bool sta)
{
	scan_status = sta;
}
bool Scan_Status_Get(void)
{
	return scan_status;
}

void ADV_Two_Set(_adv_two *sta)
{
	adv_two.who_block = sta->who_block;
	adv_two.adv_two_status = sta->adv_two_status;
}
void ADV_Two_Get(_adv_two *sta)
{
	sta->who_block = adv_two.who_block;
	sta->adv_two_status = adv_two.adv_two_status;
}

void ADV_Param_Init(void)
{
	strcpy(adv_prm.adv_name, "JS135---2");//par.adv_name = "JS_12";
	adv_prm.adv_interval = 64;
	adv_prm.adv_delay = 100;
	adv_prm.adv_timeout = 10;
	adv_prm.tx_power = 4;
	
}

bool ADV_Set_Param(_adv_param *prm)
{
	strcpy(adv_prm.adv_name, prm->adv_name);//adv_prm.adv_name = prm->adv_name;
	adv_prm.adv_interval = prm->adv_interval;
	adv_prm.adv_delay = prm->adv_delay;
	adv_prm.adv_timeout = prm->adv_timeout;
	adv_prm.tx_power = prm->tx_power;

	return true;
}
bool ADV_Get_Param(_adv_param *prm)
{
	strcpy(prm->adv_name, adv_prm.adv_name);//prm->adv_name = adv_prm.adv_name;
	prm->adv_interval = adv_prm.adv_interval;
	prm->adv_delay = adv_prm.adv_delay;
	prm->adv_timeout = adv_prm.adv_timeout;
	prm->tx_power = adv_prm.tx_power;
	
	return true;
}


void ADV_Set_Name(void)
{
	uint32_t                err_code;
    ble_gap_conn_sec_mode_t sec_mode;
	
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) adv_prm.adv_name,
                                          strlen(adv_prm.adv_name));
    APP_ERROR_CHECK(err_code);

	
}

void ADV_Store_Name(char *name_buf)
{
	uint8_t name_len = 0;  //接收到名字实际长度
	uint8_t loop;
	
	
	name_len = strlen(name_buf) - 11;
	memset(adv_prm.adv_name, 0, sizeof(adv_prm.adv_name));
	
	for(loop = 0; loop < name_len; loop++)
	{
		adv_prm.adv_name[loop] = name_buf[loop+10];
	}
	
	NRF_LOG_INFO("name is %s", adv_prm.adv_name);
}
char* ADV_Get_Name(void)
{
      char name[6] = "<name=";
	  
	  memset(send_str, 0, strlen(send_str));
	  strcpy(send_str, name); 
	  strcat(send_str, adv_prm.adv_name); 
	  
	  send_str[strlen(send_str)] = '>';
	  
	  return send_str;
}


char* ADV_Get_Interval(void)
{
	char head_buf[] = "<adv_int=";
	char buffer[4] = "";
	
	memset(send_str, 0, strlen(send_str));
  	strcpy(send_str, head_buf); 
	
	sprintf((char*)buffer, "%d", adv_prm.adv_interval);
	
  	strcat(send_str, buffer); 
  
  	send_str[strlen(send_str)] = '>';

	return send_str;
}

void ADV_Set_Interval(char *itv_buf)
{
	char str[6] = "";              
	
    uint8_t loop1;
	uint8_t residue = strlen("<SET_ADV_INT=");
    uint8_t len = 0;
    
    len = strlen(itv_buf) - residue;
    
    for(loop1 = 0; loop1 < len; loop1++)
    {
        str[loop1] = itv_buf[residue + loop1];
    }

	adv_prm.adv_interval = atoi(str);
	
	NRF_LOG_INFO("adv_itv is %d", adv_prm.adv_interval);
	
}

void ADV_Set_Delay(char *tim_buf)
{
	char str[6] = "";              
	U16 delay_buffer = 0;
	
    uint8_t loop1;
	uint8_t residue = strlen("<SET_ADV_DELAY=");
    uint8_t len = 0;
    
    len = strlen(tim_buf) - residue;
    
    for(loop1 = 0; loop1 < len; loop1++)
    {
        str[loop1] = tim_buf[residue + loop1];
    }
	delay_buffer = atoi(str);
	 uart_data.adv_time= delay_buffer*10;
	for(loop1 = 0; loop1 < 4; loop1++)
	{
		uart_data.adv_sta[loop1] = true;
	}
	NRF_LOG_INFO("adv_delay is %d", uart_data.adv_time);

}
char* ADV_Get_Delay(void)
{
	char head_buf[] = "<adv_delay=";
	char buffer[10] = "";
	U16 delay_buffer = 0;
	
	memset(send_str, 0, strlen(send_str));
  	strcpy(send_str, head_buf); 
	delay_buffer = uart_data.adv_time / 10;
	sprintf((char*)buffer, "%d", delay_buffer);
	
  	strcat(send_str, buffer); 
  
  	send_str[strlen(send_str)] = '>';

	return send_str;
}

void ADV_Set_Timeout(char *timeout_buf)
{
	char str[6] = "";              
	
    uint8_t loop1;
	uint8_t residue = strlen("<SET_ADV_TIMEOUT=");
    uint8_t len = 0;
    
    len = strlen(timeout_buf) - residue;
    
    for(loop1 = 0; loop1 < len; loop1++)
    {
        str[loop1] = timeout_buf[residue + loop1];
    }

	adv_prm.adv_timeout= atoi(str);
	
	NRF_LOG_INFO("adv_timeout is %d", adv_prm.adv_timeout);
}
char* ADV_Get_Timeout(void)
{
	char head_buf[] = "<adv_timeout=";
	char buffer[10] = "";
	
	memset(send_str, 0, strlen(send_str));
  	strcpy(send_str, head_buf); 
	
	sprintf((char*)buffer, "%d", adv_prm.adv_timeout);
	
  	strcat(send_str, buffer); 
  
  	send_str[strlen(send_str)] = '>';

	return send_str;
}

void ADV_Set_TxPwr(char *txp_buf)
{
	char str[6] = "";              
	
    uint8_t loop1;
	uint8_t residue = strlen("<ST_TX_POWER=");
    uint8_t len = 0;
    
    len = strlen(txp_buf) - residue-1;

	if(txp_buf[residue] == '+')
	{
		for(loop1 = 0; loop1 < len; loop1++)
    	{
        	str[loop1] = txp_buf[residue + loop1+1];
			adv_prm.tx_power= atoi(str);
    	}
	}
	else if(txp_buf[residue] == '-')
	{
		for(loop1 = 0; loop1 < len; loop1++)
    	{
        	str[loop1] = txp_buf[residue + loop1+1];
			adv_prm.tx_power= -(atoi(str));
    	}
	}
	else
	{
		NRF_LOG_INFO("error");
	}
	#if 0
    for(loop1 = 0; loop1 < len; loop1++)
    {
        str[loop1] = txp_buf[residue + loop1];
    }
	NRF_LOG_INFO("str is %s", str);
	adv_prm.tx_power= atoi(str);
	#endif
	NRF_LOG_INFO("tx_power is %d", adv_prm.tx_power);
}
char* ADV_Get_TxPwr(void)
{
	char head_buf[] = "<st_tx_power=";
	char buffer[10] = "";
	
	memset(send_str, 0, strlen(send_str));
  	strcpy(send_str, head_buf); 

	
	sprintf((char*)buffer, "%d", adv_prm.tx_power);
	
  	strcat(send_str, buffer); 
  
  	send_str[strlen(send_str)] = '>';

	return send_str;
}

void ADV_Update_Data(void)
{
	uint8_t loop;
	if(ADV_Status_Get() == true)
	{
		for(loop = 0; loop < 8; loop++)
		{
			if(uart_data.adv_timeout[loop] == true)
			{
				uart_data.adv_timeout[loop] = false;
				advertising_stop();
				Uart_Data_Transfer();
				adv_updata();
				advertising_init();
				advertising_start();
			}
		}
	}
	
}


