#pragma once

#include "EngineDefs.h"
#include "IDeviceObject.h"
#include "IShader.h"

namespace Vox
{

enum ResourceBindingFrequency : Uint32
{
    // Static resources once bound cannot be unbound/rebound. Useful for fixed global constants.
    // In vulkan, this uses the descriptor set 0.
    RESOURCE_BINDING_FREQUENCY_STATIC = 0,
    // Resources that change binding on a per-frame basis.
    // In vulkan, this uses the descriptor set 1.
    RESOURCE_BINDING_FREQUENCY_PER_FRAME = 1,
    // Resources that are bound/unbound multiple times throughout the frame. Ex: Material Buffers (Per-Pipeline), model matrices (Per-Object), etc.
    // In vulkan, this uses the descriptor sets 2 & 3.
    RESOURCE_BINDING_FREQUENCY_DYNAMIC = 2,
};

class ENGINE_API IShaderResourceVariable
{
protected:
    IShaderResourceVariable() {}
public:
    virtual ~IShaderResourceVariable() {}

public: // Public API
    virtual void Set(IDeviceObject* pObject) = 0;
    virtual void Set(IDeviceObject* pObject, Uint32 dynamicOffset) = 0;
    virtual void SetDynamicOffset(Uint32 offset) = 0;
    virtual Uint32 GetDynamicOffset() = 0;
};

class ENGINE_API IShaderResourceBinding
{
protected:
    IShaderResourceBinding() {}
public:
    virtual ~IShaderResourceBinding() {}

public: // Public API
    virtual IShaderResourceVariable* GetVariableByName(const char* pName) = 0;
};

}
