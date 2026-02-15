#include "console.h"
#include "ring_buffer.h"
#include "stm32f1xx.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

static UART_HandleTypeDef *console_uart = NULL;

// Terminal buffer
static char console_buffer[CONSOLE_MAX_RX_DATA_LENGTH];
static volatile uint16_t console_buffer_length = 0;
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

    memset(console_buffer, 0, CONSOLE_MAX_RX_DATA_LENGTH);
    console_buffer_length = 0;

    HAL_UART_RegisterCallback(
        console_uart,
        HAL_UART_RX_COMPLETE_CB_ID,
        console_rx_callback
    );

    // HAL_UART_RegisterCallback(
    //     console_uart,
    //     HAL_UART_TX_COMPLETE_CB_ID,
    //     console_tx_callback
    // );

    HAL_UART_Receive_IT(console_uart, (uint8_t *) &rx_byte, 1);
    return 0;
}

void console_printf(const char *fmt, ...) {
    char buffer[CONSOLE_MAX_TX_DATA_LENGTH];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, CONSOLE_MAX_TX_DATA_LENGTH, fmt, args);
    console_puts(buffer);
    va_end(args);
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
        strcpy(buffer, console_buffer);
        buffer[console_buffer_length] = '\0';
        
        memset(console_buffer, 0, CONSOLE_MAX_RX_DATA_LENGTH);

        rx_ready = 0;

        uint16_t length = console_buffer_length;
        console_buffer_length = 0;

        return length;
    }

    while (ring_buffer_get_length(&rx_buffer) > 0) {
        char ch = ring_buffer_pop(&rx_buffer);
        if (ch == '\r' || ch == '\n') {
            rx_ready = 1;
            console_puts("\r\n");
        }
        else if (ch == '\b' || ch == 0x7F) {
            if (console_buffer_length > 0) {
                console_buffer_length--;
                console_buffer[console_buffer_length] = '\0';
                console_puts("\b \b");
            }
        }
        else if (ch >= 32 && ch <= 126) {
            if (console_buffer_length < CONSOLE_MAX_RX_DATA_LENGTH - 1) {
                console_buffer[console_buffer_length++] = ch;
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