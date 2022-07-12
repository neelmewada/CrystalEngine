#pragma once

#include "EngineDefs.h"
#include "Rendering/EngineFactoryVk.h"
#include "BufferVk.h"

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <functional>
#include <vector>

namespace Vox
{

class EngineContextVk;
class DeviceContextVk;
class SwapChainVk;
class BufferVk;

typedef std::function<void(VkCommandBuffer)> RenderCommand;

class RenderContextVk : public IRenderContext
{
private:
    friend class EngineFactoryVk;
    RenderContextVk(RenderContextCreateInfoVk& renderContextInfo);
    ~RenderContextVk();

public:
    DELETE_COPY_CONSTRUCTORS(RenderContextVk)

public: // Public API
    // - Getters
    VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }
    VkDescriptorSetLayout GetGlobalDescriptorSetLayout() { return m_GlobalDescriptorSetLayout; }

    // - Setup
    void SetClearColor(float clearColor[4]) override;

    // - Commands
    void ReRecordCommands() override;
    void ClearRecording() override;

    void BeginRecording() override;
    void CmdBindPipeline(IGraphicsPipelineState* pPipeline) override;
    void CmdSetConstants(IGraphicsPipelineState *pPipeline, uint32_t offset, uint32_t size, const void *pValues) override;
    void CmdBindVertexBuffers(uint32_t bufferCount, IBuffer** ppBuffers, uint64_t* offsets) override;
    void CmdBindIndexBuffer(IBuffer *pBuffer, IndexType indexType, uint64_t offset) override;

    void CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstIndex, uint32_t firstInstanceIndex) override;
    void EndRecording() override;

private: // Internal API
    void CreateDescriptorPool();
    void CreateGlobalDescriptorSet();

private: // Internal Members
    // - Buffers
    std::vector<BufferVk*> m_GlobalUniformBuffer;

    // - References
    EngineContextVk* m_pEngine = nullptr;
    DeviceContextVk* m_pDevice = nullptr;
    SwapChainVk* m_pSwapChain = nullptr;
    std::vector<VkDescriptorSet> m_GlobalDescriptorSet;

    // - Data
    GlobalUniforms m_GlobalUniforms{};
    float m_ClearColor[4] = {0,0,0,1};
    std::vector<RenderCommand> m_RenderCommands;
    bool m_IsRecording = false;

private: // Vulkan Members
    VkDescriptorPool m_DescriptorPool = nullptr;
    VkDescriptorSetLayout m_GlobalDescriptorSetLayout = nullptr;

};

}
