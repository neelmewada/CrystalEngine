#pragma once

#include "Rendering/IShader.h"
#include "DeviceContextVk.h"
#include "TypesVk.h"

#include <vulkan/vulkan.h>
#include <map>

namespace Vox
{

struct ShaderResourceVariableDefinition
{
public:
    Uint32 set;
    Uint32 binding;
    std::string name;
    Uint32 size;
    ShaderResourceVariableBaseType baseType;
    ShaderResourceVariableType type;
    bool isArray;
    std::vector<ShaderResourceVariableBlockMember> members;
    ShaderStageFlags shaderStages;
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
    const char* GetName() { return m_Name.c_str(); }
    size_t GetDefineFlagsCount() { return m_DefineFlags.size(); }
    const char* GetDefineFlagAt(int index) { return m_DefineFlags[index].c_str(); }
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
    std::string m_Name{};
    std::vector<std::string> m_DefineFlags{};

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

private: // Internal Members
    DeviceContextVk* m_Device;

    const char* m_pVertEntryPoint = nullptr;
    const char* m_pFragEntryPoint = nullptr;
    Uint32 m_DefaultVariant = 0;
    Uint32 m_CurrentVariant = 0;
    size_t m_VariantsCount;
    std::vector<ShaderVariantVk*> m_pVariants; // Array of pointer to variants
};

}
