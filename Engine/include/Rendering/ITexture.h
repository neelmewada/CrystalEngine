#pragma once

#include "EngineDefs.h"
#include "IDeviceObject.h"

#include "ITextureView.h"

namespace Vox
{

class ENGINE_API ITexture : public IDeviceObject
{
protected:
    ITexture() {}
public:
    virtual ~ITexture() {}

public: // Public API
    virtual ITextureView* GetDefaultView() = 0;

public:
    // - Getters
    Type GetDeviceObjectType() override { return DEVICE_OBJECT_TEXTURE; }

    // - Setters
};

}

