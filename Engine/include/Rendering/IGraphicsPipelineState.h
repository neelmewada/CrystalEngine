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

};

}
