#pragma once

#include "EngineDefs.h"
#include "IDeviceObject.h"

#include <stdint.h>

namespace Vox
{

enum BufferBindFlags
{
    BIND_VERTEX_BUFFER = 0x01,
    BIND_INDEX_BUFFER = 0x02,
    BIND_UNIFORM_BUFFER = 0x04,
    BIND_STORAGE_BUFFER = 0x08,
    // Internal Usage Only: Use this buffer as a temporary staging buffer
    BIND_STAGING_BUFFER = 0x10
};

enum BufferAllocationType
{
    BUFFER_MEM_UNKNOWN = 0,
    BUFFER_MEM_GPU_ONLY = 1,
    BUFFER_MEM_CPU_ONLY = 2,
    BUFFER_MEM_CPU_TO_GPU = 3,
    BUFFER_MEM_GPU_TO_CPU = 4
};

enum BufferUsageFlags
{
    BUFFER_USAGE_DEFAULT = 0,
    BUFFER_USAGE_DYNAMIC_OFFSET_BIT = 0x01,
    // TODO: Not implemented. Does nothing for now!
    BUFFER_USAGE_DYNAMIC_SIZE_BIT = 0x02,
};

enum BufferTransferFlags
{
    BUFFER_TRANSFER_NONE = 0,
    BUFFER_TRANSFER_SRC_BIT = 0x01,
    BUFFER_TRANSFER_DST_BIT = 0x02,
};

enum BufferOptimizationFlags
{
    BUFFER_OPTIMIZE_NONE = 0,
    // The buffer data is never updated.
    BUFFER_OPTIMIZE_UPDATE_NEVER_BIT = 0x01,
    // The buffer data is seldom updated. Ex: Updated on an occasional (event) basis.
    BUFFER_OPTIMIZE_UPDATE_SELDOM_BIT = 0x02,
    // The buffer data is updated regularly. Ex: Updated every frame, every 1 second, etc.
    BUFFER_OPTIMIZE_UPDATE_REGULAR_BIT = 0x03,
    BUFFER_OPTIMIZE_TEMPORARY_BIT = 0x04
};

struct BufferData;

class IDeviceObject;

class ENGINE_API IBuffer : public IDeviceObject
{
protected:
    IBuffer() {}
public:
    virtual ~IBuffer() {}

public: // Public API
    // - Getters
    virtual uint64_t GetBufferSize() = 0;

    // - Buffer API

    // Don't use this if buffer alloc type is GPU-ONLY
    virtual void SetBufferData(BufferData& bufferData) = 0;
    // Use this if buffer alloc type is GPU-ONLY
    virtual void UploadBufferDataToGPU(BufferData& bufferData) = 0;
};

}
