#pragma once

#include <stdint.h>

#define OPCODE_MAX_LENGTH (32)
#define DESCRIPTION_MAX_LENGTH (128)

// typedef void (*menu_printer_t)(const char *, ...);
typedef void (*menu_printer_t)(const char *str);
typedef uint8_t (*menu_handler_t)(void);

typedef struct {
    char opcode[OPCODE_MAX_LENGTH];
    char description[DESCRIPTION_MAX_LENGTH];
    menu_handler_t handler;
} menu_entry_t;

uint8_t menu_register_command(
    const char *opcode,
    const char *description,
    menu_handler_t handler
);

uint8_t menu_init(
    const menu_entry_t *menu_entries,
    uint8_t entries_count,
    menu_handler_t default_handler
);

menu_handler_t find_menu_handler(const char *opcode);

uint8_t menu_execute(const char *opcode);

void menu_print(menu_printer_t printer);
