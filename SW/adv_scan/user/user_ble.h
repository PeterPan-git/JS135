#ifndef _USER_BLE_H_
#define _USER_BLE_H_
#include "user_head.h"
//#define APP_COMPANY_IDENTIFIER          0xEF13
#define APP_BLE_CONN_CFG_TAG            1                                           /**< Tag that identifies the SoftDevice BLE configuration. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to the first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */                             
#define APP_BLE_OBSERVER_PRIO           3
#define DB_DISCOVERY_INSTANCE_CNT       2  /**< Number of DB Discovery instances. */
#define NUS_SERVICE_UUID_TYPE          BLE_UUID_TYPE_BLE

//#define HEAD_ID_1              0x13ED
//#define HEAD_ID_2              0x0000

void BLE_ADV_Init(U8 *data);
void BLE_ADV_Evt_Handle(ble_adv_evt_t ble_adv_evt);
void BLE_ADV_Start(void);
void BLE_ADV_Stop(void);

void BLE_Scan_Init(void);
void BLE_Scan_Evt_Handler(scan_evt_t const * p_scan_evt);
void BLE_Scan_Start(void);
void BLE_Scan_Stop(void);


void BLE_GAP_Params_Init(void);
void BLE_GAP_Conn_Params_Error_Handler(uint32_t nrf_error);
void BLE_GAP_Conn_Params_Init(void);
void BLE_Disconnect(void);

void BLE_GATT_Init(void);
void BLE_NUS_Data_Handler(ble_nus_evt_t * p_evt);
bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event);
void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context);
void advertising_config_get(ble_adv_modes_config_t * p_config);
void disconnect(uint16_t conn_handle, void * p_context);
void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event);


void BLE_ADV_Updata(U8 *data);
void BLE_Serve_Init(void);
void BLE_QWR_Error_Handler(uint32_t nrf_error);
void BLE_Stack_Init(void);
void BLE_Evt_Handler(ble_evt_t const * p_ble_evt, void * p_context);
#endif


