#pragma once

#include "EngineDefs.h"
#include "IDeviceContext.h"
#include "IBuffer.h"

namespace Vox
{

struct BufferData;

class ENGINE_API IGraphicsPipelineState
{
protected:
    IGraphicsPipelineState() {}
public:
    virtual ~IGraphicsPipelineState() {}

public: // Public API
    // TODO: ALL Temp Code
    virtual void CreateUniformBuffer(Uint64 bufferSize, BufferData& initialData) = 0;
    virtual void UpdateUniformBuffer(BufferData& data) = 0;
    virtual void CreateDynamicUniformBuffer(Uint64 bufferSize, BufferData& initialData, uint64_t alignment) = 0;
    virtual void UpdateDynamicUniformBuffer(BufferData& data) = 0;
};

}
