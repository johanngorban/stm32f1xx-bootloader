#pragma once

#include "blip.h"

typedef void (*handler_t)(const blip_request_t *, blip_response_t *);

typedef struct {
    blip_command_t  id;
    handler_t       handler;
} router_entry_t;

void router_handle_request(const blip_request_t *request);