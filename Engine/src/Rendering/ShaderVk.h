#pragma once

#include "Rendering/IShader.h"
#include "DeviceContextVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

class ShaderVariantVk
{
public:
    ShaderVariantVk(const ShaderVariantCreateInfo& createInfo, DeviceContextVk* device);
    ~ShaderVariantVk();

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


private: // Internal API
    DeviceContextVk* m_Device;

private: // Internal Members
    int m_VariantsCount;
    ShaderVariantVk** m_pVariants; // Array of pointer to variants
};

}
