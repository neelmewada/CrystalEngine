#pragma once

#include "EngineDefs.h"

#include <stdint.h>

namespace Vox
{

enum BufferUsageFlags
{
    BUFFER_USAGE_DEFAULT = 0x00
};

enum BufferBindFlags
{
    BIND_VERTEX_BUFFER = 0x01,
    BIND_INDEX_BUFFER = 0x02,
    BIND_UNIFORM_BUFFER = 0x04,
    BIND_STORAGE_BUFFER = 0x08
};

enum BufferAllocationType
{
    BUFFER_MEM_UNKNOWN = 0,
    BUFFER_MEM_GPU_ONLY = 1,
    BUFFER_MEM_CPU_ONLY = 2,
    BUFFER_MEM_CPU_TO_GPU = 3,
    BUFFER_MEM_GPU_TO_CPU = 4
};

struct BufferData;

class ENGINE_API IBuffer
{
protected:
    IBuffer() {}
public:
    virtual ~IBuffer() {}

public: // Public API
    // - Getters
    virtual uint64_t GetBufferSize() = 0;

    // - Buffer API
    virtual void SendBufferData(BufferData& bufferData) = 0;
};

}
