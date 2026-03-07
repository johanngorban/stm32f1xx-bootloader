#pragma once

#include "blip.h"

void handle_unknown_command(const blip_request_t *request, blip_response_t *response);

void handle_upload_firmware(const blip_request_t *request, blip_response_t *response);

void handle_update_firmware(const blip_request_t *request, blip_response_t *response);

void handle_calc_bank_crc(const blip_request_t *request, blip_response_t *response);

void handle_run_firmware(const blip_request_t *request, blip_response_t *response);

void handle_get_version(const blip_request_t *request, blip_response_t *response);