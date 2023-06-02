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


//============================= CONDITIONALS ==================================


//=============================== INCLUDES ====================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assa_protocol.h"

//====================== CONSTANTS, TYPES, AND MACROS =========================

//// Field values.
#define VERSION_ID              0x01
#define COMMAND_FRAME           0x01

#define MSG_FRAME_HDR_LEN       0x02 // STX + LENGTH FIELD
#define CMD_FRAME_HDR_LEN       0x03 // cmdtype, frameType, src&dst,

#define LEN_STX_LEN_CRC         0x03

// Field offsets.
#define START_OFFSET            UART_FRM_STX_POS
#define LENGTH_OFFSET           UART_FRM_LEN_POS
#define SESSION_CMD_OFFSET      UART_FRM_CMD_TYPE_POS
#define APP_VERSION_OFFSET      3
#define APP_FRAMETYPE_OFFSET    UART_FRM_FRM_TYPE_POS
#define APP_SRC_OFFSET          UART_FRM_ADDR_POS
#define APP_DST_OFFSET          UART_FRM_ADDR_POS
#define CMD_FRAME_CMD_OFFSET    UART_FRM_CMD_POS
#define CMD_FRAME_PARAM_OFFSET  6

//====================== STATIC FUNCTION DECLARATIONS =========================
static uint8_t compute_crc(const uint8_t* p_src, uint16_t len);
static uint8_t gen_cmd_frame(uint8_t*, uint8_t, uint8_t*, uint8_t, uint8_t * const frm_num);
static uint16_t populate_header_crc(uint8_t*, uint8_t);


//=============================== FUNCTIONS ===================================

/** Compute CRC
 *
 * @param[in] p_src Pointer to source data.
 * @param[in] len   Length of source data.
 *
 * @return The CRC value.
 */
uint8_t compute_crc(const uint8_t* p_src, uint16_t len)
{
  //  ASSERT(p_src);
  //  ASSERT(len);

    uint8_t crc_val = 0;
    for(uint16_t ii = 0; ii < len; ii++)
    {
        crc_val ^= *p_src++;
    }

    return(crc_val);
}


// generates the command frame header and payload
/** Generate Command Frame and Payload
 *
 * @param[out] p_dst Pointer to destination where command frame is written.
 * @param[in]  cmd  Command to place in frame.
 * @param[in]  p_data Pointer to command data (may be NULL if no data).
 * @param[in]  data_len Length of command data (may be zero if no data).
 *
 * @return Length of resulting command frame.
 */
uint8_t gen_cmd_frame(uint8_t* const p_dst, uint8_t cmd, uint8_t* const p_data, uint8_t data_len, uint8_t * const frm_num)
{
	// ASSERT(p_dst);

    uint8_t cmd_frame_len = 1;

    // Place the command.
    *p_dst = cmd;

	/* Add frame number after command */
    memcpy(p_dst+1, frm_num, UART_FRM_NUM_SZ);
	cmd_frame_len += UART_FRM_NUM_SZ;
    if(p_data != NULL)
    {
        memcpy(p_dst+3, p_data, data_len);
        cmd_frame_len += data_len;
    }

    return(cmd_frame_len);
}



/** Populate the Header CRC
 *
 * @param[in] p_msg_frame Pointer to message frame, including STX.
 * @param[in] payload_len Length of payload.
 *
 * @return Length of entire frame.
 */
uint16_t populate_header_crc(uint8_t* const p_msg_frame, uint8_t payload_len)
{
//    ASSERT(p_msg_frame);

    // Place STX and length.
    p_msg_frame[START_OFFSET] = STX;
    p_msg_frame[LENGTH_OFFSET] = payload_len;

    // Compute CRC including length field.
    uint8_t crc_val = compute_crc(&p_msg_frame[LENGTH_OFFSET], payload_len+1);

    // Offset is payload length + STX field + LEN field
    p_msg_frame[payload_len + MSG_FRAME_HDR_LEN] = crc_val;

    // Add STX, len and CRC
    uint16_t frame_len = payload_len + LEN_STX_LEN_CRC;

    return(frame_len);
}



/** Generate Text Message
 *
 *
 * @param[out] p_msg_frame Pointer to location where frame is written.
 *             @note It's the caller responsibility to ensure that the buffer
 *             is large enough to hold the entire message frame.
 * @param[in] command Command.
 * @param[in] p_cmd_params Pointer to command parameters (may be NULL).
 * @param[in] params_len Length of parameters (may be zero).
 *
 * @return Length of generated message.
 */
uint16_t assa_gen_msg_text(uint8_t* const p_msg_frame, uint8_t command, uint8_t* const p_cmd_params, uint8_t params_len, uint8_t * const frm_num)
{
 //   ASSERT(p_msg_frame);

    uint16_t frame_len = 0;

    // If have non-empty frame.
    uint8_t cmd_frame_len = gen_cmd_frame(&p_msg_frame[CMD_FRAME_CMD_OFFSET], command, p_cmd_params, params_len, frm_num);
    if(cmd_frame_len > 0)
    {
        // Complete the application layer frame.
        p_msg_frame[SESSION_CMD_OFFSET] = PLAIN_TEXT_CMD;
        p_msg_frame[APP_FRAMETYPE_OFFSET] = (VERSION_ID<<4) | COMMAND_FRAME;
        p_msg_frame[APP_SRC_OFFSET] = (ADDRESS_SUBGHZ << 4) | (ADDRESS_RT  & 0xf);
		/* memcpy(&p_msg_frame[UART_FRM_FRM_NUM_POS],frm_num,UART_FRM_NUM_SZ); */
        uint8_t app_frame_len = cmd_frame_len + CMD_FRAME_HDR_LEN;
        frame_len = populate_header_crc(p_msg_frame, app_frame_len);
    }

    // return total length of frame
    return(frame_len);
}

// p_msg - pointer to entire msg frame including STX
/** Check Integrity of Message
 *
 * @param[in] p_msg Pointer to message.
 *
 * @return True of CRC is correct.
 */
bool assa_check_integrity(const uint8_t* const p_msg)
{
  //  ASSERT(p_msg);

    // Obtain the CRC from the received message.
    uint8_t len = p_msg[LENGTH_OFFSET];
    uint8_t rx_crc = *(p_msg + MSG_FRAME_HDR_LEN + len);

    // CRC includes length field.
    uint8_t crc_computed = compute_crc(&p_msg[LENGTH_OFFSET], len+1);

	//printf("[T] RX CHK = %d, CAL CHK = %d \n",rx_crc, crc_computed);
    // Return true if the received CRC matches the computed CRC.
    return (rx_crc == crc_computed);
}


/** Get Command Frame
 *
 * @param p_msg Pointer to message.
 *
 * @return Command frame value.
 */
const uint8_t* assa_get_command_frame(const uint8_t* const p_msg)
{
 //   ASSERT(p_msg);
    return(&p_msg[CMD_FRAME_CMD_OFFSET]);
}

/** Get Command Frame Length
 *
 * Subtracts the command frame header length from the overall payload length.
 *
 * @param payload_len Payload length.
 *
 * @return Command frame length.
 */
uint8_t assa_get_command_frame_length(uint8_t payload_len)
{
   // ASSERT(CMD_FRAME_HDR_LEN <= payload_len);
    return(payload_len - CMD_FRAME_HDR_LEN);
}

/** Get Length of Message
 *
 * The length of the message includes the STX, LEN, and CRC field.
 *
 * @param p_msg_start Pointer to start of message (where STX starts).
 *
 * @return The expected message length including STX, LEN and CRC field.
 */
uint16_t assa_get_msg_len(uint8_t* const p_msg_start)
{
  //  ASSERT(p_msg_start);
    return(p_msg_start[LENGTH_OFFSET] + LEN_STX_LEN_CRC);
}

uint8_t assa_get_length_field(const uint8_t* const p_msg_start)
{
   // ASSERT(p_msg_start);
    return(p_msg_start[LENGTH_OFFSET]);
}

uint8_t assa_get_cmd_field(const uint8_t* const p_msg_start)
{
  //  ASSERT(p_msg_start);
    return(p_msg_start[SESSION_CMD_OFFSET]);
}



/** Get the Src Value
 *
 * @param p_msg_start Pointer to start of message (where STX starts).
 *
 * @return The Src value.
 */
uint8_t assa_get_src(uint8_t* const p_msg_start)
{
  //  ASSERT(p_msg_start);
    return(p_msg_start[APP_SRC_OFFSET] >> 4);
}



/** Get the Dst Value
 *
 * @param p_msg_start Pointer to start of message (where STX starts).
 *
 * @return The Dst value.
 */
uint8_t assa_get_dst(uint8_t* const p_msg_start)
{
  //  ASSERT(p_msg_start);
    return(p_msg_start[APP_DST_OFFSET] & 0xf);
}
