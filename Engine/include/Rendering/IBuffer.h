#pragma once

#include "EngineDefs.h"

#include <stdint.h>

namespace Vox
{

enum BufferUsageFlags
{
    BUFFER_USAGE_IMMUTABLE
};

enum BufferBindFlags
{
    BIND_VERTEX_BUFFER = 0x01,
    BIND_INDEX_BUFFER = 0x02,
    BIND_UNIFORM_BUFFER = 0x04,
    BIND_STORAGE_BUFFER = 0x08
};

enum BufferAllocationFlags
{
    BUFFER_MEM_UNKNOWN = 0,
    BUFFER_MEM_GPU_ONLY = 1,
    BUFFER_MEM_CPU_ONLY = 2,
    BUFFER_MEM_CPU_TO_GPU = 3,
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
