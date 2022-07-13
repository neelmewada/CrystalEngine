#pragma once

#include "EngineDefs.h"

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
class IBuffer;

enum ShaderStageFlags
{
    SHADER_STAGE_ALL = 0,
    SHADER_STAGE_VERTEX = 0x01,
    SHADER_STAGE_FRAGMENT = 0x02,
};

enum ShaderResourceType
{
    // Static resources can't be bound/unbound once they're bound initially. Ex: Global Uniforms, etc.
    SHADER_RESOURCE_TYPE_STATIC,
    // Mutable resources are bound on a per-material or per-object basis. In Vulkan, they're always storage buffers.
    SHADER_RESOURCE_TYPE_MUTABLE,
    // Dynamic resources can change their binding frequently and randomly.
    SHADER_RESOURCE_TYPE_DYNAMIC
};

struct ShaderResourceVariableDesc
{
    uint32_t binding;
    const char* pVariableName;
    ShaderResourceType resourceType;
    ShaderStageFlags stages;
};

// In Vulkan, a ShaderResourceObject represents a single descriptor set
struct ShaderResourceObjectCreateInfo
{
    uint32_t variableCount;
    const ShaderResourceVariableDesc* pVariables;
};

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
    virtual void CmdBindPipeline(IGraphicsPipelineState* pPipeline) = 0;
    virtual void CmdBindVertexBuffers(uint32_t bufferCount, IBuffer** ppBuffers, uint64_t* offsets) = 0;
    virtual void CmdBindIndexBuffer(IBuffer* pBuffer, IndexType indexType, uint64_t offset) = 0;
    virtual void CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstIndex, uint32_t firstInstanceIndex) = 0;
    virtual void End() = 0;
};

}
