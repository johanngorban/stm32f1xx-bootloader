#include "console.h"
#include "stm32f1xx.h"

#include <string.h>
#include <stdint.h>

static UART_HandleTypeDef *console_uart = NULL;

static volatile uint8_t rx_byte;
static volatile uint8_t rx_ready = 0;
static volatile uint16_t rx_index = 0;
static char rx_buffer[CONSOLE_MAX_INPUT_DATA_LENGTH];
static char rx_temp_char;

uint8_t console_init(UART_HandleTypeDef *huart) {
    if (huart == NULL || console_uart != NULL) {
        return 1;
    }

    console_uart = huart;

    HAL_UART_Receive_IT(console_uart, (uint8_t *) &rx_temp_char, 1);
    return 0;
}

void console_print(const char *str) {
    if (str == NULL) {
        return;
    }

    HAL_UART_Transmit_IT(console_uart, (uint8_t *) str, strlen(str));
    while (
        HAL_UART_GetState(console_uart) == HAL_UART_STATE_BUSY_TX ||
        HAL_UART_GetState(console_uart) == HAL_UART_STATE_BUSY_TX_RX
    );
}

uint8_t console_read(char *buffer) {
    if (buffer == NULL || rx_ready == 0) {
        return 0;
    }

    __disable_irq();

    strcpy(buffer, rx_buffer);
    rx_ready = 0;

    __enable_irq();

    return strlen(buffer);
}

static void console_rx_callback() {
    if (console_uart == NULL) {
        return;
    }

    if (rx_index >= CONSOLE_MAX_INPUT_DATA_LENGTH - 1) {
        return;
    }

    if (rx_temp_char == '\r' || rx_temp_char == '\n') {
        rx_buffer[rx_index] = '\0';
        rx_index = 0;
        rx_ready = 1;
        HAL_UART_Transmit(console_uart, (uint8_t *) "\r\n", 2, 10);
    }
    else {
        if (rx_temp_char == '\b' && rx_index > 0) {
            rx_index--;
            HAL_UART_Transmit(console_uart, (uint8_t *) "\b \b", 3, 10);
        }
        else {
            rx_buffer[rx_index++] = rx_temp_char;
            HAL_UART_Transmit(console_uart, (uint8_t *) &rx_temp_char, 1, 10);
        }
    }

    HAL_UART_Receive_IT(console_uart, (uint8_t *) &rx_temp_char, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart) {
    if (console_uart != NULL && uart == console_uart) {
        console_rx_callback();
    }
}