#pragma once

#include <stdint.h>

#define BLIP_MAX_DATA_LENGTH        (255u)
#define BLIP_REQUEST_HEADER_SIZE    (2u)
#define BLIP_RESPONSE_HEADER_SIZE   (3u)

#define BLIP_SOF_BYTE (0xAAu)

typedef enum {
    BLIP_UNKNOWN_COMMAND  = 0x00,
    BLIP_UPLOAD_FIRMWARE  = 0x01,
    BLIP_UPDATE_FIRMWARE  = 0x02,
    BLIP_CALC_BANK_CRC    = 0x03,
    BLIP_RUN_FIRMWARE     = 0x04,
    BLIP_GET_VERSION      = 0x05,
} bcp_command_t;

typedef enum {
    BLIP_OK                    = 0x00,
    BLIP_ERROR_UNKNOWN_COMMAND = 0x01,
    BLIP_ERROR_INVALID_PARAM   = 0x02,
    BLIP_ERROR_BAD_CRC         = 0x03,
    BLIP_ERROR_BUSY            = 0x04,
    BLIP_ERROR_MEMORY          = 0x05,
} bcp_status_t;

typedef struct {
    uint8_t command;
    uint8_t length;
    uint8_t data[BLIP_MAX_DATA_LENGTH];
    uint16_t crc;
} bcp_request_t;

typedef struct {
    uint8_t command;
    uint8_t status;
    uint8_t length;
    uint8_t data[BLIP_MAX_DATA_LENGTH];
    uint16_t crc;
} bcp_response_t;

uint8_t bcp_request_init(bcp_request_t *request);

uint8_t bcp_request_parse(bcp_request_t *request, const uint8_t *data, uint8_t length);

uint8_t bcp_response_init(bcp_response_t *response);

uint8_t bcp_response_set_data(bcp_response_t *response, const uint8_t *data, uint8_t length);

uint8_t bcp_response_to_bytes(const bcp_response_t *response, uint8_t *data);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stm32f1xx_hal.h"

void bcp_uart_init(UART_HandleTypeDef *huart);

void bcp_send(const bcp_response_t *response);

uint8_t bcp_receive(bcp_request_t *request);