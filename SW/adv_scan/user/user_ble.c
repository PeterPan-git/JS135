#include "user_head.h"
#include "user_ble.h"
NRF_BLE_GQ_DEF(m_ble_gatt_queue, NRF_SDH_BLE_CENTRAL_LINK_COUNT, NRF_BLE_GQ_QUEUE_SIZE);
NRF_BLE_GATT_DEF(m_gatt);                                              
NRF_BLE_QWR_DEF(m_qwr); 
BLE_ADVERTISING_DEF(m_advertising);                                    
NRF_BLE_SCAN_DEF(m_scan);                                           
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);


static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; 
static char const m_target_periph_name[] = "RRRRR";
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
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};


/****************************************************************
*---------ADV configuration---------
*---------------START---------------
***********************************/
void BLE_ADV_Init(U16 ID, U8 *data)
{
	uint32_t               err_code;
//	U8 data[20];
//	Param_ADV_Data_Get(data, ADV_DATA1);
#if 1	
	ble_advdata_manuf_data_t manuf_specific_data;

	manuf_specific_data.company_identifier = ID;//////
	manuf_specific_data.data.p_data = data;//my_adv_dat;///////
    manuf_specific_data.data.size   = 18;//sizeof(adv_data);//sizeof(my_adv_dat);//////
#endif
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
	init.config.ble_adv_fast_timeout = adv_params.adv_timeout;  //目前时间只能设置为0
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
			//adv_updata();
			//NRF_LOG_INFO("BLE_ADV_EVT_IDLE.");
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
    APP_ERROR_CHECK(err_code);
	Param_ADV_Status_Set(ADV_OPEN);
}

void BLE_ADV_Stop(void)
{
	ret_code_t err_code;
	
	err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
	APP_ERROR_CHECK(err_code);
	Param_ADV_Status_Set(ADV_CLOSE);
}


/***********************************
*----------------END----------------
*---------ADV configuration---------
****************************************************************/



/****************************************************************
*---------Beacon configuration--------
*----------------START----------------
*************************************/

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
            // scan_start();
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
    APP_ERROR_CHECK(err_code);
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
		NRF_LOG_INFO("receive success");
        NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            do
            {
                err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                    NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
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
*---------Update ADV data configuration--------
*--------------------START---------------------
**********************************************/
void BLE_ADV_Updata(U16 ID, U8 *data)
{
	ret_code_t err_code;
	ble_advdata_t           adv_data; //广播数据
	ble_advdata_t           sr_data;  //扫描响应数据
//	U8 data[18];
//	memset(data,0, 18);
//	_adv_two sta;
//	
//	Param_ADV_Two_Get(&sta);
//	if(sta.adv_two_status == true)
//	{
//		if(sta.who_block == false)
//		{
//		//	NRF_LOG_INFO("adv first message");
//			sta.who_block = true;
//			Param_ADV_Two_Set(&sta);
//			Param_ADV_Data_Get(data, ADV_DATA1);
//		}
//		else
//		{
//		//	NRF_LOG_INFO("adv second message");
//			sta.who_block = false;
//			Param_ADV_Two_Set(&sta);
//			Param_ADV_Data_Get(data, ADV_DATA2);
//		}
//	}
//	else
//	{
//		Param_ADV_Data_Get(data, ADV_DATA1);
//	}
	ble_advdata_manuf_data_t manuf_specific_data;
	//  ble_advdata_service_data_t serve_data;

	//制造商ID，0x0059是Nordic的厂商ID
	manuf_specific_data.company_identifier = ID;
	//指向制造商自定义的数据
	manuf_specific_data.data.p_data = data;
	//制造商自定义的数据大小(字节数)
	manuf_specific_data.data.size   = 18;
	


	memset(&adv_data, 0, sizeof(adv_data));
	memset(&sr_data, 0, sizeof(sr_data));
	//adv_data.name_type               = BLE_ADVDATA_FULL_NAME;
	//是否包含外观：包含
	adv_data.include_appearance      = false;
	//广播中加入制造商自定义数据
	adv_data.p_manuf_specific_data    = &manuf_specific_data;
	// adv_data.p_service_data_array = &serve_data;
	//Flag:一般可发现模式，不支持BR/EDR
	adv_data.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
		
	//UUID放到扫描响应里面
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

    // Initialize Queued Write Module.
    qwr_init.error_handler = BLE_QWR_Error_Handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = BLE_NUS_Data_Handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
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
            NRF_LOG_INFO("Connected");
//            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
//            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
			Param_Set_Ble_Connect_Status(BLE_DISCONNECT);
            NRF_LOG_INFO("Disconnected");
            // LED indication will be changed when advertising starts.
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
