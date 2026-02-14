#include "ring_buffer.h"

#include <string.h>
#include <stddef.h>

static inline uint16_t move_right(uint16_t value) {
    return (value + 1) % RING_BUFFER_MAX_LENGTH;
}

uint16_t ring_buffer_get_freespace(const ring_buffer_t *buffer) {
    return RING_BUFFER_MAX_LENGTH - buffer->len - 1;
}

uint16_t ring_buffer_get_length(const ring_buffer_t *buffer) {
    return buffer->len;
}

void ring_buffer_init(ring_buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }

    buffer->head = 0;
    buffer->tail = 0;
    buffer->len = 0;
    memset(buffer->data, 0, RING_BUFFER_MAX_LENGTH);
}

void ring_buffer_clear(ring_buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }

    buffer->head = 0;
    buffer->tail = 0;
    buffer->len = 0;
    memset(buffer->data, 0, RING_BUFFER_MAX_LENGTH);
}

uint8_t ring_buffer_pop_back(ring_buffer_t *buffer) {
    if (buffer == NULL) {
        return '\0';
    }

    if (buffer->len == 0) {
        return '\0';
    }

    buffer->head = (buffer->head + RING_BUFFER_MAX_LENGTH - 1) % RING_BUFFER_MAX_LENGTH;
    uint8_t byte = buffer->data[buffer->head];
    buffer->len--;

    return byte;
}

uint8_t ring_buffer_pop(ring_buffer_t *buffer) {
    if (buffer == NULL) {
        return '\0';
    }

    if (buffer->len == 0) {
        return '\0';
    }

    char byte = buffer->data[buffer->tail];
    buffer->len--;
    buffer->tail = move_right(buffer->tail);
    
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
    buffer->head = move_right(buffer->head);
    buffer->len++;
}

uint16_t ring_buffer_read(
    ring_buffer_t *buffer,
    uint8_t *data,
    uint16_t length
) {
    if (data == NULL || buffer == NULL) {
        return 0;
    }

    if (length > buffer->len) {
        return 0;
    }

    char ch;
    uint16_t read_bytes = 0;

    if (buffer->head > buffer->tail) {
        memcpy(data, buffer->data + buffer->tail, length);
        buffer->len -= length;
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
    if (data == NULL || buffer == NULL) {
        return 0;
    }

    if (length >= RING_BUFFER_MAX_LENGTH) {
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

    return length;
}
