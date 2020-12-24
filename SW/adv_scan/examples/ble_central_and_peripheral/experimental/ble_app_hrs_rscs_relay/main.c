#include "user_head.h"

int main(void)
{
	U8 data[20];
	Drive_Log_Init();
	Drive_WDT_Init();
	Drive_GPIO_Connect_State_Init();
	CMNC_Data_Init();                          //广播数据块初始化
	Param_ADV_Data_Init();                     //广播buffer初始化      
	//CMCN_Get();                                //数据块转到buffer
	Param_ADV_Data_Get(data, ADV_DATA1);       //数据从buffer取出到data, 准备广播
	
	Drive_Timer_Init();
	Drive_Timer0_to_3_Init();
	
	Drive_UART_Init();
	CMNC_APP_MCU_Data_Init();
	Drive_Power_Manage_Init();
	
	BLE_Stack_Init();
	
	Param_ADV_Param_Init();
	BLE_Scan_Init();
	BLE_GAP_Params_Init();
	BLE_GATT_Init();
	BLE_GAP_Conn_Params_Init();
	BLE_Serve_Init();
	BLE_ADV_Init(data);

	BLE_ADV_Start();
	BLE_Scan_Start();
	Param_Get_MacAddr();
	NRF_LOG_INFO("start main while");
	while(1)
    {
		
		Drive_Feed_Dog();
		Uart_Data_Choose();
		//Param_ADV_Update_Data();
        Drive_Idle_State_Handle();
    }

}
