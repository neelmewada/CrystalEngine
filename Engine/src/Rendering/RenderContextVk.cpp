
#include "RenderContextVk.h"

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <functional>

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
    // TODO: Temp Code
    BeginRecording();
}

void RenderContextVk::BeginRecording()
{
    // TODO: Temp Function Code
    // Make sure the command buffers aren't being used by GPU while we record commands
    vkQueueWaitIdle(m_pDevice->GetGraphicsQueue());

    auto extent = m_pSwapChain->GetExtent();

    // -- Dynamic Viewport & Scissor --
    // Create a viewport info struct
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Create a scissor info struct (aka Cropping region)
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
    VkClearValue clearValues[] = {
        {m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]}
    };
    // TODO: Add Depth Attachment clear values later
    renderPassBeginInfo.pClearValues = clearValues; // Clear values are 1:1 with the attachments of the whole render pass
    renderPassBeginInfo.clearValueCount = 1;    // clear value count should be exactly equal to the no. of attachments in render pass

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

                // Do nothing for now
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

void RenderContextVk::EndRecording()
{

}

#pragma endregion
