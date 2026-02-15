#pragma once

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define TERMINAL_MAX_RX_DATA_LENGTH (128)
#define TERMINAL_MAX_TX_DATA_LENGTH (64)

uint8_t terminal_init(UART_HandleTypeDef *huart);

void terminal_printf(const char *fmt, ...);

void terminal_putc(char c);

void terminal_puts(const char *str);

void terminal_clear();

uint16_t terminal_gets(char *buffer);