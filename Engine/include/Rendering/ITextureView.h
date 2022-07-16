#pragma once

#include "EngineDefs.h"

#include "IDeviceObject.h"

namespace Vox
{

enum TextureType
{
    TEXTURE_TYPE_2D = 1,
    TEXTURE_TYPE_3D = 2,
    TEXTURE_TYPE_CUBE = 3,
};

class ENGINE_API ITextureView : public IDeviceObject
{
protected:
    ITextureView() {}
public:
    virtual ~ITextureView() {}

public: // Public API
    Type GetDeviceObjectType() override { return DEVICE_OBJECT_TEXTURE_VIEW; }

};

}

