#pragma once

#include "EngineDefs.h"

#include <glm/glm.hpp>

namespace Vox
{

enum IndexType
{
    INDEX_TYPE_UINT16 = 0, INDEX_TYPE_UINT32 = 1
};

struct GlobalUniforms
{
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 view;
};

class IGraphicsPipelineState;
class IBuffer;

class ENGINE_API IRenderContext
{
protected:
    IRenderContext() {}
public:
    virtual ~IRenderContext() {}

public: // Public API
    // - API
    virtual void UpdateGlobalUniforms(const GlobalUniforms& globals) = 0;

    // - Setup
    virtual void SetClearColor(float clearColor[4]) = 0;

    // - Commands
    virtual void ReRecordCommands() = 0;
    virtual void ClearRecording() = 0;

    virtual void BeginRecording() = 0;
    virtual void CmdBindPipeline(IGraphicsPipelineState* pPipeline) = 0;
    virtual void CmdSetConstants(IGraphicsPipelineState* pPipeline, uint32_t offset, uint32_t size, const void* pValues) = 0;
    virtual void CmdBindVertexBuffers(uint32_t bufferCount, IBuffer** ppBuffers, uint64_t* offsets) = 0;
    virtual void CmdBindIndexBuffer(IBuffer* pBuffer, IndexType indexType, uint64_t offset) = 0;
    virtual void CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstIndex, uint32_t firstInstanceIndex) = 0;
    virtual void EndRecording() = 0;
};

}
