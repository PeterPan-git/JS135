#ifndef _USER_CMNC_H_
#define _USER_CMNC_H_
#include "user_head.h"

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256  

#define HD_1                            0x13
#define HD_2                            '<'  

#define FD_1                            0xEF

#define USER_UART_BUF_SIZE           50

#define JS_VERSION                      "V1.0"
#define BLE_CONNECT      true
#define BLE_DISCONNECT   false

extern uint8_t rx_inde;
extern uint8_t user_tx_buf[USER_UART_BUF_SIZE];
extern uint8_t user_rx_buf[USER_UART_BUF_SIZE];
extern U8 name_hex[3];
typedef struct
{
	bool adv_sta[8];                   //每组数据广播状态
	bool adv_timeout[8];               //每组数字广播超时
	uint32_t adv_timer[8];              //每组数据的广播计时
	//uint32_t adv_delay;                  //广播时间
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
	DATA_GAOUP_0 = 0,
	DATA_GROUP_1,
	DATA_GROUP_2,
	DATA_GROUP_3,
	DATA_GROUP_4,
	
}e_group;
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

void CMNC_Data_Init(void);
void UART_Cmd(char *tx_buf, char *rx_buf);
void Uart_Data_Choose(void);
void Uart_Clean_Block(E_BLOCK e_block);

void Uart_Data_Transfer(void);


void CMNC_APP_MCU_Data_Init(void);
void CMNC_APP_MCU_Data_Set(U8 *data);
U8* CMNC_APP_MCU_Data_Get(void);
U32 CMNC_APP_MCU_Data_Receice(const ble_gap_evt_adv_report_t *p_adv_report);
void CMCN_APP_MCU_Data_Send(U8 len);
//void CMNC_MCU_APP_Date_Get(U8 *rx_buf);
void CMCN_Save(U8 *rx);
void CMCN_Get(void);
void CMCN_Deal(E_BLOCK e_block);
U8 CMCN_Check(void);
U8 CMNC_CRC_Data(U8* rx, U8 num);
U8 CMNC_Get_CRC_Digit(U8* rx);
void CMNC_String_To_Hex(char* str, unsigned char* hex);
bool CMNC_Repeat_Filt(U8 *rx);
#endif
