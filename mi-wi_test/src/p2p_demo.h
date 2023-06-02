/**
* \file  p2p_demo.h
*
* \brief Demo Application for MiWi P2P Interface
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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

#ifndef P2P_DEMO_H
#define	P2P_DEMO_H

extern unsigned char flag;
extern bool upState;
extern bool lastUpState;

extern volatile uint8_t feed_count, heartbeat_count, temp_heartbeat_count;

#define RX_BUFFER_SIZE 100		//org 109 //94//40

typedef struct  __attribute__((packed))
{
	uint8_t addr_type;
	uint8_t src_long_addr[8];
	uint8_t src_short_addr[2];
	uint8_t len;
	uint8_t data[RX_BUFFER_SIZE];
}subghz_rx_data_frame_t;


void subghz_rx_queue_push(subghz_rx_data_frame_t* push_packet);
bool subghz_rx_queue_pop(subghz_rx_data_frame_t* pop_packet);

uint32_t com_miwi_bytes_to_uint(uint8_t* bytes, uint8_t num_bytes, uint8_t endian);
void com_miwi_uint_to_bytes(uint32_t input, uint8_t* bytes, uint8_t num_bytes, uint8_t endian);

/*********************************************************************
* Function: void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
*
* Overview: Process a Received Message
*
* PreCondition: MiApp_ProtocolInit
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ReceivedDataIndication (RECEIVED_MESSAGE *ind);

void start_join_callback(miwi_status_t status);

#endif	/* P2P_DEMO_H */

