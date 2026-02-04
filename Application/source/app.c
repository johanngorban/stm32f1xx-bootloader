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
    char consoleInput[CONSOLE_MAX_INPUT_DATA_LENGTH];
    memset(consoleInput, 0, CONSOLE_MAX_INPUT_DATA_LENGTH);

    while(1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);

        uint8_t bytesReceived = console_read(consoleInput);
        if (bytesReceived > 0) {
            menu_execute(consoleInput);
        }
    }
}