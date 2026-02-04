#include "menu_handlers.h"
#include "console.h"
#include "menu.h"
#include "stm32f1xx_hal.h"

uint8_t rebootSystem() {
    console_clear();
    console_print("Rebooting...\r\n");
    HAL_NVIC_SystemReset();
    return 0;
}

uint8_t displayMenu() {
    console_clear();
    menu_print(console_print);
    return 0;
}

uint8_t unknownCommand() {
    console_print("Unknown command\r\n");
    return 0;
}