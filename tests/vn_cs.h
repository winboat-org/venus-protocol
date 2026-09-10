/*
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: MIT
 */

#ifndef VN_CS_H
#define VN_CS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include "test_wire.h"
#define vn_cs_encoder test_wire
#define vn_cs_decoder test_wire

typedef uint64_t vn_object_id;

struct vn_cs_encoder;
struct vn_cs_decoder;

static inline bool
vn_cs_renderer_protocol_has_api_version(uint32_t api_version)
{
    return true;
}

static inline bool
vn_cs_renderer_protocol_has_extension(uint32_t ext_number)
{
    return true;
}

static inline size_t
vn_cs_encoder_get_len(const struct vn_cs_encoder *enc)
{
    return enc->size;
}

static inline bool
vn_cs_encoder_reserve(struct vn_cs_encoder *cs, size_t size)
{
    return size <= sizeof(cs->bytes) - cs->size;
}

static inline void
vn_cs_encoder_write(struct vn_cs_encoder *cs, size_t size, const void *val, size_t val_size)
{
    test_write(cs, size, val, val_size);
}

static inline void
vn_cs_decoder_set_fatal(struct vn_cs_decoder *dec)
{
    dec->fatal = true;
}

static inline void
vn_cs_decoder_read(struct vn_cs_decoder *dec, size_t size, void *val, size_t val_size)
{
    test_read(dec, size, val, val_size);
}

static inline void
vn_cs_decoder_peek(struct vn_cs_decoder *dec, size_t size, void *val, size_t val_size)
{
    size_t pos = dec->cursor;
    test_read(dec, size, val, val_size);
    dec->cursor = pos;
}

static inline vn_object_id
vn_cs_handle_load_id(const void **handle, VkObjectType type)
{
    uint64_t id;
    memcpy(&id, handle, sizeof(id));
    return id;
}

static inline void
vn_cs_handle_store_id(void **handle, vn_object_id id, VkObjectType type)
{
    memcpy(handle, &id, sizeof(id));
}

#endif /* VN_CS_H */
