#include "user_head.h"

//char ble_name[20] = "JS_BLE";
char send_str[30] = "send_name";
static _adv_dat adv_dat;
static _adv_param adv_prm;
static _adv_two adv_two;
static bool adv_status;
static bool scan_status;
static bool con_sta = false;
U8 adv_name_cot = 0;
U8 adv_mode = 0;
void Param_ADV_Data_Init(void)
{
	adv_two.adv_two_status = true;
	adv_two.who_block = false;
	memset(adv_dat.data1, 0x00, sizeof(adv_dat.data1));
	memset(adv_dat.data2, 0x00, sizeof(adv_dat.data2));
	Param_Set_Ble_Connect_Status(BLE_DISCONNECT);
}

void Param_ADV_Data_Set(U8 *data, _dat_blk dat_blk)
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
void Param_ADV_Data_Get(U8 *data, _dat_blk dat_blk)
{
	uint8_t loop;

	switch(dat_blk)
	{
		case ADV_DATA1:
		//	memcpy(data,adv_dat.data1, sizeof(data));
			for(loop = 0; loop < 20; loop++)
			{
				data[loop] = adv_dat.data1[loop];
			}
			
			break;

		case ADV_DATA2:
			//memcpy(data,adv_dat.data2, sizeof(data));
			for(loop = 0; loop < 20; loop++)
			{
				data[loop] = adv_dat.data2[loop];
			}
	
			break;

		default:
			break;
	}
	
}
void Param_ADV_Two_Set(_adv_two *sta)
{
	adv_two.who_block = sta->who_block;
	adv_two.adv_two_status = sta->adv_two_status;
}
void Param_ADV_Two_Get(_adv_two *sta)
{
	sta->who_block = adv_two.who_block;
	sta->adv_two_status = adv_two.adv_two_status;
}

void Param_ADV_Param_Init(void)
{
	strcpy(adv_prm.adv_name, FDS_Get_Params()->adv_nam);
	adv_prm.adv_interval = FDS_Get_Params()->adv_int;
	adv_prm.adv_delay = FDS_Get_Params()->adv_dly;
	adv_prm.adv_timeout = FDS_Get_Params()->adv_tmo;
	adv_prm.tx_power = FDS_Get_Params()->adv_tx_pwr;
	CMNC_String_To_Hex(adv_prm.adv_name, name_hex);
	//NRF_LOG_INFO(" FDS_Get_Params()->adv_nam is : %s", FDS_Get_Params()->adv_nam);
	//NRF_LOG_INFO(" adv_prm.adv_name is : %s", adv_prm.adv_name);
}

bool Param_ADV_Set_Param(_adv_param *prm)
{
	strcpy(adv_prm.adv_name, prm->adv_name);//adv_prm.adv_name = prm->adv_name;
	adv_prm.adv_interval = prm->adv_interval;
	adv_prm.adv_delay = prm->adv_delay;
	adv_prm.adv_timeout = prm->adv_timeout;
	adv_prm.tx_power = prm->tx_power;

	return true;
}
bool Param_ADV_Get_Param(_adv_param *prm)
{
	strcpy(prm->adv_name, adv_prm.adv_name);//prm->adv_name = adv_prm.adv_name;
	prm->adv_interval = adv_prm.adv_interval;
	prm->adv_delay = adv_prm.adv_delay;
	prm->adv_timeout = adv_prm.adv_timeout;
	prm->tx_power = adv_prm.tx_power;
	
	return true;
}


void Param_ADV_Set_Name(void)
{
	uint32_t                err_code;
    ble_gap_conn_sec_mode_t sec_mode;
	
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) adv_prm.adv_name,
                                          sizeof(adv_prm.adv_name));   //strlen(adv_prm.adv_name)
    APP_ERROR_CHECK(err_code);

	
}

void Param_ADV_Store_Name(char *name_buf)
{
	uint8_t name_len = 0;  //接收到名字实际长度
	uint8_t loop;
	

	name_len = strlen(name_buf) - 11;
	memset(adv_prm.adv_name, 0, sizeof(adv_prm.adv_name));
	
	for(loop = 0; loop < name_len; loop++)
	{
		adv_prm.adv_name[loop] = name_buf[loop+10];
	}
	CMNC_String_To_Hex(adv_prm.adv_name, name_hex);
	//FDS_Set_Name(adv_prm.adv_name);
	FDS_Set_Params(adv_prm);
	//NRF_LOG_INFO("name is %s", adv_prm.adv_name);
	//for(loop = 0; loop < 3; loop++)
	//	{
	//		NRF_LOG_INFO("!!!!!!!!!!!!!!!!!name_hex[%d]:0x%02x", loop, name_hex[loop]);
	//	}
}
char* Param_ADV_Get_Name(void)
{
      char name[6] = "<name=";
	  
	  memset(send_str, 0, strlen(send_str));
	  strcpy(send_str, name); 
	  strcat(send_str, adv_prm.adv_name); 
	  
	  send_str[strlen(send_str)] = '>';
	  
	  return send_str;
}


char* Param_ADV_Get_Interval(void)
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

void Param_ADV_Set_Interval(char *itv_buf)
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
	FDS_Set_Params(adv_prm);
	
	//NRF_LOG_INFO("adv_itv is %d", adv_prm.adv_interval);
	
}

void Param_ADV_Set_Delay(char *tim_buf)
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
	 adv_prm.adv_delay= delay_buffer*10;
	FDS_Set_Params(adv_prm);
	//NRF_LOG_INFO("adv_delay is %d", adv_prm.adv_delay);

}
char* Param_ADV_Get_Delay(void)
{
	char head_buf[] = "<adv_delay=";
	char buffer[10] = "";
	U16 delay_buffer = 0;
	
	memset(send_str, 0, strlen(send_str));
  	strcpy(send_str, head_buf); 
	delay_buffer = adv_prm.adv_delay / 10;
	sprintf((char*)buffer, "%d", delay_buffer);
	
  	strcat(send_str, buffer); 
  
  	send_str[strlen(send_str)] = '>';

	return send_str;
}

void Param_ADV_Set_Timeout(char *timeout_buf)
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

	adv_prm.adv_timeout= atoi(str)*1000;
	FDS_Set_Params(adv_prm);
	//NRF_LOG_INFO("^^^^^^^^^^^^^^^^adv_timeout is %d", adv_prm.adv_timeout);
}
char* Param_ADV_Get_Timeout(void)
{
	char head_buf[] = "<adv_timeout=";
	char buffer[10] = "";
	
	memset(send_str, 0, strlen(send_str));
  	strcpy(send_str, head_buf); 
	adv_prm.adv_timeout = adv_prm.adv_timeout / 1000;
	sprintf((char*)buffer, "%d", adv_prm.adv_timeout);
	
  	strcat(send_str, buffer); 
  
  	send_str[strlen(send_str)] = '>';

	return send_str;
}

void Param_ADV_Set_TxPwr(char *txp_buf)
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
	FDS_Set_Params(adv_prm);
	NRF_LOG_INFO("tx_power is %d", adv_prm.tx_power);
}
char* Param_ADV_Get_TxPwr(void)
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

void Param_ADV_Update_Data(void)
{
	U8 adv_data[18];
	memset(adv_data, 0, 18);

	if(Param_ADV_Status_Get() == true)
	{
		
		if(adv_update_tim == 60)
		{
			adv_update_tim = 0;
			if(CMCN_Check() > 4)
			{
				Param_Update_One();
			}
			else
			{
				Param_Update_Two();
			}
		}
	}
	
}
void Param_ADV_Update_By_Time(void)
{
	if(Param_ADV_Status_Get() == true)
	{
	//	Param_Update_One();
		if(CMCN_Check() == 8)//如果数据为空则工作在可连接广播模式
		{
		//	NRF_LOG_INFO("con mode");
			if(adv_mode != 0)
			{
				//NRF_LOG_INFO("modified con mode");
				BLE_Beacon_Stop();
				BLE_ADV_CON();
				BLE_Beacon_Start();
			}
		}
		else
		{
			if(adv_update_tim == 0)
			{
				if(CMCN_Check() > 4)
				{
					Param_Update_One();
				//	NRF_LOG_INFO("@4:ble --> app by adv");	
				}
				else
				{
					Param_Update_Two();
				}
			}
		}
					
	}
}
void Param_ADV_Status_Set(bool sta)
{
	adv_status = sta;
}
bool Param_ADV_Status_Get(void)
{
	return adv_status;
}

void Param_Scan_Status_Set(bool sta)
{
	scan_status = sta;
}
bool Param_Scan_Status_Get(void)
{
	return scan_status;
}

void Param_Set_Ble_Connect_Status(bool status)
{
	con_sta = status;
}
bool Param_Get_Ble_Connect_Status(void)
{
	return con_sta;
}

void Param_Update_One(void)
{
	U8 adv_data[20];
	memset(adv_data, 0, 20);
	
	CMCN_Get();
	Param_ADV_Data_Get(adv_data, ADV_DATA1);
//	NRF_LOG_RAW_INFO("adv data:0x");
//	for(U8 i=0;i<20;i++)
//	{
//		NRF_LOG_RAW_INFO("%02x", adv_data[i]);

//	}
//	NRF_LOG_RAW_INFO("\n");
	BLE_Beacon_Stop();
	BLE_ADV_Data(adv_data);
	BLE_Beacon_Start();

	
}
void Param_Update_Two(void)
{
	U8 adv_data[20];
	static U8 data_block = 0;
	memset(adv_data, 0, 18);
	
//	NRF_LOG_INFO("two adv");
	if(adv_loop >= adv_prm.adv_delay)
	{
		
		data_block++;
		//NRF_LOG_INFO("data_block is %d", data_block);
		adv_loop = 0;
		CMCN_Get();
		//NRF_LOG_INFO("data_block is %d", data_block);
		switch(data_block)
		{
			case 1:
			//	NRF_LOG_INFO("ADV_DATA1");
				Param_ADV_Data_Get(adv_data, ADV_DATA1);
				break;
			case 2:
			//	NRF_LOG_INFO("ADV_DATA2");
				Param_ADV_Data_Get(adv_data, ADV_DATA2);
				data_block = 0;
				break;
			default:
				data_block = 0;
				break;
		}
		BLE_Beacon_Stop();
		BLE_ADV_Data(adv_data);
		BLE_Beacon_Start();
	}
	
}
void Param_Get_MacAddr(void)
{
	ret_code_t err_code;
	ble_gap_addr_t addr;
	
	err_code = sd_ble_gap_addr_get(&addr);
	APP_ERROR_CHECK(err_code);
	NRF_LOG_INFO("MAC addr*****************");
	NRF_LOG_INFO("%02x, %02x, %02x, %02x, %02x, %02x,", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
	NRF_LOG_INFO("*****************MAC addr");
	addr.addr[0] += 1;
	#if 0
	addr.addr[0] = 0x11;
	addr.addr[1] = 0x22;
	addr.addr[2] = 0x33;
	addr.addr[3] = 0x44;
	addr.addr[4] = 0x55;
	addr.addr[5] = 0x66;
	#endif
	
	
	//err_code = sd_ble_gap_addr_set(&addr);
	APP_ERROR_CHECK(err_code);
}
void Param_Set_PA_LNA(U32 gpio_pa_pin, U32 gpio_lna_pin)
{
    ret_code_t err_code;

    static const uint32_t gpio_toggle_ch = 0;
    static const uint32_t ppi_set_ch = 0;
    static const uint32_t ppi_clr_ch = 1;
    
    // Configure SoftDevice PA/LNA assist
    ble_opt_t opt;
    memset(&opt, 0, sizeof(ble_opt_t));
    // Common PA/LNA config
    opt.common_opt.pa_lna.gpiote_ch_id  = gpio_toggle_ch;        // GPIOTE channel
    opt.common_opt.pa_lna.ppi_ch_id_clr = ppi_clr_ch;            // PPI channel for pin clearing
    opt.common_opt.pa_lna.ppi_ch_id_set = ppi_set_ch;            // PPI channel for pin setting
    // PA config
    opt.common_opt.pa_lna.pa_cfg.active_high = 1;                // Set the pin to be active high
    opt.common_opt.pa_lna.pa_cfg.enable      = 1;                // Enable toggling
    opt.common_opt.pa_lna.pa_cfg.gpio_pin    = gpio_pa_pin;      // The GPIO pin to toggle
  
    // LNA config
    opt.common_opt.pa_lna.lna_cfg.active_high  = 1;              // Set the pin to be active high
    opt.common_opt.pa_lna.lna_cfg.enable       = 1;              // Enable toggling
    opt.common_opt.pa_lna.lna_cfg.gpio_pin     = gpio_lna_pin;   // The GPIO pin to toggle

    err_code = sd_ble_opt_set(BLE_COMMON_OPT_PA_LNA, &opt);
    APP_ERROR_CHECK(err_code);
}
void Param_PA_Test(void)
{
	nrf_gpio_cfg_output(PA_PIN);
	nrf_gpio_cfg_output(LNA_PIN);
//	nrf_gpio_pin_set(PA_PIN);
//	nrf_gpio_pin_clear(LNA_PIN);
	nrf_gpio_pin_clear(LNA_PIN);
	nrf_gpio_pin_clear(PA_PIN);
}
