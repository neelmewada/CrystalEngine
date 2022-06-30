#pragma once

namespace Vox
{

class ISwapChain
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

