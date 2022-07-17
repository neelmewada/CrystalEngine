#pragma once

#include "EngineDefs.h"
#include "IDeviceContext.h"
#include "IBuffer.h"
#include "IShaderResourceBinding.h"

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
    virtual IShaderResourceBinding* CreateResourceBinding(ResourceBindingFrequency bindingFrequency) = 0;

};

}
