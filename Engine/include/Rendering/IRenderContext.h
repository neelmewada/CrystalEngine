#pragma once

#include "EngineDefs.h"

#include "IShaderResourceBinding.h"

#include <glm/glm.hpp>

namespace Vox
{

enum IndexType
{
    INDEX_TYPE_UINT16 = 0, INDEX_TYPE_UINT32 = 1
};

/*struct GlobalUniforms
{
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 view;
};*/

class IGraphicsPipelineState;
class IShaderResourceBinding;
class IBuffer;

class ENGINE_API IRenderContext
{
protected:
    IRenderContext() {}
public:
    virtual ~IRenderContext() {}

public: // Public API
    // - API
    //virtual void UpdateGlobalUniforms(const GlobalUniforms& globals) = 0;

    // - Setup
    virtual void SetClearColor(float clearColor[4]) = 0;

    // - Commands

    virtual void Begin() = 0;
    virtual void CmdBindGraphicsPipeline(IGraphicsPipelineState* pPipeline) = 0;
    virtual void CmdBindShaderResources(IShaderResourceBinding* pSRB) = 0;
    virtual void CmdBindVertexBuffers(Uint32 bufferCount, IBuffer** ppBuffers, Uint64* pOffsetInBuffers) = 0;
    virtual void CmdBindIndexBuffer(IBuffer* pBuffer, IndexType indexType, Uint64 offset) = 0;
    virtual void CmdDrawIndexed(Uint32 indexCount, Uint32 instanceCount, int32_t vertexOffset, Uint32 firstIndex, Uint32 firstInstanceIndex) = 0;
    virtual void End() = 0;
};

}
