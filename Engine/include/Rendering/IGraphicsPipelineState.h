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
    // TODO: Temp Code
    virtual void CreateUniformBuffer(uint64_t bufferSize, BufferData& bufferData) = 0;
    virtual void UpdateUniformBuffer(BufferData& data) = 0;
};

}
