#pragma once

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define CONSOLE_MAX_RX_DATA_LENGTH (128)

uint8_t console_init(UART_HandleTypeDef *huart);

void console_poll();

void console_print(const char *str);

void console_clear();

uint8_t console_read(char *buffer);