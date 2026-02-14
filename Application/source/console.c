#include "console.h"
#include "ring_buffer.h"
#include "stm32f1xx.h"

#include <string.h>
#include <stdint.h>
#include <ctype.h>

static UART_HandleTypeDef *console_uart = NULL;

// Terminal buffer
static ring_buffer_t console_buffer;
static volatile uint8_t rx_ready = 0;

// TX Buffer
static ring_buffer_t tx_buffer;
static volatile uint8_t tx_busy = 0;
static volatile uint8_t tx_byte;

// RX Buffer
static ring_buffer_t rx_buffer;
static volatile uint8_t rx_byte;

static void console_rx_callback(UART_HandleTypeDef *);
static void console_tx_callback(UART_HandleTypeDef *);

uint8_t console_init(UART_HandleTypeDef *huart) {
    if (huart == NULL || console_uart != NULL) {
        return 1;
    }

    console_uart = huart;

    ring_buffer_init(&tx_buffer);
    tx_busy = 0;    

    ring_buffer_init(&rx_buffer);
    rx_ready = 0;

    ring_buffer_init(&console_buffer);

    HAL_UART_RegisterCallback(
        console_uart,
        HAL_UART_RX_COMPLETE_CB_ID,
        console_rx_callback
    );

    HAL_UART_RegisterCallback(
        console_uart,
        HAL_UART_TX_COMPLETE_CB_ID,
        console_tx_callback
    );

    HAL_UART_Receive_IT(console_uart, (uint8_t *) &rx_byte, 1);
    return 0;
}

void console_putc(char c) {
    HAL_UART_Transmit(console_uart, (uint8_t *) &c, 1, 10);
}

void console_puts(const char *str) {
    HAL_UART_Transmit(console_uart, (uint8_t *) str, strlen(str), 10);
}

void console_clear() {
    console_puts("\033[2J\033[H");
}

uint16_t console_gets(char *buffer) {
    if (buffer == NULL) {
        return 0;
    }

    if (rx_ready == 1) {
        uint16_t length = ring_buffer_get_length(&console_buffer);
        ring_buffer_read(&console_buffer, (uint8_t *) buffer, length);
        
        buffer[length] = '\0';
        ring_buffer_clear(&console_buffer);

        rx_ready = 0;

        return length;
    }

    while (ring_buffer_get_length(&rx_buffer) > 0) {
        char ch = ring_buffer_pop(&rx_buffer);
        if (ch == '\r' || ch == '\n') {
            rx_ready = 1;
            console_puts("\r\n");
        }
        else if (ch == '\b') {
            if (ring_buffer_get_length(&console_buffer) > 0) {
                ring_buffer_pop_back(&console_buffer);
                console_puts("\b \b");
            }
        }
        else if (ch >= 32 && ch <= 126) {
            if (ring_buffer_get_length(&console_buffer) < CONSOLE_MAX_RX_DATA_LENGTH - 1) {
                ring_buffer_push(&console_buffer, ch);
                console_putc(ch);
            }
        }
    }

    return 0;
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

static void console_rx_callback(UART_HandleTypeDef *console) {
    ring_buffer_push(&rx_buffer, rx_byte);
    HAL_UART_Receive_IT(console_uart, &rx_byte, 1);
}