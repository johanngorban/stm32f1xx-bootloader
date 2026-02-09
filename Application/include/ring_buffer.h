#pragma once

#include <stdint.h>

#define RING_BUFFER_MAX_LENGTH (256)

typedef struct {
    uint8_t data[RING_BUFFER_MAX_LENGTH];
    uint16_t head;
    uint16_t tail;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *buffer);

uint16_t ring_buffer_get_length(const ring_buffer_t *buffer);

uint16_t ring_buffer_get_freespace(const ring_buffer_t *buffer);

uint8_t ring_buffer_pop(ring_buffer_t *buffer);

void ring_buffer_push(
    ring_buffer_t *buffer,
    uint8_t byte
);

uint16_t ring_buffer_read(
    ring_buffer_t *buffer,
    uint8_t *data,
    uint16_t length
);

uint16_t ring_buffer_write(
    ring_buffer_t *buffer,
    const uint8_t *data,
    uint16_t length
);