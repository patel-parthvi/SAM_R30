/**
* \file  p2p_demo.c
*
* \brief Demo Application for MiWi P2P Implementation
*
* Copyright (c) 2018 - 2019 Microchip Technology Inc. and its subsidiaries.
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products.
* It is your responsibility to comply with third party license terms applicable
* to your use of third party software (including open source software) that
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

/************************ HEADERS ****************************************/
#include "miwi_api.h"
#include "miwi_p2p_star.h"

#include "task.h"
#include "p2p_demo.h"
#include "mimem.h"
#include "asf.h"
#if defined(ENABLE_SLEEP_FEATURE)
#include "sleep_mgr.h"
#endif
#if defined (ENABLE_CONSOLE)
#include "sio2host.h"
#endif

#if defined(ENABLE_NETWORK_FREEZER)
#include "pdsMemIds.h"
#include "pdsDataServer.h"
#include "wlPdsTaskManager.h"
#endif

#include "adc_feature.h"

#if defined(PROTOCOL_STAR)
/************************ LOCAL VARIABLES ****************************************/
uint8_t i;
uint8_t TxSynCount = 0;
uint8_t TxSynCount2 = 0;
uint32_t TxNum = 0;
uint32_t RxNum = 0;
bool chk_sel_status = true;
uint8_t NumOfActiveScanResponse;
bool update_ed;
uint8_t select_ed;

volatile uint8_t feed_count;
// uint8_t msghandledemo = 0;
// MIWI_TICK t1 , t2;
/* Connection Table Memory */
extern CONNECTION_ENTRY connectionTable[CONNECTION_SIZE];

unsigned char flag;
bool upState;
bool lastUpState;

#define SUBGHZ_BUFF_SZ		(75)

typedef struct
{
	subghz_rx_data_frame_t subghz_rx_buff[SUBGHZ_BUFF_SZ];
	uint8_t wr_index;
	uint8_t rd_index;
}subghz_rx_packet_t;

volatile subghz_rx_packet_t rx_packet;

/************************ FUNCTION DEFINITIONS ****************************************/
/*********************************************************************
* Function: static void dataConfcb(uint8_t handle, miwi_status_t status)
*
* Overview: Confirmation Callback for MiApp_SendData
*
* Parameters:  handle - message handle, miwi_status_t status of data send
****************************************************************************/
void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer)
{		
    if (SUCCESS == status)
    {
        /* Update the TX NUM and Display it on the LCD */
		//++TxNum;
        //DemoOutput_UpdateTxRx(++TxNum, RxNum);
        /* Delay for Display */
        //delay_ms(100);
    }
    /* After Displaying TX and RX Counts , Switch back to showing Demo Instructions */
    //DemoOutput_Instruction ();
}

#define BIG_ENDIAN						1
#define LITTLE_ENDIAN					2

///////////////////////////////////////////////////////////////////////////////////////////////

uint32_t com_miwi_bytes_to_uint(uint8_t* bytes, uint8_t num_bytes, uint8_t endian)
{
	uint32_t ret_val = 0;
	
	//printf("bytes to int num_bytes, endian : %d %d \r\n", num_bytes, endian);

	if(endian == 1)
	{
		switch(num_bytes)
		{
			case 1:
				ret_val = (uint32_t)bytes[3];
				break;
				
			case 2:
				ret_val = (uint32_t)( bytes[1] | (uint32_t)bytes[0] << 8);
				break;
				
			case 3:
				ret_val = (uint32_t)( bytes[2] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[0] << 16);
				break;
				
			case 4:
				ret_val = (uint32_t)( bytes[3] | (uint32_t)bytes[2] << 8 | (uint32_t)bytes[1] << 16 | (uint32_t)bytes[0] << 24);
				break;
				
			default:
				break;
		}
	}	
	else if(endian == 2)
	{
		switch(num_bytes)
		{
			case 1:
				ret_val = (uint32_t)bytes[0];
				break;
				
			case 2:
				ret_val = (uint32_t)( bytes[0] | (uint32_t)bytes[1] << 8);
				break;
				
			case 3:
				ret_val = (uint32_t)( bytes[0] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[2] << 16);
				break;
				
			case 4:
				ret_val = (uint32_t)( bytes[0] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24);
				break;
				
			default:
				break;
		}
	}
		
	return ret_val;
}

void com_miwi_uint_to_bytes(uint32_t input, uint8_t* bytes, uint8_t num_bytes, uint8_t endian)
{
	uint8_t out_bytes[4] = { 0, 0, 0, 0};
		
	if(endian == 1)
	{
		switch(num_bytes)
		{
			case 1:
				out_bytes[3] = (uint8_t)(input);
				break;

			case 2:
				out_bytes[3] = 0;
				out_bytes[2] = 0;
				out_bytes[1] = (uint8_t)(input);
				out_bytes[0] = (uint8_t)(input >> 8);
				break;

			case 3:
				out_bytes[3] = 0;
				out_bytes[2] = (uint8_t)(input);
				out_bytes[1] = (uint8_t)(input >> 8);
				out_bytes[0] = (uint8_t)(input >> 16);
				break;

			case 4:
				out_bytes[3] = (uint8_t)(input);
				out_bytes[2] = (uint8_t)(input >> 8);
				out_bytes[1] = (uint8_t)(input >> 16);
				out_bytes[0] = (uint8_t)(input >> 24);
				break;
				
			default:
				break;
		}
	}
	else if(endian == 2)
	{
		switch(num_bytes)
		{
			case 1:
				out_bytes[0] = (uint8_t)(input);
				break;

			case 2:
				out_bytes[0] = (uint8_t)(input);
				out_bytes[1] = (uint8_t)(input >> 8);
				break;

			case 3:
				out_bytes[0] = (uint8_t)(input);
				out_bytes[1] = (uint8_t)(input >> 8);
				out_bytes[2] = (uint8_t)(input >> 16);
				break;

			case 4:
				out_bytes[0] = (uint8_t)(input);
				out_bytes[1] = (uint8_t)(input >> 8);
				out_bytes[2] = (uint8_t)(input >> 16);
				out_bytes[3] = (uint8_t)(input >> 24);
				break;
				
			default:
				break;
		}
	}
	
	memcpy(bytes, out_bytes, num_bytes);
}

void subghz_rx_queue_push(subghz_rx_data_frame_t* push_packet)
{
	memcpy(&rx_packet.subghz_rx_buff[rx_packet.wr_index++], push_packet, sizeof(subghz_rx_data_frame_t));
	// printf("Write index : %d \r\n",rx_packet.wr_index);
	if(rx_packet.wr_index == SUBGHZ_BUFF_SZ)
	{
		rx_packet.wr_index = 0;
	}
}

bool subghz_rx_queue_pop(subghz_rx_data_frame_t* pop_packet)
{
	if(rx_packet.rd_index != rx_packet.wr_index)
	{
		memcpy(pop_packet, &rx_packet.subghz_rx_buff[rx_packet.rd_index++], sizeof(subghz_rx_data_frame_t));
		if(rx_packet.rd_index == SUBGHZ_BUFF_SZ)
		{
			rx_packet.rd_index = 0;
		}
		return true;
	}
	return false;
}


/*********************************************************************
* Function: void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
*
* Overview: Process a Received Message
*
* PreCondition: MiApp_ProtocolInit
*
* Input:  RECEIVED_MESSAGE *ind - Indication structure
********************************************************************/
void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
{
	volatile subghz_rx_data_frame_t rx_packet;
	
	if( rxMessage.flags.bits.srcPrsnt )
    {
        if( rxMessage.flags.bits.altSrcAddr )
        {
			memcpy(rx_packet.src_short_addr, rxMessage.SourceAddress, SHORT_ADDR_LEN);
			rx_packet.addr_type = 2; /* Short address */
        }
        else
        {    
			memcpy(rx_packet.src_long_addr, rxMessage.SourceAddress, LONG_ADDR_LEN);
			rx_packet.addr_type = 1; /* Long address */
        }
		memcpy(rx_packet.data, rxMessage.Payload, rxMessage.PayloadSize);		
		rx_packet.len = rxMessage.PayloadSize;
		subghz_rx_queue_push(&rx_packet);
    }
	
// 	volatile subghz_rx_data_frame_t rx_packet;
// 	uint16_t pkt_offset = 0;
// 	
// 	typedef struct __attribute__((packed))
// 	{
// 		uint8_t target;						//1. iMXRT, 2. SAMR30, 3. nRF52,
// 		uint8_t offset[2];	//Flash offset
// 		uint8_t data[8];			//Program bytes
// 	}cmd_subGhz_program_t;
// 
// 	cmd_subGhz_program_t program_packet;
// 	
// 	uint8_t msghandledemo;
// 	uint16_t broadcastAddress = 0xFFFF;
// 	uint8_t bytes_data[2] = { 0x00, 0x00};
// 	unsigned char size = 0, crc = 0;
// 	
// 	memcpy(rx_packet.data, rxMessage.Payload, rxMessage.PayloadSize);
// 	rx_packet.len = rxMessage.PayloadSize;
// 	
// 	printf("Receiced MiWi Data\r\n");
// 	printf("MiWi Tester Receiced Data : ");
// 	for(unsigned char i=0; i<rx_packet.len ; i++)
// 	{
// 		printf("%X ", rx_packet.data[i]);	
// 	}
// 	printf("\r\n");
// 		
// 	
// 	memset(program_packet.offset, 0x00, 2);	
// 	memset(bytes_data, 0x00, 2);	
// 	memcpy((uint8_t*)program_packet.offset, &rx_packet.data[9], 2);	
// 	pkt_offset = com_miwi_bytes_to_uint(&program_packet.offset[0], 2);
// 	pkt_offset = pkt_offset + 1;
// 	printf("Packet Offset : %d\r\n", pkt_offset);
// 	com_miwi_uint_to_bytes(pkt_offset, bytes_data, 2);
// 		
// 	unsigned char ACK[] = {0x02, 0x08, 0x00, 0x11, 0xBA, 0x3F, 0x02, bytes_data[0], bytes_data[1], 0x00, 0x9F};
// 		
// 	crc = 0;	ACK[10] = 0;
// 	size = sizeof(ACK);
// 	for(i=1; i<=size-1; i++)
//         crc = ACK[i] ^ crc;
// 	ACK[10] = crc;
// 	
// 	printf("Offsets byte0	byte1	size	CRC  :  0x%X	0x%X	0x%X	0x%X\r\n", bytes_data[0],bytes_data[1], size, ACK[10]);
// 			
// 	if(MiApp_SendData(SHORT_ADDR_LEN, (uint8_t *)&broadcastAddress, 11, ACK, msghandledemo++, false, NULL))
// 	{
// // 		printf("Sent data over MiWi....\r\n");
// // 		delay_ms(100);
// 	}
// 	
// 	memset(rx_packet.data, '\0', 100);	
	
// #if defined(ENABLE_CONSOLE)
//     /* Print the received information via Console */
//     DemoOutput_HandleMessage();
// #endif

//     /* Update the TX AND RX Counts on the display */
//     DemoOutput_UpdateTxRx(TxNum, ++RxNum);

// #if !defined(ENABLE_SLEEP_FEATURE)
//     /* Toggle LED2 to indicate receiving a packet */
//     LED_Toggle(LED0);
// #endif
// 
//     /* Display the Instructions message */
//     DemoOutput_Instruction();
}
#endif

/*********************************************************************
* Function: static void Connection_Confirm(miwi_status_t status)
*
* Overview: callback function called upon MiAPP_StarConnection
*           or MiApp_EstablishConnection procedure completes
* Parameter: status of the completed operation
********************************************************************/

#define UART_FRM_NUM_SZ			 (2U)
#define DEVICE_TYPE_PAN_CORD			1
#define DEVICE_TYPE_CORD				2
#define DEVICE_TYPE_END_DEV				3

uint8_t device_type_set = 0;
uint8_t ack_send_enable = 0;
uint8_t device_type_request = 0;

void start_join_callback(miwi_status_t status)
{		
	// printf("MiWi Connection event............. : %d  \r\n",status);
		
    if ((SUCCESS == status) || (ALREADY_EXISTS == status))
    {
        if (role == PAN_COORD)
        {
			device_type_set = DEVICE_TYPE_PAN_CORD;
        }
        else if(role == END_DEVICE)
        {
			device_type_set = DEVICE_TYPE_CORD;
        }
		
		// printf("MiWI Tester Device Type Set : %d\r\n", device_type_set);
		
#if defined(ENABLE_CONSOLE)
        DumpConnection(0xFF);
#endif
    }
    else
    {
		printf("[T][ERROR] Formation or connecting network with given parameters failed\r\n");
        /* Upon EstablishConnection failure, initiate the startConnection to form a network		*/
	    /* MiApp_StartConnection(START_CONN_DIRECT, 10, (1L << myChannel), Connection_Confirm); */
    }
	
	if(ack_send_enable)
	{
		if(device_type_set == DEVICE_TYPE_PAN_CORD && device_type_request == DEVICE_TYPE_PAN_CORD)
		{
			printf("[T] Device type set to PAN CORDINATOR success\r\n");		
		}
		else if(device_type_set == DEVICE_TYPE_CORD && device_type_request == DEVICE_TYPE_CORD)
		{
			printf("[T] Device type set to CORDINATOR success\r\n");	
		}
		else
		{			
			printf("[T][ERROR] Set to requested device type FAIL\r\n");	
		}		
		
		ack_send_enable = 0;
		device_type_request = 0;
	}
}
