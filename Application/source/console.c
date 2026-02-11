#include "console.h"
#include "ring_buffer.h"
#include "stm32f1xx.h"

#include <string.h>
#include <stdint.h>

static UART_HandleTypeDef *console_uart = NULL;

// TX Buffer
static ring_buffer_t tx_buffer;
static volatile uint8_t tx_busy = 0;
static volatile uint8_t tx_byte;

static void console_tx_callback(UART_HandleTypeDef *);

uint8_t console_init(UART_HandleTypeDef *huart) {
    if (huart == NULL || console_uart != NULL) {
        return 1;
    }

    console_uart = huart;

    ring_buffer_init(&tx_buffer);
    tx_busy = 0;

    HAL_UART_RegisterCallback(
        console_uart,
        HAL_UART_TX_COMPLETE_CB_ID,
        console_tx_callback
    );

    return 0;
}

void console_print(const char *str) {
    ring_buffer_write(&tx_buffer, (uint8_t *) str, strlen(str));

    if (tx_busy == 0) {
        tx_busy = 1;
        tx_byte = ring_buffer_pop(&tx_buffer);
        HAL_UART_Transmit_IT(console_uart, &tx_byte, 1);
    }
}

void console_clear() {
    console_print("\033[2J\033[H");
}

void console_read(char *buffer, uint16_t length) {
    HAL_UART_Receive(console_uart, (uint8_t *) buffer, length, 0);
}

static void console_tx_callback(UART_HandleTypeDef *console) {
    if (ring_buffer_get_length(&tx_buffer) == 0) {
        tx_busy = 0;
    }
    else {
        tx_byte = ring_buffer_pop(&tx_buffer);
        HAL_UART_Transmit_IT(console_uart, &tx_byte, 1);
    }
}