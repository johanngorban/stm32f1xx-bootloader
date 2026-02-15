#include "app.h"
#include "menu.h"
#include "menu_handlers.h"
#include "terminal.h"
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

    terminal_init(ctx->terminalUart);    
    menu_init(menu_entries, 2, unknownCommand);
}

void app_start() {
    uint32_t lastBlink = 0;

    char terminalInput[TERMINAL_MAX_RX_DATA_LENGTH];
    memset(terminalInput, 0, TERMINAL_MAX_RX_DATA_LENGTH);

    terminal_clear();

    while(1) {
        if (HAL_GetTick() - lastBlink >= 500) {
            lastBlink = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }

        if (terminal_gets(terminalInput)) {
            // menu_execute(terminalInput);
            terminal_printf("%s\r\n", terminalInput);
        }
    }
}