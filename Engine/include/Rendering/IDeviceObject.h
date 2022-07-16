#pragma once

#include "EngineDefs.h"

namespace Vox
{

class ENGINE_API IDeviceObject
{
protected:
    IDeviceObject() {}
public:
    virtual ~IDeviceObject() {}

public: // Public API
    enum Type {
        DEVICE_OBJECT_BUFFER, DEVICE_OBJECT_TEXTURE
    };

    virtual Type GetDeviceObjectType() = 0;
};

}
