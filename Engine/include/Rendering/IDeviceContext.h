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

enum ShaderStageFlags : Uint32
{
    SHADER_STAGE_NONE = 0,
    SHADER_STAGE_VERTEX = 0x01,
    SHADER_STAGE_FRAGMENT = 0x10,
    SHADER_STAGE_ALL = 0x1F,
};

enum ShaderResourceType
{
    // Static resources can't be bound/unbound once they're bound initially. Ex: Global Uniforms, etc.
    SHADER_RESOURCE_TYPE_STATIC,
    // Mutable resources are bound on a per-material or per-object basis. In Vulkan, they're always storage buffers.
    SHADER_RESOURCE_TYPE_MUTABLE,
    // Dynamic resources can change their binding frequently and randomly.
    SHADER_RESOURCE_TYPE_DYNAMIC
};

struct ShaderResourceVariableDesc
{
    const char* pVariableName;
    ShaderStageFlags stages;
    ShaderResourceType resourceType;
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
    BufferAllocationType allocType;
    BufferTransferFlags transferFlags;
    BufferOptimizationFlags optimizationFlags;
    uint64_t size;
};

struct BufferData
{
    uint64_t dataSize; // number of bytes
    uint64_t offset = 0; // offset in number of bytes
    uint64_t range = 0;
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
    int mipLevels = 1;
    TextureLayout textureLayout;
    ImageFormat format;
    BufferAllocationType allocType;
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
    virtual Uint64 GetUniformBufferOffsetAlignment() = 0;
};

}
