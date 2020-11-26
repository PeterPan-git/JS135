#ifndef _USER_TIMER_H_
#define _USER_TIMER_H_
#include "nrf_drv_timer.h"
#include "Nrf_timer.h"
#include "user_head.h"
extern bool rx_status;
extern uint32_t test_time;
extern unsigned char rx_timeout;
extern const nrfx_timer_t TIMER_DELAY;

typedef struct
{
	nrfx_timer_t timer_compare0;
	nrfx_timer_t timer_compare1;
	nrfx_timer_t timer_compare2;
	nrfx_timer_t timer_compare3;
}_timer_com;
void User_Timer_Init(void);
void User_Timer_Delay_event_handler(nrf_timer_event_t event_type, void* p_context);
#endif
