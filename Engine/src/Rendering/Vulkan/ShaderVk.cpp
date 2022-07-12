
#include "EngineDefs.h"

#include "ShaderVk.h"

#include <spirv_cross/spirv_reflect.hpp>

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

    VK_ASSERT(vkCreateShaderModule(device->GetDevice(), &vertShaderInfo, nullptr, &m_VertModule),
              "Failed to create Vertex Shader");

    VkShaderModuleCreateInfo fragShaderInfo = {};
    fragShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragShaderInfo.codeSize = createInfo.fragmentByteSize;
    fragShaderInfo.pCode = createInfo.pFragmentBytes;

    VK_ASSERT(vkCreateShaderModule(device->GetDevice(), &fragShaderInfo, nullptr, &m_FragModule),
              "Failed to create Fragment Shader");

    using namespace spirv_cross;

    CompilerReflection vertReflection(createInfo.pVertexBytes, createInfo.vertexByteSize / 4);

    auto resources = vertReflection.get_shader_resources();

    for (const auto& uniformBuffer: resources.uniform_buffers)
    {
        auto descriptorSet = vertReflection.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
        auto binding = vertReflection.get_decoration(uniformBuffer.id, spv::DecorationBinding);
        std::cout << "Uniform Buffer: " << uniformBuffer.name << " ; " << descriptorSet << " ; " << binding << std::endl;

        auto& type = vertReflection.get_type(uniformBuffer.type_id);
        if (type.basetype == SPIRType::Struct)
        {
            std::cout << "Struct: " << vertReflection.get_declared_struct_size(type) << std::endl;
            int i = 0;
            for (const auto& member: type.member_types)
            {
                auto& memType = vertReflection.get_type(member);
                std::cout << "Member: " << vertReflection.get_declared_struct_member_size(type, i) << std::endl;
                i++;
            }
        }
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

