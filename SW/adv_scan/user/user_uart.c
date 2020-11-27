#include "user_head.h"
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);  

uint8_t rx_inde = 0;
uint8_t user_tx_buf[USER_UART_BUF_SIZE] = {0};
uint8_t user_rx_buf[USER_UART_BUF_SIZE] = {0};


_uart_data uart_data;
char rx_cmd1[]  = "<DISCONNECT>";
char rx_cmd2[]  = "<CONNECT_STATE>";
char rx_cmd3[]  = "<STOP_SCAN>";
char rx_cmd4[]  = "<START_SCAN>";
char rx_cmd5[]  = "<SCAN_STATE>";
char rx_cmd6[]  = "<SET_NAME=>";         //
char rx_cmd7[]  = "<READ_NAME>";
char rx_cmd8[]  = "<SET_ADV_INT=>";
char rx_cmd9[]  = "<READ_ADV_INT>";
char rx_cmd10[] = "<SET_ADV_DELAY=>";
char rx_cmd11[] = "<READ_ADV_DELAY>";
char rx_cmd12[] = "<SET_ADV_TIMEOUT=>";
char rx_cmd13[] = "<READ_ADV_TIMEOUT>";
char rx_cmd14[] = "<STOP_ADV>";
char rx_cmd15[] = "<START_ADV>";
char rx_cmd16[] = "<ADV_STATE>";
char rx_cmd17[] = "<SET_BLE_UP_MODE=>";
char rx_cmd18[] = "<READ_BLE_UP_MODE>";
char rx_cmd19[] = "<SET_MODE2_ADV_TIMEOUT=>";
char rx_cmd20[] = "<READ_MODE2_ADV_TIMEOUT>";
char rx_cmd21[] = "<ST_TX_POWER=>";           //
char rx_cmd22[] = "<READ_TX_POWER>";
char rx_cmd23[] = "<RECOVER_FACTORY_SET>";
char rx_cmd24[] = "<SW_VERION>";
char rx_cmd25[] = "<CLEAN_BLOCK>";

char tx_cmd1[] = "<ok>";
char tx_cmd2[] = "<connect=1>";
char tx_cmd3[] = "<connect=0>";
char tx_cmd4[] = "<scan_enable=1>";
char tx_cmd5[] = "<scan_enable=0>";
char tx_cmd6[] = "<st_tx_power=ok>";
char tx_cmd7[] = "<adv_enable=1>";
char tx_cmd8[] = "<adv_enable=0>";
//char tx_cmd7[] = "<adv_int=32>";
//char tx_cmd8[] = "<adv_delay=8>";

void Uart_Data_Init(void)
{
	//U8 loop;
	uart_data.adv_time = 0;
	memset(uart_data.adv_sta, false, sizeof(uart_data.adv_sta));
	memset(uart_data.adv_timeout, false, sizeof(uart_data.adv_timeout));
	memset(uart_data.adv_timer, 0, sizeof(uart_data.adv_timer));
	memset(uart_data.ret_block, 0, sizeof(uart_data.ret_block));
	uart_data.cs = 0;
	uart_data.length = 0;

	//Uart_Clean_Block(BLOCK2);
	//for(loop = 0; loop < 4; loop++)
	//{
	//	uart_data.ret_block[2][loop] = 0xEE;
	//}
}

void UART_Send_String(char* buf, uint16_t len)
{
	uint8_t loop;

	for(loop = 0; loop < len; loop++)
	{
		app_uart_put(buf[loop]);
	}
	//app_uart_put('\r');
	//app_uart_put('\n');
	
}

void Uart_Cmd(char *tx_buf, char *rx_buf)
{
	//u8 length = 0;
	//length = strlen(rx_buf)-1;
	
	if((strcmp(rx_buf, rx_cmd1)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("disconnect");
		#endif
		Ble_Disconnect();
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd2)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("View connection status");
		#endif
		if(Get_Ble_Connect_Status() == true)
		{
			UART_Send_String(tx_cmd2, strlen(tx_cmd2));
		}
		else
		{
			UART_Send_String(tx_cmd3, strlen(tx_cmd3));
		}
	}
	else if((strcmp(rx_buf, rx_cmd3)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("stop san");
		#endif
		Scan_Stop();
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd4)) == 0)
	{
		#if (DEBUG_CMD) 
		NRF_LOG_INFO("Start san");
		#endif
		scan_start();
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd5)) == 0)
	{
		//Scan_Status_Get();
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read san status");
		#endif
		if(Scan_Status_Get() == true)
		{
			UART_Send_String(tx_cmd4, strlen(tx_cmd4));
		}
		else
		{
			UART_Send_String(tx_cmd5, strlen(tx_cmd5));
		}
	}
	else if((strncmp(rx_buf, rx_cmd6, 10)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set ble name");
		NRF_LOG_INFO("rx_buf is %s", rx_buf);
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));

		advertising_stop();
		ADV_Store_Name(rx_buf);
		ADV_Set_Name();
		
		advertising_init();
		advertising_start();
	}
	else if((strcmp(rx_buf, rx_cmd7)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read ble name");
		#endif
		tx_buf = ADV_Get_Name();
		UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strncmp(rx_buf, rx_cmd8, 13)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv interval");
		#endif
		advertising_stop();
		ADV_Set_Interval(rx_buf);
		advertising_init();
		advertising_start();
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd9)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv interval");
		#endif
		tx_buf = ADV_Get_Interval();
		UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strncmp(rx_buf, rx_cmd10, 15)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv delay");
		#endif
		ADV_Set_Delay(rx_buf);
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd11)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv delay");
		#endif
		tx_buf = ADV_Get_Delay();
		UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strncmp(rx_buf, rx_cmd12, 17)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv timeout");
		#endif
		advertising_stop();
		ADV_Set_Timeout(rx_buf);
		advertising_init();
		advertising_start();
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd13)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv timeout");
		#endif
		tx_buf = ADV_Get_Timeout();
		UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strcmp(rx_buf, rx_cmd14)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Stop Adv");
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		nrf_delay_ms(10);
		advertising_stop();
		
	}
	else if((strcmp(rx_buf, rx_cmd15)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Start Adv");
		#endif
		//Beacon_Adv_Start();
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		nrf_delay_ms(10);
		advertising_start();
	}
	else if((strcmp(rx_buf, rx_cmd16)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv state");
		#endif
		
		if(ADV_Status_Get() == true)
		{
			UART_Send_String(tx_cmd7, strlen(tx_cmd7));
		}
		else
		{
			UART_Send_String(tx_cmd8, strlen(tx_cmd8));
		}
	}
	else if((strcmp(rx_buf, rx_cmd17)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv up mode");
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd18)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv up mode");
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd19)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set mode2 Adv timeout");
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		
	}
	else if((strcmp(rx_buf, rx_cmd20)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read mode2 Adv timeout");
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strncmp(rx_buf, rx_cmd21, 13)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set TX power");
		#endif
		
		advertising_stop();
		ADV_Set_TxPwr(rx_buf);
		advertising_init();
		advertising_start();
		
		UART_Send_String(tx_cmd6, strlen(tx_cmd6));
	}
	else if((strcmp(rx_buf, rx_cmd22)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read TX power");
		#endif
		
		tx_buf = ADV_Get_TxPwr();
		UART_Send_String(tx_buf, strlen(tx_buf));
		
	}
	else if((strcmp(rx_buf, rx_cmd23)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Recover factory");
		#endif
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		nrf_delay_ms(500);
		NVIC_SystemReset();
	}
	else if((strcmp(rx_buf, rx_cmd24)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Get version");
		#endif
		UART_Send_String(JS_VERSION, strlen(JS_VERSION));
	}
	else if((strncmp(rx_buf, rx_cmd25, 12)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("clean block");
		#endif
		 Uart_Clean_Block(BLOCK0);
		
		UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("cmd is error");
		#endif
	}
	memset(rx_buf, 0, 31);
	memset(tx_buf, 0, 31);
	//rx_inde = 0;
	
}

void Uart_Data(uint8_t *tx_buf, uint8_t *rx_buf)
{
	U8 loop1 = 0;
	U8 loop2 = 0;
	U8 loop3 = 0;
//	U8 loop4 = 0;
	U8 len = rx_buf[2] + 2;

	uart_data.length = rx_buf[2]/4;
	NRF_LOG_INFO("The uart_data.length is %d", uart_data.length);

//	while((uart_data.adv_sta[loop4] == false) && (loop4 < 8))
//	{
//		uart_data.adv_sta[loop4] = true;
//		loop4++;
//	}
	while(loop1 < 8)
	{
		if((uart_data.ret_block[loop1][0] == 0x00)
		  &&(uart_data.ret_block[loop1][1] == 0x00)
		  &&(uart_data.ret_block[loop1][2] == 0x00)
		  &&(uart_data.ret_block[loop1][3] == 0x00))
		{
			//uart_data.adv_sta[loop1] = true;

			for(loop2 = 0; loop2 < 4; loop2++)
			{
				uart_data.ret_block[loop1][loop2] = rx_buf[3 + loop3];
				//NRF_LOG_INFO("uart_data.ret_block[%d][%d] is  0x%x", loop1, loop2, uart_data.ret_block[loop1][loop2]);
				loop3++;
			}
			if(loop3 == len)
			{
				break;
			}
		}
		
		loop1++;
		//NRF_LOG_INFO("The value of loop1 is %d", loop1);
	}
//	NRF_LOG_INFO("uart_data.ret_block[1][2] is  0x%x", uart_data.ret_block[1][2]);

	memset(rx_buf, 0, 20);
	memset(tx_buf, 0, 20);
	
	
}



/**************************************************
*@Author:PeterPan
*
*@Date:  2020.10.29
*
*@Breaf: Clearly specify the data block e_block,Please Look at the enumeration variable E_BLOCK.
**************************************************/
void Uart_Clean_Block(E_BLOCK e_block)
{
	U8 loop;
	
	for(loop = 0; loop < 4; loop++)
	{
		uart_data.ret_block[e_block][loop] = 0;
	}
}


void Uart_Data_Transfer(void)
{
	U8 loop1 = 0;
	U8 loop2 = 0;
	U8 loop3 = 0;
	U8 data[18];
	for(loop1 = 0; loop1 < 4; loop1++)
	{
		for(loop2 = 0; loop2 < 4; loop2++)
		{
			data[loop3] = uart_data.ret_block[loop1][loop2];
			//NRF_LOG_INFO("dat.data1[%d] is  0x%x", loop3, dat.data1[loop3]);
			loop3++;
		}
	}
	
	ADV_Data_Set(data, ADV_DATA1);
	
	for(loop1 = 4; loop1 < 8; loop1++)
	{
		for(loop2 = 0; loop2 < 4; loop2++)
		{
			data[loop3] = uart_data.ret_block[loop1][loop2];
			loop3++;
			//NRF_LOG_INFO("uart_data.ret_block[%d][%d] is  0x%x", loop1, loop2, uart_data.ret_block[loop1][loop2]);
		}
	}
	ADV_Data_Set(data, ADV_DATA2);
	
}
