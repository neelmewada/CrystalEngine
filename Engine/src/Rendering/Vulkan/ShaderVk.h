#pragma once

#include "Rendering/IShader.h"
#include "DeviceContextVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

class ShaderVariantVk
{
public:
    friend class ShaderVk;
    ShaderVariantVk(const ShaderVariantCreateInfo& createInfo, DeviceContextVk* device);
    ~ShaderVariantVk();

public:
    ShaderVariantVk(const ShaderVariantVk&) = delete;

public: // Public API
    const char* GetName() { return m_pName; }
    size_t GetDefineFlagsCount() { return m_DefineFlagsCount; }
    const char* GetDefineFlagAt(int index) { return m_pDefineFlags[index]; }
    VkShaderModule GetVertexModule() { return m_VertModule; }
    VkShaderModule GetFragmentModule() { return m_FragModule; }

private: // Internal API
    DeviceContextVk* m_Device = nullptr;

private: // Internal Members
    VkShaderModule m_VertModule = nullptr;
    VkShaderModule m_FragModule = nullptr;
    const char* m_pName = nullptr;
    size_t m_DefineFlagsCount = 0;
    const char** m_pDefineFlags = nullptr;
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
    ShaderVariantVk** m_pVariants; // Array of pointer to variants
};

}
