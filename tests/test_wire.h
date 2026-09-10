/* Copyright 2026 Helios contributors; SPDX-License-Identifier: MIT */
#ifndef TEST_WIRE_H
#define TEST_WIRE_H
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define TEST_CHECK(x) do { if (!(x)) abort(); } while (0)
struct test_wire {
    uint8_t bytes[65536];
    size_t size, cursor, temp_size;
    bool fatal;
    _Alignas(16) uint8_t temp[65536];
};
static inline void test_write(struct test_wire *w, size_t size, const void *p, size_t n)
{
    TEST_CHECK(n <= size);
    if (size > sizeof(w->bytes) - w->size) { w->fatal = true; return; }
    memset(w->bytes + w->size, 0, size);
    if (n) memcpy(w->bytes + w->size, p, n);
    w->size += size;
}
static inline void test_read(struct test_wire *w, size_t size, void *p, size_t n)
{
    TEST_CHECK(n <= size);
    if (size > w->size - w->cursor) {
        w->fatal = true;
        if (n) memset(p, 0, n);
        return;
    }
    if (n) memcpy(p, w->bytes + w->cursor, n);
    w->cursor += size;
}
static inline void *test_alloc(struct test_wire *w, size_t size)
{
    size_t at = (w->temp_size + 15) & ~(size_t)15;
    if (at > sizeof(w->temp) || size > sizeof(w->temp) - at) {
        w->fatal = true; return NULL;
    }
    w->temp_size = at + size;
    memset(w->temp + at, 0, size);
    return w->temp + at;
}
size_t test_encode_layout(struct test_wire *w, const VkIndirectCommandsLayoutCreateInfoEXT *info);
size_t test_encode_set(struct test_wire *w, const VkIndirectExecutionSetCreateInfoEXT *info);
void test_decode_layout(struct test_wire *w, VkIndirectCommandsLayoutCreateInfoEXT *info);
void test_decode_set(struct test_wire *w, VkIndirectExecutionSetCreateInfoEXT *info);
#endif
