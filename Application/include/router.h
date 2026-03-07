#pragma once

#include "bcp.h"

typedef void (*handler_t)(const bcp_request_t *, bcp_response_t *);

typedef struct {
    bcp_command_t  id;
    handler_t       handler;
} router_entry_t;

void router_handle_request(const bcp_request_t *request);