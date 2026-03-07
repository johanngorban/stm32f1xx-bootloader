#include "app.h"
#include "bcp.h"
#include "router.h"
#include <stdbool.h>

extern UART_HandleTypeDef huart1;

void app_init() {
    bcp_uart_init(&huart1);
}

void app_run() {
    bcp_request_t request;
    bool activity_detected = false;

    while(1) {
        // if (!activity_detected && waited >= MAX_WAIT_TIME_MS) {
        //     jump_to_app();
        // }

        if (bcp_receive(&request)) {
            activity_detected = true;

            router_handle_request(&request);
        }
    }
}