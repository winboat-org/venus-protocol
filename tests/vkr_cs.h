/*
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: MIT
 */

#ifndef VKR_CS_H
#define VKR_CS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include "test_wire.h"
#define vkr_cs_encoder test_wire
#define vkr_cs_decoder test_wire

struct vkr_cs_encoder;
struct vkr_cs_decoder;

typedef uint64_t vkr_object_id;

struct vkr_object {
    union {
        uint64_t u64;
    } handle;
};

static inline bool
vkr_cs_encoder_acquire(struct vkr_cs_encoder *enc)
{
   return true;
}

static inline void
vkr_cs_encoder_release(struct vkr_cs_encoder *enc)
{
}

static inline void
vkr_cs_encoder_write(struct vkr_cs_encoder *enc,
                     size_t size,
                     const void *val,
                     size_t val_size)
{
   test_write(enc, size, val, val_size);
}

static inline void
vkr_cs_decoder_set_fatal(const struct vkr_cs_decoder *dec)
{
   ((struct test_wire *)dec)->fatal = true;
}

static inline bool
vkr_cs_decoder_get_fatal(const struct vkr_cs_decoder *dec)
{
   return dec->fatal;
}

static inline void
vkr_cs_decoder_read(struct vkr_cs_decoder *dec,
                    size_t size,
                    void *val,
                    size_t val_size)
{
   test_read(dec, size, val, val_size);
}

static inline void
vkr_cs_decoder_peek(const struct vkr_cs_decoder *dec,
                    size_t size,
                    void *val,
                    size_t val_size)
{
   struct test_wire *w = (struct test_wire *)dec;
   size_t pos = w->cursor;
   test_read(w, size, val, val_size);
   w->cursor = pos;
}

static inline struct vkr_object *
vkr_cs_decoder_lookup_object(const struct vkr_cs_decoder *dec,
                             vkr_object_id id,
                             VkObjectType type)
{
    if (!id)
        return NULL;
    static struct vkr_object pipeline = { .handle.u64 = 0xfeed1234 };
    if (id == 7 && type == VK_OBJECT_TYPE_PIPELINE)
        return &pipeline;
    vkr_cs_decoder_set_fatal(dec);
    return NULL;
}

static inline void
vkr_cs_decoder_reset_temp_pool(struct vkr_cs_decoder *dec)
{
}

static inline void *
vkr_cs_decoder_alloc_temp(struct vkr_cs_decoder *dec, size_t size)
{
    return test_alloc(dec, size);
}

static inline void *
vkr_cs_decoder_alloc_temp_array(struct vkr_cs_decoder *dec,
                                size_t size,
                                size_t count)
{
    if (size && count > SIZE_MAX / size) { dec->fatal = true; return NULL; }
    return test_alloc(dec, size * count);
}

static inline void *
vkr_cs_decoder_get_blob_storage(struct vkr_cs_decoder *dec, size_t size)
{
   return NULL;
}

static inline void *
vkr_cs_encoder_get_blob_storage(struct vkr_cs_encoder *enc, size_t offset, size_t size)
{
   return NULL;
}

static inline bool
vkr_cs_handle_indirect_id(VkObjectType type)
{
    return true;
}

static inline vkr_object_id
vkr_cs_handle_load_id(const void **handle, VkObjectType type)
{
    uint64_t id;
    memcpy(&id, handle, sizeof(id));
    return id;
}

static inline void
vkr_cs_handle_store_id(void **handle,
                       vkr_object_id id,
                       VkObjectType type)
{
    memcpy(handle, &id, sizeof(id));
}

#endif /* VKR_CS_H */
