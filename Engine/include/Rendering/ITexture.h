#pragma once

#include "EngineDefs.h"
#include "IDeviceObject.h"

namespace Vox
{

class ENGINE_API ITexture : public IDeviceObject
{
protected:
    ITexture() {}
public:
    virtual ~ITexture() {}

public: // Public API


public:
    // - Getters

    // - Setters
};

}

