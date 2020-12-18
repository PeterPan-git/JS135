#include "user_head.h"
#include "nrfx_wdt.h"
bool rx_status = 0;
U16 adv_loop = 0;
unsigned char rx_timeout = 0;
nrfx_wdt_channel_id m_channel_id;
const nrfx_timer_t TIMER_DELAY = NRFX_TIMER_INSTANCE(3);
static _timer_com tim_com = 
{
	.timer_compare0 = NRFX_TIMER_INSTANCE(0),
	.timer_compare1 = NRFX_TIMER_INSTANCE(1),
	.timer_compare2 = NRFX_TIMER_INSTANCE(2),
	.timer_compare3 = NRFX_TIMER_INSTANCE(3),
};
/***********************************
*-----Serial port configuration-----
*---------------START---------------
***********************************/

void Drive_UART_Init(void)
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
        .baud_rate    = NRF_UART_BAUDRATE_57600
#else
        .baud_rate    = NRF_UART_BAUDRATE_57600
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       Drive_UART_Evt_Handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}	
void Drive_UART_Send_String(char* buf, uint16_t len)
{
	uint8_t loop;

	for(loop = 0; loop < len; loop++)
	{
		app_uart_put(buf[loop]);
	}
}
void Drive_UART_Evt_Handle(app_uart_evt_t * p_event)
{
	switch (p_event->evt_type)
	{
		case APP_UART_DATA_READY:
			//NRF_LOG_INFO("uart APP_UART_DATA_READY");
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
/***********************************
*----------------END----------------
*-----Serial port configuration-----
*****************************************************************/


/*****************************************************************
*--------Timer configuration--------
*---------------START---------------
***********************************/
void Drive_Timer_Init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void Drive_Timer0_to_3_Init(void)
{
	uint32_t err_code = NRF_SUCCESS;
	//定时时间1ms
	uint32_t time_ms[4] = {1, 1, 1, 1}; 
	//保存定时时间对应的Ticks
	uint32_t time_ticks[4];

	//定义定时器配置结构体，并使用默认配置参数初始化结构体
	nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;


	//初始化定时器，初始化时会注册timer_led_event_handler事件回调函数
	err_code = nrfx_timer_init(&tim_com.timer_compare1, &timer_cfg, User_Timer_Delay_Event_Handler);
	APP_ERROR_CHECK(err_code);
	err_code = nrfx_timer_init(&tim_com.timer_compare2, &timer_cfg, User_Timer_Delay_Event_Handler);
	APP_ERROR_CHECK(err_code);
	err_code = nrfx_timer_init(&tim_com.timer_compare3, &timer_cfg, User_Timer_Delay_Event_Handler);
	APP_ERROR_CHECK(err_code);

	//定时时间(单位ms)转换为ticks
	//time_ticks = nrfx_timer_ms_to_ticks(&tim_com.timer_compare3, time_ms);
	//time_ticks[0] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare0, time_ms[0]);
	time_ticks[1] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare1, time_ms[1]);
	time_ticks[2] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare2, time_ms[2]);
	time_ticks[3] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare3, time_ms[3]);
	//NRF_LOG_INFO("time_ticks is %d", time_ticks);
	//设置定时器捕获/比较通道及该通道的比较值，使能通道的比较中断
	nrfx_timer_extended_compare(&tim_com.timer_compare1, NRF_TIMER_CC_CHANNEL1, time_ticks[1], NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);
	nrfx_timer_extended_compare(&tim_com.timer_compare2, NRF_TIMER_CC_CHANNEL2, time_ticks[2], NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK, true);
	nrfx_timer_extended_compare(&tim_com.timer_compare3, NRF_TIMER_CC_CHANNEL3, time_ticks[3], NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK, true);

	nrfx_timer_enable(&tim_com.timer_compare1);
	nrfx_timer_enable(&tim_com.timer_compare2);
	nrfx_timer_enable(&tim_com.timer_compare3);
}

void User_Timer_Delay_Event_Handler(nrf_timer_event_t event_type, void* p_context)
{
	if(adv_loop > 1000)
	{
		adv_loop = 0;
	}
	switch (event_type)
    {
  
		case NRF_TIMER_EVENT_COMPARE0:
            NRF_LOG_INFO("timer INT0");
            break;

		case NRF_TIMER_EVENT_COMPARE1:
			
			if(Param_ADV_Status_Get() == true)
			{
				_adv_param adv_params;
				Param_ADV_Get_Param(&adv_params);

				if(adv_params.adv_timeout == 0)
				{
					//NRF_LOG_INFO("return");
					return;
				}
			   if(uart_data.adv_sta[0] == true)
	           {
					uart_data.adv_timer[0]++;
					//NRF_LOG_INFO("time block0");
					if(uart_data.adv_timer[0] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block0");
						uart_data.adv_sta[0] =false;
						uart_data.adv_timer[0] = 0;
						Uart_Clean_Block(BLOCK0);
						uart_data.adv_timeout[0] = true;
					}
			   }
			   if(uart_data.adv_sta[1] == true)
			   {
					uart_data.adv_timer[1]++;
					//NRF_LOG_INFO("time block1");
					if(uart_data.adv_timer[1] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block1");
						uart_data.adv_sta[1] =false;
						uart_data.adv_timer[1] = 0;
						Uart_Clean_Block(BLOCK1);
						uart_data.adv_timeout[1] = true;
					}
			   }
			   if(uart_data.adv_sta[2] == true)
			   {
					uart_data.adv_timer[2]++;
					//NRF_LOG_INFO("time block2");
					if(uart_data.adv_timer[2] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block2");
						uart_data.adv_sta[2] =false;
						uart_data.adv_timer[2] = 0;
						Uart_Clean_Block(BLOCK2);
						uart_data.adv_timeout[2] = true;
					}
			   }
			   if(uart_data.adv_sta[3] == true)
			   {
					uart_data.adv_timer[3]++;
					//NRF_LOG_INFO("time block3");
					if(uart_data.adv_timer[3] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block3");
						uart_data.adv_sta[3] =false;
						uart_data.adv_timer[3] = 0;
						Uart_Clean_Block(BLOCK3);
						uart_data.adv_timeout[3] = true;
					}
			   }
			   if(uart_data.adv_sta[4] == true)
			   {
					uart_data.adv_timer[4]++;
					//NRF_LOG_INFO("time block4");
					if(uart_data.adv_timer[4] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block4");
						uart_data.adv_sta[4] =false;
						uart_data.adv_timer[4] = 0;
						Uart_Clean_Block(BLOCK4);
						uart_data.adv_timeout[4] = true;
					}
			   }
			   if(uart_data.adv_sta[5] == true)
			   {
					uart_data.adv_timer[5]++;
					//NRF_LOG_INFO("time block5");
					if(uart_data.adv_timer[5] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block5");
						uart_data.adv_sta[5] =false;
						uart_data.adv_timer[5] = 0;
						Uart_Clean_Block(BLOCK5);
						uart_data.adv_timeout[5] = true;
					}
			   }
			   if(uart_data.adv_sta[6] == true)
			   {
					uart_data.adv_timer[6]++;
					//NRF_LOG_INFO("time block6");
					if(uart_data.adv_timer[6] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block6");
						uart_data.adv_sta[6] =false;
						uart_data.adv_timer[6] = 0;
						Uart_Clean_Block(BLOCK6);
						uart_data.adv_timeout[6] = true;
					}
			   }
			   if(uart_data.adv_sta[7] == true)
			   {
					uart_data.adv_timer[7]++;
					//NRF_LOG_INFO("time block7");
					if(uart_data.adv_timer[7] >= adv_params.adv_timeout)
					{
						NRF_LOG_INFO("Clean Block7");
						uart_data.adv_sta[7] =false;
						uart_data.adv_timer[7] = 0;
						Uart_Clean_Block(BLOCK7);
						uart_data.adv_timeout[7] = true;
					}
			   }
			   
			}
            break;
		
		case NRF_TIMER_EVENT_COMPARE2:
			adv_loop++;
            break;
		case NRF_TIMER_EVENT_COMPARE3:
         
			if(rx_status == true)
			{
				rx_timeout++;
				if(rx_timeout > 30)
				{
					rx_status = false;
					rx_timeout = 0;
				}
			}
            break;
		
        default:
            //Do nothing.
            break;
    }
}
/***********************************
*----------------END----------------
*--------Timer configuration--------
****************************************************************/


/****************************************************************
*--------Power configuration--------
*---------------START---------------
***********************************/
void Drive_Power_Manage_Init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}
/***********************************
*----------------END----------------
*--------Power configuration--------
****************************************************************/


/****************************************************************
*---------Idle configuration--------
*---------------START---------------
***********************************/
void Drive_Idle_State_Handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}
/***********************************
*----------------END----------------
*---------Idle configuration--------
****************************************************************/


/****************************************************************
*---------Log configuration--------
*---------------START---------------
***********************************/
void Drive_Log_Init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
/***********************************
*----------------END----------------
*---------Log configuration--------
****************************************************************/

/****************************************************************
*---------WDT configuration--------
*---------------START---------------
***********************************/
void Drive_WDT_Init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    
    //定义WDT配置结构体并使用
    nrfx_wdt_config_t config = NRFX_WDT_DEAFULT_CONFIG;
	  //初始化WDT
    err_code = nrfx_wdt_init(&config, Drive_WDT_Even_Handle);
    APP_ERROR_CHECK(err_code);
	  //申请喂狗通道，也就是使用哪个
    err_code = nrfx_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
	  //启动WDT
    nrfx_wdt_enable();       
}	
void Drive_Feed_Dog(void)
{
	nrfx_wdt_channel_feed(m_channel_id);
}
void Drive_WDT_Even_Handle(void)
{
	NRF_LOG_INFO("Drive_WDT_Even_Handle");
}
/***********************************
*----------------END----------------
*---------Log configuration--------
****************************************************************/


void Drive_GPIO_Connect_State_Init(void)
{
	//nrf_gpio_cfg_input(GPIO_CONNECT, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_output(GPIO_CONNECT);
	Drive_GPIO_Connect_State_Close();
}
void Drive_GPIO_Connect_State_Open(void)
{
	nrf_gpio_pin_clear(GPIO_CONNECT);
}
void Drive_GPIO_Connect_State_Close(void)
{
	nrf_gpio_pin_set(GPIO_CONNECT);
}

