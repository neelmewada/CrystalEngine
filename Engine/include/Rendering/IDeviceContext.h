#pragma once

#include "EngineDefs.h"
#include "Types.h"

#include "IRenderContext.h"
#include "IGraphicsPipelineState.h"
#include "IShader.h"
#include "ISwapChain.h"
#include "IBuffer.h"
#include "ITexture.h"

#include <vector>

namespace Vox
{

class IDeviceContext;
class IGraphicsPipelineState;
class ISwapChain;
class IRenderContext;
class ITexture;

struct GraphicsPipelineVertexAttributeDesc
{
    uint32_t binding;
    uint32_t location;
    uint32_t offset;
    VertexAttribFormat format; //usage: format = offsetof(Vertex, position);
};


struct ImmutableSamplerDesc
{
    const char* pTextureName;
    ShaderStageFlags shaderStages;
    float anisotropy = 0;
    SamplerFilterType minFilter = SAMPLER_FILTER_TYPE_LINEAR,
    magFilter = SAMPLER_FILTER_TYPE_LINEAR,
    mipFilter = SAMPLER_FILTER_TYPE_LINEAR;
    SamplerAddressMode addressModeU, addressModeV, addressModeW;
};

struct GraphicsPipelineStateCreateInfo
{
    const char* pName;
    IDeviceContext* pDevice;
    ISwapChain* pSwapChain;
    IRenderContext* pRenderContext;
    IShader* pShader;
    uint32_t vertexStructByteSize; // Number of bytes used per vertex
    uint32_t attributesCount;
    GraphicsPipelineVertexAttributeDesc* pAttributes = nullptr;
    uint32_t variableCount;
    const ShaderResourceVariableDesc* pResourceVariables;
    uint32_t immutableSamplersCount;
    ImmutableSamplerDesc* pImmutableSamplers = nullptr;
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
    const char* pVertEntryPoint;
    const char* pFragEntryPoint;
};

struct BufferCreateInfo
{
    const char* pName;
    BufferBindFlags bindFlags;
    DeviceAllocationType allocType;
    BufferTransferFlags transferFlags;
    BufferOptimizationFlags optimizationFlags;
    BufferUsageFlags usageFlags;
    uint64_t size;
    uint64_t structureByteStride;
};

struct BufferData
{
    uint64_t dataSize; // number of bytes
    uint64_t offsetInBuffer = 0; // offsetInBuffer in number of bytes
    const void* pData;
};

enum TextureLayout
{
    TEXTURE_LAYOUT_OPTIMAL = 0,
    TEXTURE_LAYOUT_LINEAR = 1
};

struct TextureCreateInfo
{
    const char* pName = nullptr;
    Uint32 width;
    Uint32 height;
    Uint32 mipLevels = 1;
    TextureType textureType = TEXTURE_TYPE_2D;
    TextureLayout textureLayout = TEXTURE_LAYOUT_OPTIMAL;
    ImageFormat format;
    DeviceAllocationType allocType = DEVICE_MEM_GPU_ONLY;
    void* pImageData = nullptr;
    size_t imageDataSize;
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
    virtual ITexture* CreateTexture(const TextureCreateInfo& createInfo) = 0;

public: // Getters & Setters
    // - Getters
    virtual Uint64 GetMinUniformBufferOffsetAlignment() = 0;
};

}
