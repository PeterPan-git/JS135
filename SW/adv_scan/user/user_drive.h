#ifndef _USER_DRIVE_H_
#define _USER_DRIVE_H_
#include "user_head.h"
#include "nrf_drv_timer.h"
#include "Nrf_timer.h"

#define UART_TX_BUF_SIZE                256                                        
#define UART_RX_BUF_SIZE                256 
#define GPIO_CONNECT                    5
extern bool rx_status;
extern U16 adv_loop;
extern unsigned char rx_timeout;
extern const nrfx_timer_t TIMER_DELAY;
extern U16 adv_update_tim;
typedef struct
{
	nrfx_timer_t timer_compare0;
	nrfx_timer_t timer_compare1;
	nrfx_timer_t timer_compare2;
	nrfx_timer_t timer_compare3;
}_timer_com;

void Drive_UART_Init(void);
void Drive_UART_Send_String(char* buf, uint16_t len);
void Drive_UART_Evt_Handle(app_uart_evt_t * p_event);
void Drive_Timer_Init(void);
void Drive_Timer0_to_3_Init(void);
void User_Timer_Delay_Event_Handler(nrf_timer_event_t event_type, void* p_context);
void Drive_Power_Manage_Init(void);
void Drive_Idle_State_Handle(void);
void Drive_Log_Init(void);
void Drive_WDT_Init(void);
void Drive_Feed_Dog(void);
void Drive_WDT_Even_Handle(void);
void Drive_GPIO_Connect_State_Init(void);
void Drive_GPIO_Connect_State_Open(void);
void Drive_GPIO_Connect_State_Close(void);

uint64_t get_now(void);
#endif
