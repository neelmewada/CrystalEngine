#pragma once

#include "EngineDefs.h"
#include "Types.h"

#include "IGraphicsPipelineState.h"
#include "IShader.h"
#include "ISwapChain.h"
#include "IBuffer.h"

#include <vector>

namespace Vox
{

class IDeviceContext;
class IGraphicsPipelineState;
class ISwapChain;

struct GraphicsPipelineVertexAttributeDesc
{
    uint32_t binding;
    uint32_t location;
    uint32_t offset;
    VertexAttribFormat format; //usage: format = offsetof(Vertex, position);
};

struct GraphicsPipelineStateCreateInfo
{
    IDeviceContext* pDevice;
    ISwapChain* pSwapChain;
    IShader* pShader;
    uint32_t vertexStructByteSize; // Number of bytes used per vertex
    uint32_t attributesCount;
    GraphicsPipelineVertexAttributeDesc* pAttributes;
};

struct ShaderVariantCreateInfo
{
    const char* name;
    int defineFlagsCount;
    const char** pDefineFlags; // List of string elements (aka define flags used)
    size_t vertexByteSize = 0;
    const uint32_t* pVertexBytes = nullptr;
    size_t fragmentByteSize = 0;
    const uint32_t* pFragmentBytes = nullptr;
};

struct ShaderCreateInfo
{
    uint32_t defaultVariant = 0;
    size_t variantCount = 1;
    ShaderVariantCreateInfo* pVariants = nullptr;
};

struct BufferCreateInfo
{
    const char* pName;
    BufferBindFlags bindFlags;
    BufferUsageFlags usageFlags;
    uint64_t size;
};

struct BufferData
{
    uint64_t dataSize; // number of bytes
    void* pData;
};

class ENGINE_API IDeviceContext
{
protected:
    IDeviceContext() {}
public:
    virtual ~IDeviceContext() {}

public: // Public API
    virtual void WaitUntilIdle() = 0;
    virtual IGraphicsPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& createInfo) = 0;
    virtual IShader* CreateShader(const ShaderCreateInfo& createInfo) = 0;
    virtual IBuffer* CreateBuffer(const BufferCreateInfo& createInfo, BufferData& bufferData) = 0;
};

}
