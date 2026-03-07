#include "router.h"
#include "handlers.h"

static const router_entry_t router[] = {
    {BLIP_UPLOAD_FIRMWARE, handle_upload_firmware},
    {BLIP_UPDATE_FIRMWARE, handle_update_firmware},
    {BLIP_CALC_BANK_CRC, handle_calc_bank_crc},
    {BLIP_RUN_FIRMWARE, handle_run_firmware},
    {BLIP_GET_VERSION, handle_get_version},
};

static handler_t find_handler(blip_command_t id) {
    uint16_t length = sizeof(router) / sizeof(router_entry_t);
    
    handler_t handler = NULL;
    while (length--) {
        if (router[length - 1].id == id) {
            handler = router[length - 1].handler;
            break;
        }
    }

    return handler;
}

static void send_error(blip_command_t cmd, blip_status_t status) {
    blip_response_t response;
    blip_response_init(&response);

    response.command = cmd;
    response.status = status;
    // blip_calculate_crc16(&response);

    blip_send(&response);
}

static void send_stub(const blip_request_t *request) {
    blip_response_t response;
    blip_response_init(&response);

    response.command = request->command;
    response.status = BLIP_OK;
}

void router_handle_request(const blip_request_t *request) {
    if (request == NULL) {
        send_error(BLIP_UNKNOWN_COMMAND, BLIP_ERROR_INVALID_PARAM);
        return;
    }

    blip_response_t response;
    blip_response_init(&response);

    handler_t handler = find_handler(request->command);
    if (handler == NULL) {
        send_error(request->command, BLIP_ERROR_UNKNOWN_COMMAND);
        return;
    }
    // handler(request, &response);
    send_stub(request);

    blip_send(&response);
}