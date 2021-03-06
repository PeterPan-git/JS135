#ifndef _USER_HEAD_H_
#define _USER_HEAD_H_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_hrs.h"
#include "ble_rscs.h"
#include "ble_hrs_c.h"
#include "ble_rscs_c.h"
#include "ble_conn_state.h"
#include "nrf_fstorage.h"
#include "fds.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_ble_scan.h"
#include "nrf_power.h"
#include "fds.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "bignum.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_bootloader_info.h"
#include "ble_dfu.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "nrf_uart.h"
#include "nrf_uarte.h"
#include "nrfx_uart.h"
#include "nrfx_uarte.h"
#include "nrf_delay.h"
#include "nrfx_wdt.h"
#include "user_app.h"
#include "user_def.h"

#include "user_ble.h"
#include "user_drive.h"
#include "user_cmnc.h"
#include "user_param.h"
#include "user_fds.h"

#endif
