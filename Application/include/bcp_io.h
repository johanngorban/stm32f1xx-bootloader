#pragma once

#include "bcp.h"
#include "stm32f1xx_hal.h"

#define BCP_RECV_OK      0
#define BCP_RECV_TIMEOUT 1
#define BCP_RECV_ERROR  -1

void bcp_uart_init(UART_HandleTypeDef *huart);

int8_t bcp_send_response(const bcp_response_t *response);

int8_t bcp_recv_request(bcp_request_t *request);