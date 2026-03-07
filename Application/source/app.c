#include "app.h"
#include "blip.h"
#include "router.h"
#include <stdbool.h>

extern UART_HandleTypeDef huart1;

void app_init() {
    blip_uart_init(&huart1);
}

void app_run() {
    blip_request_t request;
    bool activity_detected = false;

    while(1) {
        // if (!activity_detected && waited >= MAX_WAIT_TIME_MS) {
        //     jump_to_app();
        // }

        if (blip_receive(&request)) {
            activity_detected = true;

            router_handle_request(&request);
        }
    }
}