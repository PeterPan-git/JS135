#include "user_head.h"

uint8_t rx_inde = 0;
uint8_t user_tx_buf[USER_UART_BUF_SIZE] = {0};
uint8_t user_rx_buf[USER_UART_BUF_SIZE] = {0};
static U8 app_data[20];
static bool group_sta[8];

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

/****************************************************************
*-------------MCU <----> BLE----------
*-----------Frame Header: '<'--------
*----------------START----------------
*************************************/
void CMNC_Data_Init(void)
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



void Uart_Cmd(char *tx_buf, char *rx_buf)
{
	U8 adv_data[20];
	Param_ADV_Data_Get(adv_data, ADV_DATA1);
	
	if((strcmp(rx_buf, rx_cmd1)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("disconnect");
		#endif
		BLE_Disconnect();
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd2)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("View connection status");
		#endif
		if(Param_Get_Ble_Connect_Status() == true)
		{
			Drive_UART_Send_String(tx_cmd2, strlen(tx_cmd2));
		}
		else
		{
			Drive_UART_Send_String(tx_cmd3, strlen(tx_cmd3));
		}
	}
	else if((strcmp(rx_buf, rx_cmd3)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("stop san");
		#endif
		BLE_Scan_Stop();
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd4)) == 0)
	{
		#if (DEBUG_CMD) 
		NRF_LOG_INFO("Start san");
		#endif
		BLE_Scan_Start();
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd5)) == 0)
	{
		//Scan_Status_Get();
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read san status");
		#endif
		if(Param_Scan_Status_Get() == true)
		{
			Drive_UART_Send_String(tx_cmd4, strlen(tx_cmd4));
		}
		else
		{
			Drive_UART_Send_String(tx_cmd5, strlen(tx_cmd5));
		}
	}
	else if((strncmp(rx_buf, rx_cmd6, 10)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set ble name");
		NRF_LOG_INFO("rx_buf is %s", rx_buf);
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));

		BLE_ADV_Stop();
		Param_ADV_Store_Name(rx_buf);
		Param_ADV_Set_Name();
		
		BLE_ADV_Init(HEAD_ID_2, adv_data);
		BLE_ADV_Start();
	}
	else if((strcmp(rx_buf, rx_cmd7)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read ble name");
		#endif
		tx_buf = Param_ADV_Get_Name();
		Drive_UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strncmp(rx_buf, rx_cmd8, 13)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv interval");
		#endif
		BLE_ADV_Stop();
		Param_ADV_Set_Interval(rx_buf);
		BLE_ADV_Init(HEAD_ID_2, adv_data);
		BLE_ADV_Start();
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd9)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv interval");
		#endif
		tx_buf = Param_ADV_Get_Interval();
		Drive_UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strncmp(rx_buf, rx_cmd10, 15)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv delay");
		#endif
		Param_ADV_Set_Delay(rx_buf);
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd11)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv delay");
		#endif
		tx_buf = Param_ADV_Get_Delay();
		Drive_UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strncmp(rx_buf, rx_cmd12, 17)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv timeout");
		#endif
		BLE_ADV_Stop();
		Param_ADV_Set_Timeout(rx_buf);
		BLE_ADV_Init(HEAD_ID_2, adv_data);
		BLE_ADV_Start();
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd13)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv timeout");
		#endif
		tx_buf = Param_ADV_Get_Timeout();
		Drive_UART_Send_String(tx_buf, strlen(tx_buf));
	}
	else if((strcmp(rx_buf, rx_cmd14)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Stop Adv");
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		nrf_delay_ms(10);
		BLE_ADV_Stop();
		
	}
	else if((strcmp(rx_buf, rx_cmd15)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Start Adv");
		#endif
		//Beacon_Adv_Start();
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		nrf_delay_ms(10);
		BLE_ADV_Start();
	}
	else if((strcmp(rx_buf, rx_cmd16)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv state");
		#endif
		
		if(Param_ADV_Status_Get() == true)
		{
			Drive_UART_Send_String(tx_cmd7, strlen(tx_cmd7));
		}
		else
		{
			Drive_UART_Send_String(tx_cmd8, strlen(tx_cmd8));
		}
	}
	else if((strcmp(rx_buf, rx_cmd17)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set Adv up mode");
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd18)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read Adv up mode");
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strcmp(rx_buf, rx_cmd19)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set mode2 Adv timeout");
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		
	}
	else if((strcmp(rx_buf, rx_cmd20)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read mode2 Adv timeout");
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
	}
	else if((strncmp(rx_buf, rx_cmd21, 13)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Set TX power");
		#endif
		
		BLE_ADV_Stop();
		Param_ADV_Set_TxPwr(rx_buf);
		BLE_ADV_Init(HEAD_ID_2, adv_data);
		BLE_ADV_Start();
		
		Drive_UART_Send_String(tx_cmd6, strlen(tx_cmd6));
	}
	else if((strcmp(rx_buf, rx_cmd22)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Read TX power");
		#endif
		
		tx_buf = Param_ADV_Get_TxPwr();
		Drive_UART_Send_String(tx_buf, strlen(tx_buf));
		
	}
	else if((strcmp(rx_buf, rx_cmd23)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Recover factory");
		#endif
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
		nrf_delay_ms(500);
		NVIC_SystemReset();
	}
	else if((strcmp(rx_buf, rx_cmd24)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("Get version");
		#endif
		Drive_UART_Send_String(JS_VERSION, strlen(JS_VERSION));
	}
	else if((strncmp(rx_buf, rx_cmd25, 12)) == 0)
	{
		#if (DEBUG_CMD)
		NRF_LOG_INFO("clean block");
		#endif
		 Uart_Clean_Block(BLOCK0);
		
		Drive_UART_Send_String(tx_cmd1, strlen(tx_cmd1));
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
	
	Param_ADV_Data_Set(data, ADV_DATA1);
	
	for(loop1 = 4; loop1 < 8; loop1++)
	{
		for(loop2 = 0; loop2 < 4; loop2++)
		{
			data[loop3] = uart_data.ret_block[loop1][loop2];
			loop3++;
			//NRF_LOG_INFO("uart_data.ret_block[%d][%d] is  0x%x", loop1, loop2, uart_data.ret_block[loop1][loop2]);
		}
	}
	Param_ADV_Data_Set(data, ADV_DATA2);
	
}
/*************************************
*-----------------END-----------------
*-------------MCU <----> BLE----------
****************************************************************/

/****************************************************************
*-------------MCU <----> BLE----------
*-----------Frame Header: 0xEF--------
*----------------START----------------
*************************************/

/*************************************
*-----------------END-----------------
*-------------MCU <----> BLE----------
****************************************************************/

/****************************************************************
*-------------MCU ----> APP-----------
*-----------Frame Header: 0xED--------
*----------------START----------------
*************************************/
void CMNC_MCU_APP_Date_Get(U8 *rx_buf)
{
	U8 data[18];

	memcpy(data, &rx_buf[2], sizeof(data));
	BLE_ADV_Stop();
	BLE_ADV_Updata(HEAD_ID_1, data);
	BLE_ADV_Start();
	
}
/*************************************
*-----------------END-----------------
*-------------MCU <----> APP----------
****************************************************************/


/****************************************************************
*-------------APP ----> MCU-----------
*-----------Frame Header: 0xEE--------
*----------------START----------------
*************************************/
void CMNC_APP_MCU_Data_Init(void)
{
	memset(app_data, 0x00, sizeof(app_data));
}
void CMNC_APP_MCU_Data_Set(U8 *data)
{
	#if (DEBUG_BLE_APP)
	U8 loop;
	#endif
	
	memcpy(app_data, data, sizeof(app_data));
	
	#if (DEBUG_BLE_APP)
	NRF_LOG_RAW_INFO("App_data:0x");
	for(loop = 0; loop < 20; loop++)
	{
		
		NRF_LOG_RAW_INFO(" %x", app_data[loop]);
		
	}
	NRF_LOG_RAW_INFO("\n");
	#endif
}
U8* CMNC_APP_MCU_Data_Get(void)
{
	return app_data;
}

U32 CMNC_APP_MCU_Data_Receice(const ble_gap_evt_adv_report_t *p_adv_report)
{
	uint32_t  index = 0;
	uint8_t * p_data;

	p_data = p_adv_report->data.p_data;
   //检索广播数据
   while (index < p_adv_report->data.len)
   {
      uint8_t field_length = p_data[index];
      uint8_t field_type   = p_data[index + 1];
      //如果ADV TYPE是完整的设备名称或者裁剪的设备名称，打印出设备名称
  	 switch(field_type)
	  {
	  	case BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE:

			break;
			
		case BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME:
			//NRF_LOG_RAW_INFO("\n name:");
		  	#if 0
		  	for(i=0;i<field_length-1;i++)
		  	NRF_LOG_RAW_INFO("%c", p_data[index+2+i]);
			#endif
			break;
		
		case BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
			
		//NRF_LOG_RAW_INFO("p_data[5]-[6]:0x %x %x", p_data[5], p_data[6]);
	//	NRF_LOG_RAW_INFO("\n");
			if((p_data[8] == 0x13) && (p_data[9] == 0xEF))      //13    EF
			{
				CMNC_APP_MCU_Data_Set(&p_data[8]);
				CMCN_APP_MCU_Data_Send();
				
				NRF_LOG_RAW_INFO("data:0x");
				for(U8 i=0;i<field_length-1;i++)
		  		{
					NRF_LOG_RAW_INFO("%x", app_data[i]);

				}
				NRF_LOG_RAW_INFO("\n");
			}
			
			break;
	  }
      index += field_length + 1;
    }
	 return NRF_ERROR_NOT_FOUND;
}

void CMCN_APP_MCU_Data_Send(void)
{
	Drive_UART_Send_String((char*)app_data, sizeof(app_data));
}
/*************************************
*-----------------END-----------------
*-------------MCU <----> APP----------
****************************************************************/

void Uart_Data_Choose(void)
{
	uint8_t head_status[2]; 

	head_status[0] = user_rx_buf[0];
	head_status[1] = user_rx_buf[1];
	
	if((rx_status == false) && (rx_inde > 1))
	{
		if(head_status[0] == HD_1)
		{
			NRF_LOG_INFO("HD_1");
			if(head_status[1] == FD_1)
			{
				NRF_LOG_INFO("FD_1");
				CMCN_Save(user_rx_buf);
				//Uart_Data_Transfer();
				CMCN_Do();
				rx_inde = 0;
			}
		}
		else if(head_status[0] == HD_2)
		{
			NRF_LOG_INFO("HD_2");
			Uart_Cmd((char*)user_tx_buf, (char*)user_rx_buf);
			rx_inde = 0;
		}
		else
		{
			NRF_LOG_INFO("HD error");
			rx_inde = 0;
		}
		
	}
	
}

void CMCN_Save(U8 *rx)
{
	U8 len = 0;
	U8 loop1, loop2, loop3;
	U8 space_data = 0;
	U8 number_data = 0;
	
	uart_data.length = rx[2];
	number_data = (rx[2]) / 4;
	space_data = CMCN_Check();
	uart_data.cs = CMNC_CRC_Data(&rx[2]);
	NRF_LOG_INFO("crc_value and rx[19] is 0x%x 0x%x", uart_data.cs, rx[19]);
	if(uart_data.cs != rx[19])
	{
		NRF_LOG_INFO("uart crc fail!");
		return;
	}
	if(space_data == 0)                   //剩余空间为0就退出
	{
		NRF_LOG_INFO("There is no extra data space !");
		return;
	}
	switch(number_data)
	{
		case DATA_GROUP_1:
			len = 4;
			break;
		case DATA_GROUP_2:
			len = 8;
			break;
		case DATA_GROUP_3:
			len = 12;
			break;
		case DATA_GROUP_4:
			len = 16;
			break;
		default:
			break;
	}
	
	for(loop1 = 0, loop3 = 0; loop1 < 8; loop1++)
	{
		if(group_sta[loop1] == true)
		{
			for(loop2 = 0; loop2 < 4; loop2++)
			{
				uart_data.ret_block[loop1][loop2] = rx[3 + loop3];
				loop3++;
				NRF_LOG_INFO("uart_data.ret_block[x][x] is 0x%x", uart_data.ret_block[loop1][loop2]);
			}
			if(loop3 == len)            //如果待存数据长度小于剩余存储空间，则提前退出程序
			{
				return;
			}
		}
	}
}

void CMCN_Get(void)
{
	U8 loop1 = 0;
	U8 loop2 = 0;
	U8 loop3 = 0;
	U8 data[18];
	data[0] = uart_data.length;
	data[17] = uart_data.cs;
	for(loop1 = 0; loop1 < 4; loop1++)
	{
		for(loop2 = 0; loop2 < 4; loop2++)
		{
			data[loop3+1] = uart_data.ret_block[loop1][loop2];
			//NRF_LOG_INFO("dat.data1[%d] is  0x%x", loop3, dat.data1[loop3]);
			loop3++;
		}
	}
	
	Param_ADV_Data_Set(data, ADV_DATA1);
	
	for(loop1 = 4; loop1 < 8; loop1++)
	{
		for(loop2 = 0; loop2 < 4; loop2++)
		{
			data[loop3] = uart_data.ret_block[loop1][loop2];
			loop3++;
			//NRF_LOG_INFO("uart_data.ret_block[%d][%d] is  0x%x", loop1, loop2, uart_data.ret_block[loop1][loop2]);
		}
	}
	Param_ADV_Data_Set(data, ADV_DATA2);
	
}

void CMCN_Do(void)
{
	U8 adv_data[18];
	CMCN_Get();
	Param_ADV_Data_Get(adv_data, ADV_DATA1);
	BLE_ADV_Stop();
	BLE_ADV_Updata(HEAD_ID_2, adv_data);
	//BLE_ADV_Init(HEAD_ID_2, adv_data);
	BLE_ADV_Start();
}
void CMCN_Deal(E_BLOCK e_block)
{
	U8 loop;
	
	for(loop = 0; loop < 4; loop++)
	{
		uart_data.ret_block[e_block][loop] = 0;
	}
}
U8 CMCN_Check(void)
{
	U8 loop1, loop2, loop3,sum;
	sum = 0;
	memset(group_sta, false, 8);
	for(loop1 = 0; loop1 < 8; loop1++)
	{
		for(loop2 = 0, loop3 = 0; loop2 < 4; loop2++)
		{
			if(uart_data.ret_block[loop1][loop2] == 0) 
			{
				loop3++;
			}
			if(loop3 == 4)
			{
				group_sta[loop1] = true;  //真代表该数据行是空的
				sum++;
			}
		}
	}
	
	return sum;
	
}
U8 CMNC_CRC_Data(U8* rx)
{
	U8 loop;
	U8 sum = 0;
	
	for(loop = 0; loop < 17; loop++)
	{
		sum += rx[loop];
	}
	
	return sum;
}
