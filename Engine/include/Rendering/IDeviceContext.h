#pragma once

#include "IGraphicsPipelineState.h"
#include "IShader.h"

#include <vector>

namespace Vox
{

class IDeviceContext;

struct GraphicsPipelineStateCreateInfo
{
    IDeviceContext* device;
    IShader* shader;
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
    int variantCount = 1;
    ShaderVariantCreateInfo* pShaderVariants = nullptr;
};

class IDeviceContext
{
protected:
    IDeviceContext() {}
public:
    virtual ~IDeviceContext() {}

public: // Public API
    virtual IGraphicsPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& createInfo) = 0;
    virtual IShader* CreateShader(const ShaderCreateInfo& createInfo) = 0;

};

}
