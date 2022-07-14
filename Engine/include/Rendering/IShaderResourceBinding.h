#pragma once

#include "EngineDefs.h"
#include "IDeviceObject.h"
#include "IShader.h"

namespace Vox
{

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
