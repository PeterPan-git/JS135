#include "user_head.h"
#include "user_ble.h"
NRF_BLE_GQ_DEF(m_ble_gatt_queue, NRF_SDH_BLE_CENTRAL_LINK_COUNT, NRF_BLE_GQ_QUEUE_SIZE);
NRF_BLE_GATT_DEF(m_gatt);                                              
NRF_BLE_QWR_DEF(m_qwr); 
BLE_ADVERTISING_DEF(m_advertising);                                    
NRF_BLE_SCAN_DEF(m_scan);                                           
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);

#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS) 
//static ble_gap_adv_params_t m_adv_params; 
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; 
static char const m_target_periph_name[] = "RRRRR";
static uint8_t              m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t              m_enc_advdata[31];  /**< Buffer for storing an encoded advertising set. */
static uint8_t              m_enc_srdata[31];
/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_con =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = 31
    },
    .scan_rsp_data =
    {
        .p_data = m_enc_srdata,
        .len    = 31

    }
};

static ble_gap_adv_data_t m_adv_name =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,//m_enc_advdata,
        .len    = 31
    },
    .scan_rsp_data =
    {
        .p_data = NULL,
        .len    = 0

    }
};

static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = 31
    },
    .scan_rsp_data =
    {
        .p_data = m_enc_srdata,
        .len    = 31

    }
};

#define APP_BEACON_INFO_LENGTH          0x17                               /**< Total length of information advertised by the Beacon. */
#define APP_ADV_DATA_LENGTH             0x15                               /**< Length of manufacturer specific data in the advertisement. */
#define APP_DEVICE_TYPE                 0x02                               /**< 0x02 refers to Beacon. */
#define APP_MEASURED_RSSI               0xC3                               /**< The Beacon's measured RSSI at 1 meter distance in dBm. */
//#define APP_COMPANY_IDENTIFIER          0x0059                             /**< Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
#define APP_MAJOR_VALUE                 0x01, 0x02                         /**< Major value used to identify Beacons. */
#define APP_MINOR_VALUE                 0x03, 0x04                         /**< Minor value used to identify Beacons. */
#define APP_BEACON_UUID                 0xF1, 0xF2, 0xF3, 0xF4, \
                                        0xF5, 0xF6, 0xF7, 0xF8, \
                                        0xF9, 0xFa, 0xFb, 0xFc, \
                                        0xFd, 0xFe, 0xFf, 0xFF            /**< Proprietary UUID for Beacon. */

static ble_gap_scan_params_t m_scan_param =                 /**< Scan parameters requested for scanning and connection. */
{
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
    .timeout       = NRF_BLE_SCAN_SCAN_DURATION,
    .scan_phys     = BLE_GAP_PHY_1MBPS,
    .extended      = true,
};
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {ADV_SER_UUID, NUS_SERVICE_UUID_TYPE}
};


void Uart_Data_Choose(void)
{
	uint8_t head_status[2]; 
	head_status[0] = user_rx_buf[0];
	head_status[1] = user_rx_buf[1];
	uint16_t length = (uint16_t)rx_inde;
	
	
	if((rx_status == false) && (rx_inde > 1))
	{
	
		if(Param_Get_Ble_Connect_Status())
		{	
			if(head_status[0] == HD_2)
			{
				Uart_Cmd((char*)user_tx_buf, (char*)user_rx_buf);
			}
			else
			{
				ble_nus_data_send(&m_nus, user_rx_buf, &length, m_conn_handle);
			}
				
			rx_inde = 0;
		}
		else
		{
			
			if(head_status[0] == HD_1)
			{
				
				if(head_status[1] == FD_1)
				{
					NRF_LOG_INFO("@2:mcu --> ble by uart");
					if(CMNC_Repeat_Filt(user_rx_buf) == false)
					{
						CMCN_Save(user_rx_buf);
					}
					
					rx_inde = 0;
				}
			}
			else if(head_status[0] == HD_2)
			{
				Uart_Cmd((char*)user_tx_buf, (char*)user_rx_buf);
				rx_inde = 0;
			}
			else
			{
				rx_inde = 0;
			}
		}
	
	}
}
/****************************************************************
*---------ADV configuration---------
*---------------START---------------
***********************************/
void BLE_ADV_Init(U8 *data)
{
	uint32_t               err_code;
	U16 id = data[1];
	id = id << 8;
	id |= data[0];
	
	U8 adv_dat[18];
	memcpy(adv_dat, &data[2], 18);
	
	ble_advdata_manuf_data_t manuf_specific_data;

	manuf_specific_data.company_identifier = id;//////
	manuf_specific_data.data.p_data = adv_dat;//my_adv_dat;///////
    manuf_specific_data.data.size   = 18;//sizeof(adv_data);//sizeof(my_adv_dat);//////
	ble_advertising_init_t init;
	_adv_param adv_params;

	Param_ADV_Get_Param(&adv_params);
    memset(&init, 0, sizeof(init));
	
    //init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;//BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
	init.advdata.p_tx_power_level = &adv_params.tx_power;
	init.advdata.p_manuf_specific_data    = &manuf_specific_data;//////

	init.srdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;
	
    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = adv_params.adv_interval;
	init.config.ble_adv_fast_timeout = 0;//adv_params.adv_timeout;  //目前时间只能设置为0
   //init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    init.evt_handler = BLE_ADV_Evt_Handle;
	

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}
void BLE_ADV_Evt_Handle(ble_adv_evt_t ble_adv_evt)
{
	_adv_two sta;
	Param_ADV_Two_Get(&sta);
	
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
        {
            //NRF_LOG_INFO("Fast advertising.");
            
        } break;

        case BLE_ADV_EVT_IDLE:
        {
            ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            // No implementation needed.
            break;
    }
}
void BLE_ADV_Start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
	if(err_code != NRF_SUCCESS)
	{
		NVIC_SystemReset();
	}
	APP_ERROR_CHECK(err_code);
	
	Param_ADV_Status_Set(ADV_OPEN);
	
}

void BLE_ADV_Stop(void)
{

	ret_code_t err_code;

	err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
	if(err_code != NRF_SUCCESS)
	{
		NVIC_SystemReset();
	}
	APP_ERROR_CHECK(err_code);

	Param_ADV_Status_Set(ADV_CLOSE);
}


/***********************************
*----------------END----------------
*---------ADV configuration---------
****************************************************************/


void BLE_ADV_CON(void)
{
	
	uint32_t      err_code;
	U8 data_adv[18];
    ble_advdata_t srdata;
	ble_gap_adv_params_t adv_params;
	_adv_param adv_pam;
	Param_ADV_Get_Param(&adv_pam);
	
	ble_advdata_t advdata;
	ble_advdata_manuf_data_t manuf_specific_data;
	memset(&advdata, 0, sizeof(advdata));
	memset(&data_adv, 0, sizeof(data_adv));
	manuf_specific_data.company_identifier = 0xEF13;//////
	manuf_specific_data.data.p_data = data_adv;//my_adv_dat;///////
    manuf_specific_data.data.size   = sizeof(data_adv);//sizeof(adv_data);//sizeof(my_adv_dat);//////
	advdata.p_tx_power_level= &adv_pam.tx_power;
	advdata.p_manuf_specific_data = &manuf_specific_data;
	err_code = ble_advdata_encode(&advdata, m_adv_con.adv_data.p_data, &m_adv_con.adv_data.len);
    APP_ERROR_CHECK(err_code);
	
	memset(&srdata, 0, sizeof(srdata));
    srdata.name_type          = BLE_ADVDATA_FULL_NAME;
    srdata.include_appearance = true;
	//srdata.p_tx_power_level= &adv_pam.tx_power;
	srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = m_adv_uuids;
	err_code = ble_advdata_encode(&srdata, m_adv_con.scan_rsp_data.p_data, &m_adv_con.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);
	
	memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.duration        = 0;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = adv_pam.adv_interval;
	
	err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_con, &adv_params);
    APP_ERROR_CHECK(err_code);
	
	adv_mode = 0;
}
void BLE_ADV_Name(void)
{
	uint32_t      err_code;
	ble_advdata_t advdata;
	ble_gap_adv_params_t adv_params;


	memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
	err_code = ble_advdata_encode(&advdata, m_adv_name.adv_data.p_data, &m_adv_name.adv_data.len);
    APP_ERROR_CHECK(err_code);

	memset(&adv_params, 0, sizeof(adv_params));
    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.duration        = 0;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = 32;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_name, &adv_params);
    APP_ERROR_CHECK(err_code);
	
	adv_mode = 1;
}
void BLE_ADV_Data(U8 *data)
{
	uint32_t               err_code;
	U16 id = data[1];
	ble_advdata_manuf_data_t manuf_specific_data;
	ble_advdata_t advdata;
	ble_advdata_t srdata;
	ble_gap_adv_params_t adv_params;
	
	
	id = id << 8;
	id |= data[0];
	U8 adv_dat[18];
	memcpy(adv_dat, &data[2], 18);


	memset(&advdata, 0, sizeof(advdata));
	manuf_specific_data.company_identifier = id;//////
	manuf_specific_data.data.p_data = adv_dat;//my_adv_dat;///////
    manuf_specific_data.data.size   = 18;//sizeof(adv_data);//sizeof(my_adv_dat);//////
	
    //advdata.name_type = BLE_ADVDATA_NO_NAME;
	advdata.flags = BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE;//BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	advdata.p_manuf_specific_data = &manuf_specific_data;
	
    advdata.flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
	
	err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

	memset(&srdata, 0, sizeof(srdata));
    srdata.name_type          = BLE_ADVDATA_FULL_NAME;
    srdata.include_appearance = true;
	err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);
	
	memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.duration        = 0;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = 32;

	
    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
	adv_mode = 2;
}
/****************************************************************
*---------Beacon configuration--------
*----------------START----------------
*************************************/


void BLE_Beacon_Start(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
	if(err_code != NRF_SUCCESS)
	{
		NVIC_SystemReset();
	}
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    APP_ERROR_CHECK(err_code);
	
	
	Param_ADV_Status_Set(ADV_OPEN);
}
void BLE_Beacon_Stop(void)
{
	ret_code_t err_code;

	err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
	if(err_code != NRF_SUCCESS)
	{
		NVIC_SystemReset();
	}
	APP_ERROR_CHECK(err_code);

	Param_ADV_Status_Set(ADV_CLOSE);
}
/*************************************
*-----------------END-----------------
*---------Beacon configuration--------
****************************************************************/



/****************************************************************
*---------Scan configuration--------
*---------------START---------------
***********************************/
void BLE_Scan_Init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.p_scan_param = &m_scan_param;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, BLE_Scan_Evt_Handler);
    APP_ERROR_CHECK(err_code);

    if (strlen(m_target_periph_name) != 0)
    {
        err_code = nrf_ble_scan_filter_set(&m_scan, 
                                           SCAN_NAME_FILTER, 
                                           m_target_periph_name);
        APP_ERROR_CHECK(err_code);
    }

    err_code = nrf_ble_scan_filters_enable(&m_scan, 
                                           NRF_BLE_SCAN_ALL_FILTER, 
                                           false);
    APP_ERROR_CHECK(err_code);

}

void BLE_Scan_Evt_Handler(scan_evt_t const * p_scan_evt)
{
  //翻转LED指示灯D4的状态，指示扫描到设备
	//  nrf_gpio_pin_toggle(LED_4);
    //判断事件类型
    switch(p_scan_evt->scan_evt_id)
    {
         //因为不过滤扫描信息，即不设置过滤条件，所以只需关注NRF_BLE_SCAN_EVT_NOT_FOUND事件
		case NRF_BLE_SCAN_EVT_NOT_FOUND:
		{
				//定义广播报告结构体指针p_adv并指向扫描事件结构体中的p_not_found
			   ble_gap_evt_adv_report_t const * p_adv =
					   p_scan_evt->params.p_not_found;					 
				 //查找广播数据中的设备名称，找到后通过串口打印出设备名称
			   CMNC_APP_MCU_Data_Receice(p_adv);
			  // CMCN_APP_MCU_Data_Send();
				 

         } break;
				 //扫描超时事件
         case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
         {
             NRF_LOG_INFO("Scan timed out.");
					   //重启扫描
            BLE_Scan_Start();
         } break;

         default:
             break;
    }
}
void BLE_Scan_Start(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
	Param_Scan_Status_Set(SCAN_OPEN);
}

void BLE_Scan_Stop(void)
{
	nrf_ble_scan_stop();
	Param_Scan_Status_Set(SCAN_CLOSE);
}


/***********************************
*----------------END----------------
*---------Scan configuration--------
****************************************************************/




/****************************************************************
*----------GAP configuration---------
*---------------START----------------
************************************/
void BLE_GAP_Params_Init(void)
{
	 uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
	_adv_param adv_params;

	Param_ADV_Get_Param(&adv_params);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,(const uint8_t *)adv_params.adv_name,strlen(adv_params.adv_name));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = m_scan.conn_params.min_conn_interval;
    gap_conn_params.max_conn_interval = m_scan.conn_params.max_conn_interval;
    gap_conn_params.slave_latency     = m_scan.conn_params.slave_latency;
    gap_conn_params.conn_sup_timeout  = m_scan.conn_params.conn_sup_timeout;
	
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	//Param_Set_PA_LNA(PA_PIN, LNA_PIN);
    APP_ERROR_CHECK(err_code);
	Param_Set_PA_LNA(PA_PIN, LNA_PIN);
}
void BLE_GAP_Conn_Params_Init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_CONN_HANDLE_INVALID; // Start upon connection.
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;  // Ignore events.
    cp_init.error_handler                  = BLE_GAP_Conn_Params_Error_Handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}
void BLE_GAP_Conn_Params_Error_Handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
void BLE_Disconnect(void)
{
	sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}
/************************************
*----------------END-----------------
*----------GAP configuration---------
****************************************************************/




/****************************************************************
*---------GATT configuration--------
*---------------START----------------
************************************/
void BLE_GATT_Init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/************************************
*----------------END-----------------
*---------GATT configuration--------
****************************************************************/




/****************************************************************
*----------NUS configuration---------
*---------------START----------------
************************************/
void BLE_NUS_Data_Handler(ble_nus_evt_t * p_evt)
{

    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        uint32_t err_code;
//		NRF_LOG_INFO("receive success");
//        NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            do
            {
                err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                 //   NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
                    APP_ERROR_CHECK(err_code);
                }
            } while (err_code == NRF_ERROR_BUSY);
        }
        if (p_evt->params.rx_data.p_data[p_evt->params.rx_data.length - 1] == '\r')
        {
            while (app_uart_put('\n') == NRF_ERROR_BUSY);
        }
    }

}
/************************************
*----------------END-----------------
*----------NUS configuration---------
****************************************************************/

/****************************************************************
*----------DFU configuration---------
*---------------START----------------
************************************/
bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
           // NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    //NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);


void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = 300;
    p_config->ble_adv_fast_timeout  = 18000;
}
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = buttonless_dfu_sdh_state_observer,
};

void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}

void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            advertising_config_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}
/************************************
*----------------END-----------------
*----------DFU configuration---------
****************************************************************/

/****************************************************************
*---------Update ADV data configuration--------
*--------------------START---------------------
**********************************************/
void BLE_ADV_Updata(U8 *data)
{
	ret_code_t err_code;
	ble_advdata_t           adv_data; //广播数据
	ble_advdata_t           sr_data;  //扫描响应数据
	U16 id = data[1];
	id = id << 8;
	id |= data[0];
	
	U8 adv_dat[18];
	memcpy(adv_dat, &data[2], 18);
	
	_adv_param adv_params;
	Param_ADV_Get_Param(&adv_params);

	ble_advdata_manuf_data_t manuf_specific_data;
	//  ble_advdata_service_data_t serve_data;

	//制造商ID，0x0059是Nordic的厂商ID
	manuf_specific_data.company_identifier = id;
	//指向制造商自定义的数据
	manuf_specific_data.data.p_data = adv_dat;
	//制造商自定义的数据大小(字节数)
	manuf_specific_data.data.size   = 18;


	memset(&adv_data, 0, sizeof(adv_data));
	memset(&sr_data, 0, sizeof(sr_data));
	//adv_data.name_type               = BLE_ADVDATA_FULL_NAME;
	//是否包含外观：包含
	adv_data.include_appearance      = false;
	//adv_data.p_tx_power_level= &adv_params.tx_power;
	//广播中加入制造商自定义数据
	adv_data.p_manuf_specific_data    = &manuf_specific_data;
	// adv_data.p_service_data_array = &serve_data;
	//Flag:一般可发现模式，不支持BR/EDR
	adv_data.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	
	//NRF_LOG_INFO("no data adv");
	sr_data.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
	sr_data.uuids_complete.p_uuids  = m_adv_uuids;
	sr_data.name_type = BLE_ADVDATA_FULL_NAME; 
	
	
	//更新广播内容
	err_code = ble_advertising_advdata_update(&m_advertising, &adv_data, &sr_data);
	APP_ERROR_CHECK(err_code);
}


/**********************************************
*---------------------END----------------------
*---------Update ADV data configuration--------
****************************************************************/



/****************************************************************
*---------Serve configuration--------
*---------------START----------------
************************************/
void BLE_Serve_Init(void)
{
    uint32_t           err_code;
    ble_nus_init_t     nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};
	ble_dfu_buttonless_init_t dfus_init = {0};
    // Initialize Queued Write Module.
    qwr_init.error_handler = BLE_QWR_Error_Handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = BLE_NUS_Data_Handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
	//Initialize DFU
	dfus_init.evt_handler = ble_dfu_evt_handler;
    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
}

void BLE_QWR_Error_Handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
/************************************
*----------------END-----------------
*---------Serve configuration--------
****************************************************************/




/****************************************************************
*---------Stack configuration--------
*---------------START----------------
************************************/
void BLE_Stack_Init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, BLE_Evt_Handler, NULL);
}

void BLE_Evt_Handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
			Param_Set_Ble_Connect_Status(BLE_CONNECT);
			Drive_GPIO_Connect_State_Open();
			BLE_Scan_Stop();
			Param_ADV_Status_Set(ADV_CLOSE);
            NRF_LOG_INFO("Connected");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
			Param_Set_Ble_Connect_Status(BLE_DISCONNECT);
			Drive_GPIO_Connect_State_Close();
			Param_ADV_Status_Set(ADV_OPEN);
			BLE_Scan_Start();
			BLE_Beacon_Start();
            NRF_LOG_INFO("Disconnected");
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}

/************************************
*----------------END-----------------
*---------Stack configuration--------
****************************************************************/
