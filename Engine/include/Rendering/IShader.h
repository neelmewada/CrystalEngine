#pragma once

#include "EngineDefs.h"

namespace Vox
{

class ENGINE_API IShaderResourceObject
{
protected:
    IShaderResourceObject() {}
public:
    virtual ~IShaderResourceObject() {}

public: // Public API


};

class ENGINE_API IShader
{
protected:
    IShader() {}
public:
    virtual ~IShader() {}

public: // Public API


};

}
