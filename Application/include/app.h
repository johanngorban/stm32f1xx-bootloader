#pragma once

#include "stm32f1xx_hal.h"

typedef struct {
    UART_HandleTypeDef *consoleUart;
} app_context_t;

void app_init(const app_context_t *ctx);

void app_start();