
#include "EngineDefs.h"

#include "RenderContextVk.h"

#include "SwapChainVk.h"
#include "GraphicsPipelineStateVk.h"

#include "EngineContextVk.h"
#include "DeviceContextVk.h"

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <functional>
#include <array>
#include <string>

using namespace Vox;

RenderContextVk::RenderContextVk(RenderContextCreateInfoVk &renderContextInfo)
{
    m_pEngine = dynamic_cast<EngineContextVk*>(renderContextInfo.engineContext);
    m_pDevice = dynamic_cast<DeviceContextVk*>(renderContextInfo.deviceContext);
    m_pSwapChain = dynamic_cast<SwapChainVk*>(renderContextInfo.swapChain);
    m_pSwapChain->SetRenderContext(this);

    CreateDescriptorPool();

    std::cout << "Created RenderContextVk" << std::endl;
}

RenderContextVk::~RenderContextVk()
{
    // Descriptor Pool
    vkDestroyDescriptorPool(m_pDevice->GetDevice(), m_DescriptorPool, nullptr);

    std::cout << "Destroyed RenderContextVk" << std::endl;
}

#pragma region Internal API

void RenderContextVk::CreateDescriptorPool()
{
    VkDescriptorPoolSize sizes[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 5},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = _countof(sizes);
    poolInfo.pPoolSizes = sizes;
    poolInfo.maxSets = 32;

    auto result = vkCreateDescriptorPool(m_pDevice->GetDevice(), &poolInfo, nullptr, &m_DescriptorPool);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Descriptor Pool!");
    }
}

#pragma endregion

#pragma region Public API

void RenderContextVk::SetClearColor(float clearColor[4])
{
    for (int i = 0; i < 4; ++i)
        m_ClearColor[i] = clearColor[i];
}

void RenderContextVk::Begin()
{
    m_IsRecording = true;
    m_CurrentPipeline = nullptr;

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
    scissor.extent = extent;  // Extent of the region starting from offsetInBuffer

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

        // Begin Command Buffer
        VK_ASSERT(vkBeginCommandBuffer(commandBuffers[i], &bufferBeginInfo),
                  "Failed to start recording the command buffer at index " + std::to_string(i));

        // Begin Render Pass
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set viewport & scissor dynamic state
        vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);
    }
}

void RenderContextVk::CmdBindGraphicsPipeline(IGraphicsPipelineState *pPipeline)
{
    GraphicsPipelineStateVk* pPipelineVk = dynamic_cast<GraphicsPipelineStateVk*>(pPipeline);

    VOX_ASSERT(pPipelineVk != nullptr, "Failed to bind pipeline! Pipeline passed to CmdBindGraphicsPipeline is not of type GraphicsPipelineStateVk!");

    m_CurrentPipeline = pPipelineVk;

    auto curFrameIndex = m_pSwapChain->GetCurrentFrameIndex();

    //auto globalDescriptorSet = m_GlobalDescriptorSet[curFrameIndex];

    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        //pPipelineVk->CmdBindDescriptorSets(commandBuffers[i]);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pPipelineVk->GetPipeline());
    }
}

void RenderContextVk::CmdBindShaderResources(IShaderResourceBinding* pSRB)
{
    ShaderResourceBindingVk* pShaderResourceBinding = dynamic_cast<ShaderResourceBindingVk*>(pSRB);

    VOX_ASSERT(pShaderResourceBinding != nullptr, "Failed to bind shader resources! Shader Resource Binding passed to CmdBindShaderResources is not of type ShaderResourceBindingVk!");

    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();
    auto curFrameIndex = m_pSwapChain->GetCurrentFrameIndex();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        pShaderResourceBinding->CmdBindDescriptorSets(commandBuffers[i], curFrameIndex);
    }
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

    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        vkCmdBindVertexBuffers(commandBuffers[i], 0, bufferCount, buffers.data(), offsetList.data());
    }
}

void RenderContextVk::CmdBindIndexBuffer(IBuffer *pBuffer, IndexType indexType, uint64_t offset)
{
    BufferVk* pBufferVk = dynamic_cast<BufferVk*>(pBuffer);
    if (pBufferVk == nullptr)
    {
        throw std::runtime_error("Failed to bind index buffer! pBuffer passed is not of type BufferVk!");
    }

    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        VkIndexType indexTypeVk = static_cast<VkIndexType>(indexType);
        vkCmdBindIndexBuffer(commandBuffers[i], pBufferVk->GetBuffer(), offset, indexTypeVk);
    }
}

void RenderContextVk::CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstIndex, uint32_t firstInstanceIndex)
{
    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        vkCmdDrawIndexed(commandBuffers[i], indexCount, instanceCount, firstIndex, vertexOffset, firstInstanceIndex);
    }
}

void RenderContextVk::End()
{
    m_IsRecording = false;
    m_CurrentPipeline = nullptr;

    const auto& commandBuffers = m_pSwapChain->GetCommandBuffers();

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        vkCmdEndRenderPass(commandBuffers[i]);
        vkEndCommandBuffer(commandBuffers[i]);
    }
}

#pragma endregion
