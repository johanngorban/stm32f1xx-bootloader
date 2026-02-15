#include "terminal.h"
#include "ring_buffer.h"
#include "stm32f1xx.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

static UART_HandleTypeDef *terminal_uart = NULL;

// Terminal buffer
static char terminal_buffer[TERMINAL_MAX_RX_DATA_LENGTH];
static volatile uint16_t terminal_buffer_length = 0;
static volatile uint8_t rx_ready = 0;

// TX Buffer
static ring_buffer_t tx_buffer;
static volatile uint8_t tx_busy = 0;
static volatile uint8_t tx_byte;

// RX Buffer
static ring_buffer_t rx_buffer;
static volatile uint8_t rx_byte;

static void terminal_rx_callback(UART_HandleTypeDef *);
static void terminal_tx_callback(UART_HandleTypeDef *);

uint8_t terminal_init(UART_HandleTypeDef *huart) {
    if (huart == NULL || terminal_uart != NULL) {
        return 1;
    }

    terminal_uart = huart;

    ring_buffer_init(&tx_buffer);
    tx_busy = 0;    

    ring_buffer_init(&rx_buffer);
    rx_ready = 0;

    memset(terminal_buffer, 0, TERMINAL_MAX_RX_DATA_LENGTH);
    terminal_buffer_length = 0;

    HAL_UART_RegisterCallback(
        terminal_uart,
        HAL_UART_RX_COMPLETE_CB_ID,
        terminal_rx_callback
    );

    // HAL_UART_RegisterCallback(
    //     terminal_uart,
    //     HAL_UART_TX_COMPLETE_CB_ID,
    //     terminal_tx_callback
    // );

    HAL_UART_Receive_IT(terminal_uart, (uint8_t *) &rx_byte, 1);
    return 0;
}

void terminal_printf(const char *fmt, ...) {
    char buffer[TERMINAL_MAX_TX_DATA_LENGTH];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, TERMINAL_MAX_TX_DATA_LENGTH, fmt, args);
    terminal_puts(buffer);
    va_end(args);
}

void terminal_putc(char c) {
    HAL_UART_Transmit(terminal_uart, (uint8_t *) &c, 1, 10);
}


void terminal_puts(const char *str) {

    HAL_UART_Transmit(terminal_uart, (uint8_t *) str, strlen(str), 10);
}

void terminal_clear() {
    terminal_puts("\033[2J\033[H");
}

uint16_t terminal_gets(char *buffer) {
    if (buffer == NULL) {
        return 0;
    }

    if (rx_ready == 1) {
        strcpy(buffer, terminal_buffer);
        buffer[terminal_buffer_length] = '\0';
        
        memset(terminal_buffer, 0, TERMINAL_MAX_RX_DATA_LENGTH);

        rx_ready = 0;

        uint16_t length = terminal_buffer_length;
        terminal_buffer_length = 0;

        return length;
    }

    while (ring_buffer_get_length(&rx_buffer) > 0) {
        char ch = ring_buffer_pop(&rx_buffer);
        if (ch == '\r' || ch == '\n') {
            rx_ready = 1;
            terminal_puts("\r\n");
        }
        else if (ch == '\b' || ch == 0x7F) {
            if (terminal_buffer_length > 0) {
                terminal_buffer_length--;
                terminal_buffer[terminal_buffer_length] = '\0';
                terminal_puts("\b \b");
            }
        }
        else if (ch >= 32 && ch <= 126) {
            if (terminal_buffer_length < TERMINAL_MAX_RX_DATA_LENGTH - 1) {
                terminal_buffer[terminal_buffer_length++] = ch;
                terminal_putc(ch);
            }
        }
    }

    return 0;
}

static void terminal_tx_callback(UART_HandleTypeDef *terminal) {
    if (ring_buffer_get_length(&tx_buffer) == 0) {
        tx_busy = 0;
    }
    else {
        tx_byte = ring_buffer_pop(&tx_buffer);
        HAL_UART_Transmit_IT(terminal_uart, &tx_byte, 1);
    }
}

static void terminal_rx_callback(UART_HandleTypeDef *terminal) {
    ring_buffer_push(&rx_buffer, rx_byte);
    HAL_UART_Receive_IT(terminal_uart, &rx_byte, 1);
}