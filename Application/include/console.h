#pragma once

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define CONSOLE_MAX_RX_DATA_LENGTH (128)
#define CONSOLE_MAX_TX_DATA_LENGTH (64)

uint8_t console_init(UART_HandleTypeDef *huart);

void console_printf(const char *fmt, ...);

void console_puts(const char *str);

void console_clear();

uint16_t console_gets(char *buffer);