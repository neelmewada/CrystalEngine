#pragma once

#include "EngineDefs.h"

namespace Vox
{

class ENGINE_API ISwapChain
{
protected:
    ISwapChain() {}
public:
    virtual ~ISwapChain() {}

public: // Public API
    virtual void Submit() = 0;
    virtual void Present() = 0;

};

}

