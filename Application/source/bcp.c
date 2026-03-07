#include "bcp.h"
#include <string.h>

uint8_t bcp_request_init(bcp_request_t *request) {
    if (request == NULL) {
        return 1;
    }
    memset(request, 0, sizeof(bcp_request_t));
    return 0;
}

uint8_t bcp_request_parse(bcp_request_t *request, const uint8_t *data, uint8_t length) {
    if (request == NULL || data == NULL) {
        return 1;
    }
    if (length < 5) {
        return 1;
    }
    if (data[0] != BLIP_SOF_BYTE) {
        return 1;
    }

    request->command = data[1];
    request->length  = data[2];

    if (request->length > 0) {
        memcpy(request->data, &data[3], request->length);
    }

    request->crc = (uint16_t) (data[3 + request->length]) | (uint16_t) (data[3 + request->length + 1] << 8);

    return 0;
}

uint8_t bcp_response_init(bcp_response_t *response) {
    if (response == NULL) {
        return 1;
    }
    memset(response, 0, sizeof(bcp_response_t));
    return 0;
}

uint8_t bcp_response_set_data(bcp_response_t *response, const uint8_t *data, uint8_t length) {
    return 0;
}

uint8_t bcp_response_to_bytes(const bcp_response_t *response, uint8_t *data) {
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  UART_HandleTypeDef *uart = NULL;

void bcp_uart_init(UART_HandleTypeDef *huart) {
    if (uart != NULL) {
        return;
    }

    uart = huart;
}

void bcp_send(const bcp_response_t *response) {
    uint16_t packet_length = 1 + BLIP_RESPONSE_HEADER_SIZE + response->length + 2;
    uint8_t packet[packet_length];

    packet[0] = BLIP_SOF_BYTE;
    packet[1] = response->command;
    packet[2] = response->status;
    packet[3] = response->length;
    memcpy(&packet[4], response->data, response->length);
    memcpy(&packet[4 + response->length], &response->crc, 2);

    HAL_UART_Transmit(uart, packet, packet_length, HAL_MAX_DELAY);
}

uint8_t bcp_receive(bcp_request_t *request) {
    uint8_t sof_byte = 0;
    do {
        HAL_UART_Receive(uart, &sof_byte, 1, HAL_MAX_DELAY);
    } while (sof_byte != BLIP_SOF_BYTE);

    HAL_UART_Receive(uart, &request->command, 1, HAL_MAX_DELAY);
    HAL_UART_Receive(uart, &request->length, 1, HAL_MAX_DELAY);
    HAL_UART_Receive(uart, request->data, request->length, HAL_MAX_DELAY);
    HAL_UART_Receive(uart, (uint8_t *) &request->crc, 2, HAL_MAX_DELAY);

    return 1;
}   
