/******************************************************************************
    Copyright (c) 2016 Nytec. All rights reserved.
*******************************************************************************
The information contained herein is confidential property of Nytec. The use,
copying, transfer or disclosure of such information is prohibited except by
express written agreement with Nytec.
*/

/** @file

@brief ASSA Protocol Support

@details This file provides support for assembling and disassembling protocol
         messages exchanged with the Assa Abloy lock control unit.

******************************************************************************/

#ifndef _ASSA_PROTOCOL_H
#define _ASSA_PROTOCOL_H

//=============================== INCLUDES ====================================
#include <stdint.h>
#include <stdbool.h>

//#include "subghz_host_protocol.h"

//====================== CONSTANTS, TYPES, AND MACROS =========================

//typedef struct
//{
//    uint8_t* p_buffer_rx; ///< Pointer to a buffer.
//    uint16_t buffer_len;  ///< Buffer length of buffer in p_buffer_rx.
//    uint16_t cur_idx;     ///< Current unused.
//    uint16_t wr_idx;      ///< Current length of data received.
//    uint16_t rd_idx;      ///< Currently always 0.
//    uint8_t  state;       ///< State of buffer
//                          ///< (see below for valid values).
//    uint16_t msg_len;     ///< Length of message frame.
//    uint32_t num_msg_rx;  ///< Number of valid message frames found.
//} protocol_buffer_state_t;


// Max length of protocol message: including STX, LENGTH field and CRC field.
#define FRAME_MAX_LEN               260

// Start of text.
#define STX                         0x02

// Manufacturing testing mode (0x00 is invalid)
// 0x01 Lite Point testing (scannable and connectable advertising)
#define MFG_TEST_MODE_1             0x01

#define PLAIN_TEXT_CMD              0x00

// Addresses.
#define ADDRESS_LCU		         0x01
#define ADDRESS_LCX		         0x02
#define ADDRESS_GW		         0x03
#define ADDRESS_SUBGHZ	         0x0D
#define ADDRESS_RT    	         0x0E
#define ADDRESS_RFU		         0x0F

#define UART_FRM_STX_CMD         (0x02)

#define UART_FRM_NUM_SZ			 (2U)

#define UART_FRM_STX_POS         (0U)
#define UART_FRM_LEN_POS         (1U)
#define UART_FRM_CMD_TYPE_POS    (2U)
#define UART_FRM_FRM_TYPE_POS    (3U)
#define UART_FRM_ADDR_POS        (4U)
#define UART_FRM_CMD_POS         (5U)
#define UART_FRM_FRM_NUM_POS	 (6U)
#define UART_FRM_DATA_POS        (8U)

//=============================== FUNCTIONS ===================================

bool assa_check_integrity(const uint8_t* const p_msg);
uint16_t assa_gen_msg_text(uint8_t* const p_msg_frame, uint8_t command, uint8_t* const p_cmd_params, uint8_t params_len, uint8_t * const frm_num);
    
uint8_t assa_get_length_field(const uint8_t* const p_msg_start);
uint8_t assa_get_cmd_field(const uint8_t* const p_msg_start);
uint16_t assa_get_msg_len(uint8_t* const);
uint8_t assa_get_command_frame_length(uint8_t);
uint8_t assa_get_dst(uint8_t* const);
uint8_t assa_get_src(uint8_t* const);
const uint8_t* assa_get_command_frame(const uint8_t* const);

#endif // MSG_PROTOCOL_H
