
#include "ShaderVk.h"

#include <iostream>

using namespace Vox;

ShaderVariantVk::ShaderVariantVk(const ShaderVariantCreateInfo& createInfo, DeviceContextVk *device)
{
    m_Device = device;
    m_pName = createInfo.name;

    m_DefineFlagsCount = createInfo.defineFlagsCount;
    m_pDefineFlags = createInfo.pDefineFlags;

    VkShaderModuleCreateInfo vertShaderInfo = {};
    vertShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertShaderInfo.codeSize = createInfo.vertexByteSize;
    vertShaderInfo.pCode = createInfo.pVertexBytes;

    auto result = vkCreateShaderModule(device->GetDevice(), &vertShaderInfo, nullptr, &m_VertModule);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vertex Shader");
    }

    VkShaderModuleCreateInfo fragShaderInfo = {};
    fragShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragShaderInfo.codeSize = createInfo.fragmentByteSize;
    fragShaderInfo.pCode = createInfo.pFragmentBytes;

    result = vkCreateShaderModule(device->GetDevice(), &fragShaderInfo, nullptr, &m_FragModule);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Fragment Shader");
    }
}

ShaderVariantVk::~ShaderVariantVk()
{
    vkDestroyShaderModule(m_Device->GetDevice(), m_FragModule, nullptr);
    vkDestroyShaderModule(m_Device->GetDevice(), m_VertModule, nullptr);
}

ShaderVk::ShaderVk(const ShaderCreateInfo &createInfo, DeviceContextVk* device) : IShader()
{
    m_Device = device;
    m_CurrentVariant = m_DefaultVariant = createInfo.defaultVariant;
    m_pVertEntryPoint = createInfo.pVertEntryPoint;
    m_pFragEntryPoint = createInfo.pFragEntryPoint;

    m_VariantsCount = createInfo.variantCount;
    m_pVariants.resize(m_VariantsCount);

    for (int i = 0; i < m_VariantsCount; ++i)
    {
        m_pVariants[i] = new ShaderVariantVk(createInfo.pVariants[i], device);
    }
}

ShaderVk::~ShaderVk()
{
    for (int i = 0; i < m_VariantsCount; ++i)
    {
        delete m_pVariants[i];
        m_pVariants[i] = nullptr;
    }

    m_VariantsCount = 0;
    m_pVariants.clear();
}

