#pragma once

#include "Rendering/EngineFactoryVk.h"
#include "Rendering/IRenderContext.h"

#include "SwapChainVk.h"

#include "EngineContextVk.h"
#include "DeviceContextVk.h"
#include "SwapChainVk.h"

#include <functional>

typedef std::function<void(VkCommandBuffer)> RenderCommand;

namespace Vox
{

class RenderContextVk : public IRenderContext
{
private:
    friend class EngineFactoryVk;
    RenderContextVk(RenderContextCreateInfoVk& renderContextInfo);
    ~RenderContextVk();

public:
    RenderContextVk(const RenderContextVk&) = delete;

public: // Public API
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

    void CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstIndex) override;
    void EndRecording() override;

private: // Internal API

private: // Internal Members
    // - References
    EngineContextVk* m_pEngine;
    DeviceContextVk* m_pDevice;
    SwapChainVk* m_pSwapChain;

    // - Data
    float m_ClearColor[4] = {0,0,0,0};
    std::vector<RenderCommand> m_RenderCommands;
    bool m_IsRecording;

private: // Vulkan Members

};

}
