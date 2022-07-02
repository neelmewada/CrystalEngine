#pragma once

#include "EngineDefs.h"

#include <stdint.h>

namespace Vox
{

enum BufferUsageFlags
{
    USAGE_IMMUTABLE
};

enum BufferBindFlags
{
    BIND_VERTEX_BUFFER,
    BIND_INDEX_BUFFER,
    BIND_UNIFORM_BUFFER,
    BIND_STORAGE_BUFFER
};

class ENGINE_API IBuffer
{
protected:
    IBuffer() {}
public:
    virtual ~IBuffer() {}

public: // Public API
    virtual uint64_t GetBufferSize() = 0;
};

}
