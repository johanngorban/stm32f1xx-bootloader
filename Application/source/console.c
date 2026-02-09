#include "console.h"
#include "ring_buffer.h"
#include "stm32f1xx.h"

#include <string.h>
#include <stdint.h>

static UART_HandleTypeDef *console_uart = NULL;
static IRQn_Type console_uart_irq_n;
static char line_buffer[CONSOLE_MAX_RX_DATA_LENGTH];
static uint16_t line_len = 0;

// TX Buffer
static ring_buffer_t tx_buffer;
static volatile uint8_t tx_busy = 0;
static volatile uint8_t tx_byte;

// RX Buffer
static ring_buffer_t rx_buffer;
static volatile uint8_t rx_byte;
static volatile uint8_t rx_ready = 0;

uint8_t console_init(UART_HandleTypeDef *huart, IRQn_Type irq_n) {
    if (huart == NULL || console_uart != NULL) {
        return 1;
    }

    console_uart = huart;
    console_uart_irq_n = irq_n;

    ring_buffer_init(&tx_buffer);
    tx_busy = 0;    

    ring_buffer_init(&rx_buffer);
    rx_ready = 0;

    HAL_UART_Receive_IT(console_uart, (uint8_t *) &rx_byte, 1);
    return 0;
}

void console_print(const char *str) {
    HAL_UART_Transmit(console_uart, (uint8_t *) str, strlen(str), 10);
    // ring_buffer_write(&tx_buffer, (uint8_t *) str, strlen(str));

    // if (tx_busy == 0) {
    //     tx_busy = 1;
    //     tx_byte = ring_buffer_pop(&tx_buffer);
    //     HAL_UART_Transmit_IT(console_uart, &tx_byte, 1);
    // }
}

void console_clear() {
    console_print("\033[2J\033[H");
}

void console_poll(void) {
    while (ring_buffer_get_length(&rx_buffer) > 0 && rx_ready == 0) {
        uint8_t ch = ring_buffer_pop(&rx_buffer);
        if ((ch == '\b' || ch == 0x7F) && line_len > 0) {
            line_len--;
            console_print("\b \b");
        }
        else if (ch == '\r' || ch == '\n') {
            line_buffer[line_len] = '\0';
            rx_ready = 1;
            console_print("\r\n");
            return;
        }
        else if (line_len < CONSOLE_MAX_RX_DATA_LENGTH - 1) {
            line_buffer[line_len++] = ch;
            char echo[2] = {ch, 0};
            console_print(echo);
        }
    }
}

uint8_t console_ready() {
    return rx_ready;
}

void console_read(char *buffer) {
    if (buffer == NULL || rx_ready == 0) {
        return;
    }
    
    strcpy(buffer, line_buffer);
    rx_ready = 0;
    line_len = 0;
    line_buffer[0] = '\0';
}

static void console_tx_callback() {
    if (ring_buffer_get_length(&tx_buffer) == 0) {
        tx_busy = 0;
    }
    else {
        tx_byte = ring_buffer_pop(&tx_buffer);
        HAL_UART_Transmit_IT(console_uart, &tx_byte, 1);
    }
}

static void console_rx_callback() {
    ring_buffer_push(&rx_buffer, rx_byte);
    HAL_UART_Receive_IT(console_uart, &rx_byte, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart) {
    if (uart == console_uart) {
        console_tx_callback();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart) {
    if (uart == console_uart) {
        console_rx_callback();
    }
}