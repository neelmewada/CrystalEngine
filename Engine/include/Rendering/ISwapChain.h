#pragma once

#include "EngineDefs.h"

#include <stdint.h>

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
    virtual void Resize() = 0;
    virtual void GetSize(uint32_t* w, uint32_t* h) = 0;

};

}

