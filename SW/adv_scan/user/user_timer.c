//#include "user_head.h"
//#include "nrf_drv_timer.h"

//bool rx_status = 0;
//uint32_t test_time = 0;
//unsigned char rx_timeout = 0;

//const nrfx_timer_t TIMER_DELAY = NRFX_TIMER_INSTANCE(3);
//static _timer_com tim_com = 
//{
//	.timer_compare0 = NRFX_TIMER_INSTANCE(0),
//	.timer_compare1 = NRFX_TIMER_INSTANCE(1),
//	.timer_compare2 = NRFX_TIMER_INSTANCE(2),
//	.timer_compare3 = NRFX_TIMER_INSTANCE(3),
//};

//void User_Timer_Delay_event_handler(nrf_timer_event_t event_type, void* p_context)
//{
//	switch (event_type)
//    {
//  
//		case NRF_TIMER_EVENT_COMPARE0:
//            NRF_LOG_INFO("timer INT0");
//            break;

//		case NRF_TIMER_EVENT_COMPARE1:
//			test_time++;
//			if(ADV_Status_Get() == true)
//			{
//			   if(uart_data.adv_sta[0] == true)
//	           {
//					uart_data.adv_timer[0]++;
//					NRF_LOG_INFO("time block0");
//					if(uart_data.adv_timer[0] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block0");
//						uart_data.adv_sta[0] =false;
//						uart_data.adv_timer[0] = 0;
//						Uart_Clean_Block(BLOCK0);
//						uart_data.adv_timeout[0] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[1] == true)
//			   {
//					uart_data.adv_timer[1]++;
//					NRF_LOG_INFO("time block1");
//					if(uart_data.adv_timer[1] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block1");
//						uart_data.adv_sta[1] =false;
//						uart_data.adv_timer[1] = 0;
//						Uart_Clean_Block(BLOCK1);
//						uart_data.adv_timeout[1] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[2] == true)
//			   {
//					uart_data.adv_timer[2]++;
//					NRF_LOG_INFO("time block2");
//					if(uart_data.adv_timer[2] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block2");
//						uart_data.adv_sta[2] =false;
//						uart_data.adv_timer[2] = 0;
//						Uart_Clean_Block(BLOCK2);
//						uart_data.adv_timeout[2] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[3] == true)
//			   {
//					uart_data.adv_timer[3]++;
//					NRF_LOG_INFO("time block3");
//					if(uart_data.adv_timer[3] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block3");
//						uart_data.adv_sta[3] =false;
//						uart_data.adv_timer[3] = 0;
//						Uart_Clean_Block(BLOCK3);
//						uart_data.adv_timeout[3] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[4] == true)
//			   {
//					uart_data.adv_timer[4]++;
//					NRF_LOG_INFO("time block4");
//					if(uart_data.adv_timer[4] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block4");
//						uart_data.adv_sta[4] =false;
//						uart_data.adv_timer[4] = 0;
//						Uart_Clean_Block(BLOCK4);
//						uart_data.adv_timeout[4] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[5] == true)
//			   {
//					uart_data.adv_timer[5]++;
//					NRF_LOG_INFO("time block5");
//					if(uart_data.adv_timer[5] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block5");
//						uart_data.adv_sta[5] =false;
//						uart_data.adv_timer[5] = 0;
//						Uart_Clean_Block(BLOCK5);
//						uart_data.adv_timeout[5] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[6] == true)
//			   {
//					uart_data.adv_timer[6]++;
//					NRF_LOG_INFO("time block6");
//					if(uart_data.adv_timer[6] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block6");
//						uart_data.adv_sta[6] =false;
//						uart_data.adv_timer[6] = 0;
//						Uart_Clean_Block(BLOCK6);
//						uart_data.adv_timeout[6] = true;
//					}
//			   }
//			   if(uart_data.adv_sta[7] == true)
//			   {
//					uart_data.adv_timer[7]++;
//					NRF_LOG_INFO("time block7");
//					if(uart_data.adv_timer[7] >= uart_data.adv_time)
//					{
//						NRF_LOG_INFO("Clean Block7");
//						uart_data.adv_sta[7] =false;
//						uart_data.adv_timer[7] = 0;
//						Uart_Clean_Block(BLOCK7);
//						uart_data.adv_timeout[7] = true;
//					}
//			   }
//			   
//			}
//            break;
//		
//		case NRF_TIMER_EVENT_COMPARE2:
//            //test_time++;
//			
//			//NRF_LOG_INFO("timer INT2");
//		
//            break;
//		case NRF_TIMER_EVENT_COMPARE3:
//          // NRF_LOG_INFO("timer INT3");
//          //test_time++;
//			if(rx_status == true)
//			{
//				rx_timeout++;
//				if(rx_timeout > 30)
//				{
//					rx_status = false;
//					rx_timeout = 0;
//				}
//			}
//            break;
//		
//        default:
//            //Do nothing.
//            break;
//    }
//}



//void User_Timer_Init(void)
//{
//	uint32_t err_code = NRF_SUCCESS;
//	//定时时间1ms
//	uint32_t time_ms[4] = {1, 1, 1, 1}; 
//	//保存定时时间对应的Ticks
//	uint32_t time_ticks[4];

//	//定义定时器配置结构体，并使用默认配置参数初始化结构体
//	nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;


//	//初始化定时器，初始化时会注册timer_led_event_handler事件回调函数
//	err_code = nrfx_timer_init(&tim_com.timer_compare1, &timer_cfg, User_Timer_Delay_event_handler);
//	APP_ERROR_CHECK(err_code);
//	err_code = nrfx_timer_init(&tim_com.timer_compare2, &timer_cfg, User_Timer_Delay_event_handler);
//	APP_ERROR_CHECK(err_code);
//	err_code = nrfx_timer_init(&tim_com.timer_compare3, &timer_cfg, User_Timer_Delay_event_handler);
//	APP_ERROR_CHECK(err_code);

//	//定时时间(单位ms)转换为ticks
//	//time_ticks = nrfx_timer_ms_to_ticks(&tim_com.timer_compare3, time_ms);
//	//time_ticks[0] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare0, time_ms[0]);
//	time_ticks[1] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare1, time_ms[1]);
//	time_ticks[2] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare2, time_ms[2]);
//	time_ticks[3] = nrfx_timer_ms_to_ticks(&tim_com.timer_compare3, time_ms[3]);
//	//NRF_LOG_INFO("time_ticks is %d", time_ticks);
//	//设置定时器捕获/比较通道及该通道的比较值，使能通道的比较中断
//	nrfx_timer_extended_compare(&tim_com.timer_compare1, NRF_TIMER_CC_CHANNEL1, time_ticks[1], NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);
//	nrfx_timer_extended_compare(&tim_com.timer_compare2, NRF_TIMER_CC_CHANNEL2, time_ticks[2], NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK, true);
//	nrfx_timer_extended_compare(&tim_com.timer_compare3, NRF_TIMER_CC_CHANNEL3, time_ticks[3], NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK, true);

//	nrfx_timer_enable(&tim_com.timer_compare1);
//	nrfx_timer_enable(&tim_com.timer_compare2);
//	nrfx_timer_enable(&tim_com.timer_compare3);
//}


