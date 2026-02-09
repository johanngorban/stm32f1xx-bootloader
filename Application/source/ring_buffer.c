#include "ring_buffer.h"

#include <string.h>
#include <stddef.h>

uint16_t ring_buffer_get_freespace(const ring_buffer_t *buffer) {
	if (buffer->tail == buffer->head) {
        return RING_BUFFER_MAX_LENGTH - 1;
    }

    if ((buffer->head + 1) % RING_BUFFER_MAX_LENGTH == buffer->tail) {
        return 0;
    }

    if (buffer->head > buffer->tail) {
        return RING_BUFFER_MAX_LENGTH - (buffer->head - buffer->tail + 1);
    }

    return buffer->tail - buffer->head - 1;
}

uint16_t ring_buffer_get_length(const ring_buffer_t *buffer) {
    return RING_BUFFER_MAX_LENGTH - ring_buffer_get_freespace(buffer) - 1;
}

void ring_buffer_init(ring_buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }

    buffer->head = 0;
    buffer->tail = 0;
    memset(buffer->data, 0, RING_BUFFER_MAX_LENGTH);
}

uint8_t ring_buffer_pop(ring_buffer_t *buffer) {
    if (buffer == NULL) {
        return '\0';
    }

    if (ring_buffer_get_length(buffer) == 0) {
        return '\0';
    }

    char byte = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) % RING_BUFFER_MAX_LENGTH;
    return byte;
}

void ring_buffer_push(
    ring_buffer_t *buffer,
    uint8_t byte
) {
    if (buffer == NULL || ring_buffer_get_freespace(buffer) == 0) {
        return;
    }

    buffer->data[buffer->head] = byte;
    buffer->head = (buffer->head + 1) % RING_BUFFER_MAX_LENGTH;
}

uint16_t ring_buffer_read(
    ring_buffer_t *buffer,
    uint8_t *data,
    uint16_t length
) {
    if (data == NULL || buffer == NULL) {
        return 0;
    }

    if (length > ring_buffer_get_length(buffer)) {
        return 0;
    }

    char ch;
    uint16_t read_bytes = 0;

    if (buffer->head > buffer->tail) {
        memcpy(data, buffer->data + buffer->tail, length);
        buffer->tail = (buffer->tail + length) % RING_BUFFER_MAX_LENGTH;
        read_bytes = length;
    }
    else {
        while (buffer->tail != buffer->head && read_bytes < length) {
            ch = ring_buffer_pop(buffer);
            data[read_bytes++] = ch;
        }
    }
    
    return read_bytes;
}

uint16_t ring_buffer_write(
    ring_buffer_t *buffer,
    const uint8_t *data,
    uint16_t length
) {
    if (data == NULL || buffer == NULL || length > RING_BUFFER_MAX_LENGTH) {
        return 0;
    }

    uint16_t freespace = ring_buffer_get_freespace(buffer);

    // Ring buffer is full
    if (freespace == 0) {
        return 0;
    }

    // No sufficient space
    if (freespace < length) {
        return 0;
    }

    uint16_t bytes_written = 0;
    while (bytes_written < length) {
        ring_buffer_push(buffer, data[bytes_written++]);
    }

    // if (buffer->tail < buffer->head) {
    //     memcpy(buffer->data + buffer->head, data, length);
    //     buffer->head = (buffer->head + length) % RING_BUFFER_MAX_LENGTH;
    // }
    // else {
    //     uint16_t bytes_written = 0;
    //     while (bytes_written < length) {
    //         ring_buffer_push(buffer, data[bytes_written++]);
    //     }
    //     // memcpy(buffer->data + buffer->head, data, RING_BUFFER_MAX_LENGTH - buffer->head);
    //     // uint16_t old_head = buffer->head;
    //     // buffer->head += (buffer->head + length) % RING_BUFFER_MAX_LENGTH;
    //     // memcpy(buffer->data + buffer->head, data + old_head, length + old_head - RING_BUFFER_MAX_LENGTH);
    // }

    return length;
}
