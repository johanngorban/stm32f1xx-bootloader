#include "menu_handlers.h"
#include "console.h"
#include "menu.h"

uint8_t rebootSystem() {
    console_print("Rebooting...\r\n");
    return 0;
}

uint8_t helpCommand() {
    menu_print(console_print);
    return 0;
}

uint8_t unknownCommand() {
    console_print("Unknown command\r\n");
    return 0;
}