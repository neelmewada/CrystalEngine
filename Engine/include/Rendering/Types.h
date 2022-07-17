#pragma once

#include "EngineDefs.h"

namespace Vox
{

enum VertexAttribFormat
{
    VERTEX_ATTRIB_FLOAT,
    VERTEX_ATTRIB_FLOAT2,
    VERTEX_ATTRIB_FLOAT3,
    VERTEX_ATTRIB_FLOAT4,
    VERTEX_ATTRIB_HALF,
    VERTEX_ATTRIB_HALF2,
    VERTEX_ATTRIB_HALF3,
    VERTEX_ATTRIB_HALF4
};

enum ImageFormat
{
    IMG_FORMAT_UNDEFINED = 0,
    IMG_FORMAT_R8_SRGB = 15,
    IMG_FORMAT_R8G8_SRGB = 22,
    IMG_FORMAT_R8G8B8_SRGB = 29,
    IMG_FORMAT_R8G8B8A8_SRGB = 43,
    IMG_FORMAT_B8G8R8A8_SRGB = 50,
    IMG_FORMAT_R32_SFLOAT = 100,
};

enum ShaderStageFlags : Uint32
{
    SHADER_STAGE_NONE = 0,
    SHADER_STAGE_VERTEX = 0x01,
    SHADER_STAGE_FRAGMENT = 0x10,
    SHADER_STAGE_ALL = 0x1F,
};

enum ShaderResourceVariableFlagBits : Uint32
{
    SHADER_RESOURCE_VARIABLE_NONE_BIT = 0x00,
    // Statically bound resources can't be bound/unbound once they're bound initially
    SHADER_RESOURCE_VARIABLE_STATIC_BINDING_BIT = 0x01,
    // Dynamically bound resources can be bound/unbound whenever needed
    SHADER_RESOURCE_VARIABLE_DYNAMIC_BINDING_BIT = 0x02,
    // Dynamic offset uniform or storage buffers
    SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT = 0x10,
};

typedef Uint32 ShaderResourceVariableFlags;

struct ShaderResourceVariableDesc
{
    const char* pVariableName;
    ShaderStageFlags stages;
    ShaderResourceVariableFlags flags;
};

enum SamplerAddressMode
{
    SAMPLER_ADDRESS_MODE_REPEAT = 0,
    SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
    SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
    SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
    SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 4,
};

enum SamplerFilterType
{
    SAMPLER_FILTER_TYPE_NEAREST = 0,
    SAMPLER_FILTER_TYPE_LINEAR = 1,
};

struct SamplerDesc
{
    ShaderStageFlags shaderStages;
    SamplerFilterType minFilter = SAMPLER_FILTER_TYPE_LINEAR,
            magFilter = SAMPLER_FILTER_TYPE_LINEAR,
            mipFilter = SAMPLER_FILTER_TYPE_LINEAR;
    SamplerAddressMode addressModeU, addressModeV, addressModeW;
    float anisotropy = 0;
};

}

