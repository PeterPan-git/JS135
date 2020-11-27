#include "user_head.h"

#define PERIPHERAL_ADVERTISING_LED      BSP_BOARD_LED_2
#define PERIPHERAL_CONNECTED_LED        BSP_BOARD_LED_3
#define CENTRAL_SCANNING_LED            BSP_BOARD_LED_0
#define CENTRAL_CONNECTED_LED           BSP_BOARD_LED_1

#define APP_COMPANY_IDENTIFIER          0xEF13
#define DEVICE_NAME                     "JS_135"                                 /**< Name of device used for advertising. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                                         /**< The advertising interval (in units of 0.625 ms). This value corresponds to 187.5 ms. */

#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_CONN_CFG_TAG            1                                           /**< Tag that identifies the SoftDevice BLE configuration. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to the first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

                                 
#define APP_BLE_OBSERVER_PRIO           3

#define DB_DISCOVERY_INSTANCE_CNT       2  /**< Number of DB Discovery instances. */


#define NUS_SERVICE_UUID_TYPE          BLE_UUID_TYPE_BLE// BLE_UUID_TYPE_VENDOR_BEGIN
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};  






NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                    /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);
NRF_BLE_GATT_DEF(m_gatt);                                              /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr); 
//NRF_BLE_QWRS_DEF(m_qwr, NRF_SDH_BLE_TOTAL_LINK_COUNT);                 /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                    /**< Advertising module instance. */
NRF_BLE_SCAN_DEF(m_scan);                                           /**< Scanning module instance. */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);

//static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3; 
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; 

/**@brief Names that the central application scans for, and that are advertised by the peripherals.
 *  If these are set to empty strings, the UUIDs defined below are used.
 */
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

static bool con_sta = false;
void Set_Ble_Connect_Status(bool status)
{
	con_sta = status;
}
bool Get_Ble_Connect_Status(void)
{
	return con_sta;
}
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
void scan_start(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
	Scan_Status_Set(SCAN_OPEN);
}

void Scan_Stop(void)
{
	nrf_ble_scan_stop();
	Scan_Status_Set(SCAN_CLOSE);
}
static void scan_evt_handler(scan_evt_t const * p_scan_evt)
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
			   APP_Data_Receice(p_adv);
				 

         } break;
				 //扫描超时事件
         case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
         {
             NRF_LOG_INFO("Scan timed out.");
					   //重启扫描
             scan_start();
         } break;

         default:
             break;
    }
}

/**@brief Function for initialization the scanning and setting the filters.
 */
static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.p_scan_param = &m_scan_param;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
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


/**@brief Function for handling advertising events.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
	_adv_two sta;
	ADV_Two_Get(&sta);
	
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
        {
            NRF_LOG_INFO("Fast advertising.");
            //bsp_board_led_on(PERIPHERAL_ADVERTISING_LED);
        } break;

        case BLE_ADV_EVT_IDLE:
        {
			adv_updata();
			NRF_LOG_INFO("BLE_ADV_EVT_IDLE.");
            ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            // No implementation needed.
            break;
    }
}

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
			Set_Ble_Connect_Status(BLE_CONNECT);
            NRF_LOG_INFO("Connected");
//            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
//            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
			Set_Ble_Connect_Status(BLE_DISCONNECT);
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



/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupts.
 */
static void ble_stack_init(void)
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
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for initializing the GAP.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device, including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
	 uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
	_adv_param adv_params;

	ADV_Get_Param(&adv_params);
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


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
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
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}




/**@brief Function for handling Queued Write module errors.
 *
 * @details A pointer to this function is passed to each service that may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code that contains information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void UART_Evt_Handle(app_uart_evt_t * p_event)
{
	switch (p_event->evt_type)
	{
		case APP_UART_DATA_READY:
		rx_status = true;

		if(rx_timeout < 30)
		{
		UNUSED_VARIABLE(app_uart_get(&user_rx_buf[rx_inde]));
		rx_inde++;
		rx_timeout = 0;
	}

	//NRF_LOG_INFO("rx_inde is %d", rx_inde);
	break;

	case APP_UART_COMMUNICATION_ERROR:
		NRF_LOG_INFO("@@uart APP_UART_COMMUNICATION_ERROR error");
		APP_ERROR_HANDLER(p_event->data.error_communication);

	break;

	case APP_UART_FIFO_ERROR:
		NRF_LOG_INFO("uart APP_UART_FIFO_ERROR error");
		APP_ERROR_HANDLER(p_event->data.error_code);
	break;
	
	case APP_UART_DATA:
		NRF_LOG_INFO("uart APP_UART_DATA error");
	break;
	
	default:
	break;
	}
}	
void nus_data_handler(ble_nus_evt_t * p_evt)
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
/**@brief Function for initializing services that are be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void services_init(void)
{
    uint32_t           err_code;
    ble_nus_init_t     nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the advertising functionality.
 */
void advertising_init(void)
{
	uint32_t               err_code;
	U8 data[20];
	ADV_Data_Get(data, ADV_DATA1);
#if 1	
	ble_advdata_manuf_data_t manuf_specific_data;

	manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;//////
	manuf_specific_data.data.p_data = data;//my_adv_dat;///////
    manuf_specific_data.data.size   = sizeof(data);//sizeof(my_adv_dat);//////
#endif
	ble_advertising_init_t init;
	_adv_param adv_params;

	ADV_Get_Param(&adv_params);
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
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
	ADV_Status_Set(ADV_OPEN);
}

void advertising_stop(void)
{
	ret_code_t err_code;
	
	err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
	APP_ERROR_CHECK(err_code);
	ADV_Status_Set(ADV_CLOSE);
}
/**@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop). If there is no pending log operation,
          then sleeps until the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Function for initializing the timer.
 */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void UART_Init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       UART_Evt_Handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}	
void adv_updata(void)
{
	ret_code_t err_code;
	ble_advdata_t           adv_data; //广播数据
	ble_advdata_t           sr_data;  //扫描响应数据
	U8 data[18];
	memset(data,0, 18);
	_adv_two sta;
	
	ADV_Two_Get(&sta);
	if(sta.adv_two_status == true)
	{
		if(sta.who_block == false)
		{
			NRF_LOG_INFO("adv first message");
			sta.who_block = true;
			ADV_Two_Set(&sta);
			ADV_Data_Get(data, ADV_DATA1);
		}
		else
		{
			NRF_LOG_INFO("adv second message");
			sta.who_block = false;
			ADV_Two_Set(&sta);
			ADV_Data_Get(data, ADV_DATA2);
		}
	}
	else
	{
		ADV_Data_Get(data, ADV_DATA1);
	}
	ble_advdata_manuf_data_t manuf_specific_data;
	//  ble_advdata_service_data_t serve_data;

	//制造商ID，0x0059是Nordic的厂商ID
	manuf_specific_data.company_identifier = 0x13EF;
	//指向制造商自定义的数据
	manuf_specific_data.data.p_data = data;
	//制造商自定义的数据大小(字节数)
	manuf_specific_data.data.size   = sizeof(data);
	//	serve_data.service_uuid = 0x1234;
	//	serve_data.data.p_data=  my_adv_data;
	//	serve_data.data.size = sizeof(my_adv_data);


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
/**@brief Application main function.
 */

void Uart_Data_Choose(void)
{
	uint32_t  err_code;
	uint8_t head_status = 0; 

	head_status = user_rx_buf[0];
	if((rx_status == false) && (rx_inde > 1))
	{
		switch(head_status)
		{
			case HEAD_CMD:
				Uart_Cmd((char*)user_tx_buf, (char*)user_rx_buf);
				rx_inde = 0;
				break;

			case HEAD_DATA:
				NRF_LOG_INFO("HEAD_DATA");
				
				Uart_Data(user_tx_buf, user_rx_buf);
				Uart_Data_Transfer();
				
				advertising_stop();
				adv_updata();
				advertising_init();
				advertising_start();
				
				rx_inde = 0;
				break;
				
			default:		
		        NRF_LOG_HEXDUMP_DEBUG(user_rx_buf, rx_inde);
		        do
		        {
		            uint16_t length = (uint16_t)rx_inde;
		            err_code = ble_nus_data_send(&m_nus, user_rx_buf, &length, m_conn_handle);
		            if ((err_code != NRF_ERROR_INVALID_STATE) &&
		                (err_code != NRF_ERROR_RESOURCES) &&
		                (err_code != NRF_ERROR_NOT_FOUND))
		            {
		                APP_ERROR_CHECK(err_code);
		            }
		        } while (err_code == NRF_ERROR_RESOURCES);
				rx_inde = 0;
				break;
		}
	}
	
}

void Change_MacAddr(void)
{
	ret_code_t err_code;
	ble_gap_addr_t addr;
	
	err_code = sd_ble_gap_addr_get(&addr);
	APP_ERROR_CHECK(err_code);
	NRF_LOG_INFO("MAC addr*****************");
	NRF_LOG_INFO("%02x, %02x, %02x, %02x, %02x, %02x,", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
	NRF_LOG_INFO("*****************MAC addr");
	//addr.addr[0] += 1;
	#if 0
	addr.addr[0] = 0x11;
	addr.addr[1] = 0x22;
	addr.addr[2] = 0x33;
	addr.addr[3] = 0x44;
	addr.addr[4] = 0x55;
	addr.addr[5] = 0x66;
	#endif
	
	
	err_code = sd_ble_gap_addr_set(&addr);
	APP_ERROR_CHECK(err_code);
}

void Ble_Disconnect(void)
{
	sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}
/**@brief Function for initializing the application main entry.
 */
int main(void)
{
	log_init();
	ADV_Data_Init();
	timer_init();
	User_Timer_Init();
	UART_Init();
	APP_Data_Init();
	power_management_init();
	ble_stack_init();
	ADV_Data_Init();
	Uart_Data_Init();
	ADV_Param_Init();
	scan_init();
	gap_params_init();
	gatt_init();
	conn_params_init();
	services_init();
	advertising_init();


	//Change_MacAddr();
	advertising_start();
	scan_start();
	
	 while(1)
    {
		
		//Uart_Test();
		//app_uart_put(0x55);
		Uart_Data_Choose();
		ADV_Update_Data();
        idle_state_handle();
		
		if(test_time > 1000000)
		{
			test_time = 0;
		}
		//NRF_LOG_INFO("test_time** is %d", test_time);
		
    }

}
