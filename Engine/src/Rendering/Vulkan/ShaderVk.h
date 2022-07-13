#pragma once

#include "Rendering/IShader.h"
#include "DeviceContextVk.h"

#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Vox
{

enum ShaderBaseType
{
    Struct, UInt, Int, Sampler2D, Sampler3D, Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4,
    UVec2, UVec3, UVec4, IVec2, IVec3, IVec4
};

struct ShaderBlockMember
{
    Uint32 offset;
    Uint32 size;
    std::string name;
    ShaderBaseType baseType;
};

struct ShaderResourceVariableDefinition
{
public:
    enum ResourceType
    {
        UniformBuffer, StorageBuffer, SampledImage
    };

    Uint32 set;
    Uint32 binding;
    std::string name;
    Uint32 size;
    ShaderBaseType baseType;
    ResourceType type;
    bool isArray;
    std::vector<ShaderBlockMember> members;
};

class ShaderVariantVk
{
public:
    friend class ShaderVk;
    ShaderVariantVk(const ShaderVariantCreateInfo& createInfo, DeviceContextVk* device);
    ~ShaderVariantVk();

public:
    ShaderVariantVk(const ShaderVariantVk&) = delete;

public: // Public API
    // - Getters
    const char* GetName() { return m_pName; }
    size_t GetDefineFlagsCount() { return m_DefineFlagsCount; }
    const char* GetDefineFlagAt(int index) { return m_pDefineFlags[index]; }
    const std::vector<ShaderResourceVariableDefinition>& GetShaderVariableDefinitions() {
        return m_ShaderVariableDefinitions;
    }
    VkShaderModule GetVertexModule() { return m_VertModule; }
    VkShaderModule GetFragmentModule() { return m_FragModule; }


private: // Internal API

private: // Internal Members
    // - Vulkan
    VkShaderModule m_VertModule = nullptr;
    VkShaderModule m_FragModule = nullptr;

    // - Internal
    DeviceContextVk* m_Device = nullptr;
    const char* m_pName = nullptr;
    size_t m_DefineFlagsCount = 0;
    const char** m_pDefineFlags = nullptr;

    std::vector<ShaderResourceVariableDefinition> m_ShaderVariableDefinitions{0};
};

class ShaderVk : public IShader
{
public:
    ShaderVk(const ShaderCreateInfo& createInfo, DeviceContextVk* device);
    ~ShaderVk();

public: // Public API
    int GetVariantsCount() { return m_VariantsCount; }
    ShaderVariantVk* GetVariantAt(int index) { return m_pVariants[index]; }
    ShaderVariantVk* GetDefaultVariant() { return GetVariantAt(m_DefaultVariant); }
    ShaderVariantVk* GetCurrentVariant() { return GetVariantAt(m_CurrentVariant); }
    const char* GetVertEntryPoint() { return m_pVertEntryPoint; }
    const char* GetFragEntryPoint() { return m_pFragEntryPoint; }

private: // Internal API
    DeviceContextVk* m_Device;

private: // Internal Members
    const char* m_pVertEntryPoint = nullptr;
    const char* m_pFragEntryPoint = nullptr;
    int m_DefaultVariant = 0;
    int m_CurrentVariant = 0;
    size_t m_VariantsCount;
    std::vector<ShaderVariantVk*> m_pVariants; // Array of pointer to variants
};

}
