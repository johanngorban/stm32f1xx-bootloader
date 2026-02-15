#include "menu_handlers.h"
#include "terminal.h"
#include "menu.h"
#include "stm32f1xx_hal.h"

uint8_t rebootSystem() {
    terminal_clear();
    terminal_puts("Rebooting...\r\n");
    HAL_NVIC_SystemReset();
    return 0;
}

uint8_t displayMenu() {
    terminal_clear();
    menu_print(terminal_puts);
    return 0;
}

uint8_t unknownCommand() {
    terminal_puts("Unknown command\r\n");
    return 0;
}