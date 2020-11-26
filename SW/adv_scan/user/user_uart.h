#ifndef _USER_UART_H_
#define _USER_UART_H_
#include "user_head.h"
#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256  

#define HEAD_DATA                       0x13
#define HEAD_CMD                        '<'         
#define USER_UART_BUF_SIZE           50

#define JS_VERSION                      "V1.0"
#define BLE_CONNECT      true
#define BLE_DISCONNECT   false

extern uint8_t rx_inde;
extern uint8_t user_tx_buf[USER_UART_BUF_SIZE];
extern uint8_t user_rx_buf[USER_UART_BUF_SIZE];

typedef struct
{
	bool adv_sta[8];                   //每组数据广播状态
	bool adv_timeout[8];               //每组数字广播超时
	uint32_t adv_timer[8];              //每组数据的广播计时
	uint32_t adv_time;                  //广播时间
	uint8_t ret_block[8][4];           //数据存储区域
	uint8_t length;                    //单次接收到的数据长度
	uint8_t cs;                        //数据校验
}_uart_data;

enum
{
	LENGTH_1 = 1,
	LENGTH_2,
	LENGTH_3,
	LENGTH_4,
};

typedef enum
{
	BLOCK0 = 0,
	BLOCK1,
	BLOCK2,
	BLOCK3,
	BLOCK4,
	BLOCK5,
	BLOCK6,
	BLOCK7,
		
}E_BLOCK;
extern _uart_data uart_data;
//extern E_BLOCK e_block;

void Uart_Data_Init(void);
void Uart_Cmd(char *tx_buf, char *rx_buf);
void Uart_Data(uint8_t *tx_buf, uint8_t *rx_buf);
//void Uart_Data_Choose(void);
void Uart_Clean_Block(E_BLOCK e_block);

void Uart_Data_Transfer(void);

void advertising_start(void);
void advertising_stop(void);
void adv_updata(void);
void advertising_init(void);
void Change_MacAddr(void);
void ADV_Update_Param(void);
void Ble_Disconnect(void);

void Set_Ble_Connect_Status(bool status);
bool Get_Ble_Connect_Status(void);
void scan_start(void);
void Scan_Stop(void);
#endif
