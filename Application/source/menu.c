#include "menu.h"
#include "menu_handlers.h"
#include <string.h>

static menu_entry_t *menu_entries = NULL;
static menu_handler_t menu_default_handler = NULL;
static uint8_t menu_command_count = 0;

uint8_t menu_init(
    const menu_entry_t *entries,
    uint8_t entries_count,
    menu_handler_t default_handler
) {
    if (entries == NULL || default_handler == NULL || menu_entries != NULL) {
        return 1;
    }

    menu_command_count = entries_count;
    menu_entries = entries;
    menu_default_handler = default_handler;

    return 0;
}

menu_handler_t find_menu_handler(const char *opcode) {
    if (opcode == NULL) {
        return NULL;
    }

    menu_handler_t handler = NULL;
    for (uint8_t i = 0; i < menu_command_count; i++) {
        if (strcmp(menu_entries[i].opcode, opcode) == 0) {
            handler = menu_entries[i].handler;
            break;
        }
    }

    return handler;
}

uint8_t menu_execute(const char *opcode) {
    if (menu_entries == NULL) {
        return 1;
    }

    menu_handler_t handler = find_menu_handler(opcode);
    if (handler == NULL) {
        handler = menu_default_handler;
    }

    return handler();
}

void menu_print(menu_printer_t print) {
    if (print == NULL) {
        return;
    }

    print("\033[2J\033[H");
    print("\033[10;20H");
    
    for (uint8_t i = 0; i < menu_command_count; i++) {
        // print("%s. %s\n", menu_entries[i].opcode, menu_entries[i].description);
        print(menu_entries[i].opcode);
        print(". ");
        print(menu_entries[i].description);
        print("\n\r");
    }
    print("cmd>");
}