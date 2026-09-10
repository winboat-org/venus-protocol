/* Copyright 2026 Helios contributors; SPDX-License-Identifier: MIT */
#include "test_wire.h"
#include <stdio.h>
int main(void)
{
    static struct test_wire w;
    VkIndirectCommandsPushConstantTokenEXT push = { .updateRange = {VK_SHADER_STAGE_VERTEX_BIT, 4, 8} };
    VkIndirectCommandsVertexBufferTokenEXT vb = { .vertexBindingUnit = 15 };
    VkIndirectCommandsIndexBufferTokenEXT ib = { .mode = VK_INDIRECT_COMMANDS_INPUT_MODE_DXGI_INDEX_BUFFER_EXT };
    VkIndirectCommandsExecutionSetTokenEXT set = { .type = VK_INDIRECT_EXECUTION_SET_INFO_TYPE_PIPELINES_EXT, .shaderStages = VK_SHADER_STAGE_VERTEX_BIT };
    VkIndirectCommandsLayoutTokenEXT tokens[] = {
        { .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT, .type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_PUSH_CONSTANT_EXT, .data.pPushConstant = &push, .offset = 4 },
        { .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT, .type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_SEQUENCE_INDEX_EXT, .data.pPushConstant = &push, .offset = 8 },
        { .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT, .type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_VERTEX_BUFFER_EXT, .data.pVertexBuffer = &vb, .offset = 16 },
        { .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT, .type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_INDEX_BUFFER_EXT, .data.pIndexBuffer = &ib, .offset = 32 },
        { .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT, .type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_EXECUTION_SET_EXT, .data.pExecutionSet = &set, .offset = 48 },
        { .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT, .type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_EXT, .data.pPushConstant = (void *)(uintptr_t)1, .offset = 64 },
    };
    VkIndirectCommandsLayoutCreateInfoEXT info = {
        .sType = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_CREATE_INFO_EXT,
        .flags = VK_INDIRECT_COMMANDS_LAYOUT_USAGE_EXPLICIT_PREPROCESS_BIT_EXT,
        .shaderStages = VK_SHADER_STAGE_VERTEX_BIT, .indirectStride = 96,
        .tokenCount = sizeof(tokens) / sizeof(tokens[0]), .pTokens = tokens,
    }, output;
    const VkIndirectCommandsTokenTypeEXT actions[] = {
        VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_EXT, VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_INDEXED_EXT,
        VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_COUNT_EXT, VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_INDEXED_COUNT_EXT,
        VK_INDIRECT_COMMANDS_TOKEN_TYPE_DISPATCH_EXT, VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_MESH_TASKS_EXT,
        VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_MESH_TASKS_COUNT_EXT, VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_MESH_TASKS_NV_EXT,
        VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_MESH_TASKS_COUNT_NV_EXT, VK_INDIRECT_COMMANDS_TOKEN_TYPE_TRACE_RAYS2_EXT,
    };
    for (unsigned i = 0; i < sizeof(actions) / sizeof(actions[0]); i++) {
        memset(&w, 0, sizeof(w));
        tokens[5].type = actions[i];
        test_encode_layout(&w, &info);
        test_decode_layout(&w, &output);
        TEST_CHECK(!w.fatal && w.cursor == w.size);
        TEST_CHECK(output.flags == info.flags && output.indirectStride == 96 && output.tokenCount == 6);
        TEST_CHECK(output.pTokens[0].data.pPushConstant->updateRange.offset == 4);
        TEST_CHECK(output.pTokens[1].data.pPushConstant->updateRange.size == 8);
        TEST_CHECK(output.pTokens[2].data.pVertexBuffer->vertexBindingUnit == 15);
        TEST_CHECK(output.pTokens[3].data.pIndexBuffer->mode == ib.mode);
        TEST_CHECK(output.pTokens[4].data.pExecutionSet->shaderStages == set.shaderStages);
        TEST_CHECK(output.pTokens[5].type == actions[i] && output.pTokens[5].offset == 64);
    }
    size_t full = w.size;
    for (size_t size = 0; size < full; size++) {
        w.size = size; w.cursor = w.temp_size = 0; w.fatal = false;
        test_decode_layout(&w, &output);
        TEST_CHECK(w.fatal);
    }
    VkIndirectExecutionSetPipelineInfoEXT pipeline = {
        .sType = VK_STRUCTURE_TYPE_INDIRECT_EXECUTION_SET_PIPELINE_INFO_EXT,
        .initialPipeline = (VkPipeline)(uintptr_t)7, .maxPipelineCount = 32,
    };
    VkIndirectExecutionSetCreateInfoEXT create = {
        .sType = VK_STRUCTURE_TYPE_INDIRECT_EXECUTION_SET_CREATE_INFO_EXT,
        .type = VK_INDIRECT_EXECUTION_SET_INFO_TYPE_PIPELINES_EXT, .info.pPipelineInfo = &pipeline,
    }, decoded;
    memset(&w, 0, sizeof(w));
    test_encode_set(&w, &create);
    test_decode_set(&w, &decoded);
    TEST_CHECK(!w.fatal && w.cursor == w.size);
    TEST_CHECK(decoded.info.pPipelineInfo->initialPipeline == (VkPipeline)(uintptr_t)0xfeed1234);
    TEST_CHECK(decoded.info.pPipelineInfo->maxPipelineCount == 32);
    /* The outer type and serialized union tag are both 0. Change only the
     * outer type, so a decoder must reject before treating pipeline memory as
     * the differently sized shader-object arm. */
    TEST_CHECK(w.size >= 16);
    uint32_t bad = VK_INDIRECT_EXECUTION_SET_INFO_TYPE_SHADER_OBJECTS_EXT;
    memcpy(w.bytes + 12, &bad, sizeof(bad));
    w.cursor = w.temp_size = 0; w.fatal = false;
    test_decode_set(&w, &decoded);
    TEST_CHECK(w.fatal);
    puts("PASS: DGC union arms, ignored action payloads, exact lengths, pipeline handle translation, truncation and selector mismatch");
    return 0;
}
