/*
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: MIT
 */

#include "vn_protocol_driver.h"

size_t test_encode_layout(struct test_wire *w, const VkIndirectCommandsLayoutCreateInfoEXT *info)
{
    size_t size = vn_sizeof_VkIndirectCommandsLayoutCreateInfoEXT(info);
    vn_encode_VkIndirectCommandsLayoutCreateInfoEXT(w, info);
    TEST_CHECK(w->size == size && !w->fatal);
    return size;
}
size_t test_encode_set(struct test_wire *w, const VkIndirectExecutionSetCreateInfoEXT *info)
{
    size_t size = vn_sizeof_VkIndirectExecutionSetCreateInfoEXT(info);
    vn_encode_VkIndirectExecutionSetCreateInfoEXT(w, info);
    TEST_CHECK(w->size == size && !w->fatal);
    return size;
}
