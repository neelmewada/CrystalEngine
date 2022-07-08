
#include "RenderContextVk.h"
#include "GraphicsPipelineStateVk.h"

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <functional>
#include <array>

using namespace Vox;

RenderContextVk::RenderContextVk(RenderContextCreateInfoVk &renderContextInfo)
{
    m_pEngine = dynamic_cast<EngineContextVk*>(renderContextInfo.engineContext);
    m_pDevice = dynamic_cast<DeviceContextVk*>(renderContextInfo.deviceContext);
    m_pSwapChain = dynamic_cast<SwapChainVk*>(renderContextInfo.swapChain);
    m_pSwapChain->SetRenderContext(this);

    std::cout << "Created RenderContextVk" << std::endl;
}

RenderContextVk::~RenderContextVk()
{


    std::cout << "Destroyed RenderContextVk" << std::endl;
}

#pragma region Public API

void RenderContextVk::SetClearColor(float clearColor[4])
{
    for (int i = 0; i < 4; ++i)
        m_ClearColor[i] = clearColor[i];
}

void RenderContextVk::ReRecordCommands()
{
    if (m_RenderCommands.empty())
    {
        std::cerr << "ReRecordCommands() called while there are no recorded commands!" << std::endl;
    }
    EndRecording();
}

void RenderContextVk::ClearRecording()
{
    m_IsRecording = false;
    m_RenderCommands.clear();
}

void RenderContextVk::BeginRecording()
{
    m_IsRecording = true;

    m_RenderCommands.clear();
}

void RenderContextVk::CmdBindPipeline(IGraphicsPipelineState *pPipeline)
{
    GraphicsPipelineStateVk* pPipelineVk = dynamic_cast<GraphicsPipelineStateVk*>(pPipeline);
    if (pPipelineVk == nullptr)
    {
        throw std::runtime_error("Failed to bind pipeline! Pipeline passed to CmdBindPipeline is not of type GraphicsPipelineStateVk!");
    }

    auto swapChain = m_pSwapChain;

    m_RenderCommands.push_back([pPipelineVk, swapChain](VkCommandBuffer commandBuffer) -> void {
        auto descriptorSet = pPipelineVk->GetDescriptorSet(swapChain->GetCurrentFrameIndex());
        if (descriptorSet != nullptr)
        {
            // Dynamic Offset amount
            uint32_t dynamicOffset = static_cast<uint32_t>(pPipelineVk->GetDynamicUniformBufferAlignment()) * 0; // 0 = mesh index in dynamic uniform buffer
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipelineVk->GetPipelineLayout(),
                                    0, 1, &descriptorSet, 1,&dynamicOffset);
        }
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipelineVk->GetPipeline());
    });
}

void RenderContextVk::CmdSetConstants(IGraphicsPipelineState* pPipeline, uint32_t offset, uint32_t size, const void* pValues)
{
    auto* pPipelineVk = dynamic_cast<GraphicsPipelineStateVk*>(pPipeline);
    if (pPipelineVk == nullptr)
    {
        throw std::runtime_error("Failed to bind pipeline! Pipeline passed to CmdBindPipeline is not of type GraphicsPipelineStateVk!");
    }

    m_RenderCommands.push_back([pPipelineVk, offset, size, pValues](VkCommandBuffer commandBuffer) -> void {
        vkCmdPushConstants(commandBuffer, pPipelineVk->GetPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, offset, size, pValues);
    });
}

void RenderContextVk::CmdBindVertexBuffers(uint32_t bufferCount, IBuffer** ppBuffers, uint64_t* offsets)
{
    std::vector<VkBuffer> buffers(bufferCount);
    std::vector<uint64_t> offsetList(bufferCount);
    for (int i = 0; i < bufferCount; ++i)
    {
        BufferVk* buffer = dynamic_cast<BufferVk*>(ppBuffers[i]);
        if (buffer == nullptr)
        {
            throw std::runtime_error("Failed to bind Vertex Buffers! Couldn't cast IBuffer to VkBuffer!");
        }
        buffers[i] = buffer->GetBuffer();
        offsetList[i] = offsets[i];
    }

    m_RenderCommands.push_back([bufferCount, buffers, offsetList](VkCommandBuffer commandBuffer) -> void {
        vkCmdBindVertexBuffers(commandBuffer, 0, bufferCount, buffers.data(), offsetList.data());
    });
}

void RenderContextVk::CmdBindIndexBuffer(IBuffer *pBuffer, IndexType indexType, uint64_t offset)
{
    BufferVk* pBufferVk = dynamic_cast<BufferVk*>(pBuffer);
    if (pBufferVk == nullptr)
    {
        throw std::runtime_error("Failed to bind index buffer! pBuffer passed is not of type BufferVk!");
    }

    VkBuffer buffer = pBufferVk->GetBuffer();

    m_RenderCommands.push_back([buffer, offset, indexType](VkCommandBuffer commandBuffer) -> void {
        auto vkIndexType = VK_INDEX_TYPE_UINT16;
        if (indexType == INDEX_TYPE_UINT16)
            vkIndexType = VK_INDEX_TYPE_UINT16;
        else if (indexType == INDEX_TYPE_UINT32)
            vkIndexType = VK_INDEX_TYPE_UINT32;
        vkCmdBindIndexBuffer(commandBuffer, buffer, offset, vkIndexType);
    });
}

void RenderContextVk::CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstIndex)
{
    m_RenderCommands.push_back([indexCount, instanceCount, vertexOffset, firstIndex](VkCommandBuffer commandBuffer) -> void {
        vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, 0);
    });
}

void RenderContextVk::EndRecording()
{
    m_IsRecording = false;
    auto uint64_max = std::numeric_limits<uint64_t>::max();

    // Make sure the command buffers aren't being used by GPU while we record new commands
    const auto& commandBufferFences = m_pSwapChain->m_DrawFinishedFences;
    vkWaitForFences(m_pDevice->GetDevice(),
                    static_cast<uint32_t>(commandBufferFences.size()), commandBufferFences.data(),
                    VK_TRUE, uint64_max);

    auto extent = m_pSwapChain->GetExtent();

    // -- Dynamic Viewport & Scissor --
    // CreateGraphicsPipeline a viewport info struct
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // CreateGraphicsPipeline a scissor info struct (aka Cropping region)
    VkRect2D scissor = {};
    scissor.offset = {0,0};     // Offset to use region from
    scissor.extent = extent;  // Extent of the region starting from offset

    // -- Command Buffer & Render Pass --
    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_pSwapChain->GetRenderPass();
    renderPassBeginInfo.renderArea.offset = {0, 0}; // Start point of renderpass render region
    renderPassBeginInfo.renderArea.extent = extent; // Size of region to run render pass on

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]};
    clearValues[1].depthStencil.depth = 1.0;
    renderPassBeginInfo.pClearValues = clearValues.data(); // Clear values are 1:1 with the attachments of the whole render pass
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());    // clear value count should be exactly equal to the no. of attachments in render pass

    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();
    const auto& framebuffers = m_pSwapChain->GetFramebuffers();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        renderPassBeginInfo.framebuffer = framebuffers[i];

        auto result = vkBeginCommandBuffer(commandBuffers[i], &bufferBeginInfo);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to start recording the command buffer at index " + std::to_string(i));
        }

        // Begin Command Buffer
        {
            // Begin Render Pass
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            {
                // Set viewport & scissor dynamic state
                vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);
                vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

                // Run recording of stored commands
                for (const auto& command: m_RenderCommands)
                {
                    command(commandBuffers[i]);
                }
            }
            vkCmdEndRenderPass(commandBuffers[i]);
            // End Render Pass
        }
        // End Command Buffer

        result = vkEndCommandBuffer(commandBuffers[i]);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to stop recording the command buffer at index " + std::to_string(i));
        }
    }
}

#pragma endregion
