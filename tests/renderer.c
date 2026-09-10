/*
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: MIT
 */

#include "vn_protocol_renderer.h"

void test_decode_layout(struct test_wire *w, VkIndirectCommandsLayoutCreateInfoEXT *info)
{
    vn_decode_VkIndirectCommandsLayoutCreateInfoEXT_temp((struct vn_cs_decoder *)w, info);
    if (!w->fatal)
        vn_replace_VkIndirectCommandsLayoutCreateInfoEXT_handle(info);
}
void test_decode_set(struct test_wire *w, VkIndirectExecutionSetCreateInfoEXT *info)
{
    vn_decode_VkIndirectExecutionSetCreateInfoEXT_temp((struct vn_cs_decoder *)w, info);
    if (!w->fatal)
        vn_replace_VkIndirectExecutionSetCreateInfoEXT_handle(info);
}
