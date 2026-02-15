#include "app.h"
#include "menu.h"
#include "menu_handlers.h"
#include "console.h"
#include <string.h>

const static menu_entry_t menu_entries[] = {
    {"Reboot", "Reboot system", rebootSystem},
    {"?", "Display menu", displayMenu},
};

// static app_context_t *appContext = NULL;

void app_init(const app_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }

    console_init(ctx->consoleUart);    
    menu_init(menu_entries, 2, unknownCommand);
}

void app_start() {
    uint32_t lastBlink = 0;

    char consoleInput[CONSOLE_MAX_RX_DATA_LENGTH];
    memset(consoleInput, 0, CONSOLE_MAX_RX_DATA_LENGTH);

    console_clear();

    while(1) {
        if (HAL_GetTick() - lastBlink >= 500) {
            lastBlink = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }

        if (console_gets(consoleInput)) {
            // menu_execute(consoleInput);
            console_puts(consoleInput);
            console_puts("\n");
        }
    }
}