#pragma once

#include "EngineDefs.h"
#include "IDeviceObject.h"
#include "IShader.h"

namespace Vox
{

enum ResourceBindingFrequency : Uint32
{
    // Static resources once bound cannot be unbound/rebound. Useful for fixed global constants.
    RESOURCE_BINDING_FREQUENCY_STATIC = 0,
    // Resources that change binding on a per-frame basis.
    RESOURCE_BINDING_FREQUENCY_PER_FRAME = 1,
    // Resources that are bound/unbound multiple times throughout the frame. Ex: Material Buffers (Per-Pipeline), model matrices (Per-Object), etc.
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
