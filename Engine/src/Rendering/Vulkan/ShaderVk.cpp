
#include "EngineDefs.h"

#include "ShaderVk.h"

#include <spirv_cross/spirv_reflect.hpp>

#include <iostream>

using namespace Vox;

Uint32 GetShaderBaseTypeSize(ShaderBaseType baseType)
{
    switch (baseType)
    {
        case UInt: return 4;
        case Int: return 4;
        case Float: return 4;
        case Vec2: return 8;
        case Vec3: return 12;
        case Vec4: return 16;
        case Mat2: return 16;
        case Mat3: return 36;
        case Mat4: return 64;
        case UVec2: return 8;
        case UVec3: return 12;
        case UVec4: return 16;
        case IVec2: return 8;
        case IVec3: return 12;
        case IVec4: return 16;
    }
    return 0;
}

ShaderBaseType GetShaderBaseType(const spirv_cross::SPIRType& memType)
{
    if (memType.basetype == spirv_cross::SPIRType::Float)
    {
        auto vecSize = memType.vecsize;
        auto columns = memType.columns;
        if (columns == 1)
        {
            if (vecSize == 1) return ::Float;
            else if (vecSize == 2) return ::Vec2;
            else if (vecSize == 3) return ::Vec3;
            else if (vecSize == 4) return ::Vec4;
        }
        else if (columns == 2) return ::Mat2;
        else if (columns == 3) return ::Mat3;
        else if (columns == 4) return ::Mat4;
    }
    else if (memType.basetype == spirv_cross::SPIRType::UInt)
    {
        auto vecSize = memType.vecsize;
        if (vecSize == 1) return ::UInt;
        else if (vecSize == 2) return ::UVec2;
        else if (vecSize == 3) return ::UVec3;
        else if (vecSize == 4) return ::UVec4;
    }
    else if (memType.basetype == spirv_cross::SPIRType::Int)
    {
        auto vecSize = memType.vecsize;
        if (vecSize == 1) return ::Int;
        else if (vecSize == 2) return ::IVec2;
        else if (vecSize == 3) return ::IVec3;
        else if (vecSize == 4) return ::IVec4;
    }
    else if (memType.basetype == spirv_cross::SPIRType::SampledImage)
    {
        if (memType.image.dim == spv::Dim2D) return ::Sampler2D;
        else if (memType.image.dim == spv::Dim3D) return ::Sampler3D;
    }
    throw std::runtime_error("Failed to find a member type using SPIRV Reflection!");
}

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
    CompilerReflection fragReflection(createInfo.pFragmentBytes, createInfo.fragmentByteSize / 4);

    auto vertResources = vertReflection.get_shader_resources();
    auto fragResources = fragReflection.get_shader_resources();

    // Spir-V Cross Reflection

    for (const auto& uniformBuffer: vertResources.uniform_buffers)
    {
        auto descriptorSet = vertReflection.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
        auto binding = vertReflection.get_decoration(uniformBuffer.id, spv::DecorationBinding);
        auto& type = vertReflection.get_type(uniformBuffer.type_id);

        std::cout << "Uniform Buffer: " << uniformBuffer.name << " ; " << descriptorSet << " ; " << binding << std::endl;
        std::cout << "Uniform Name: " << vertReflection.get_block_fallback_name(uniformBuffer.id) << std::endl;

        ShaderResourceVariableDefinition variableDef = {};
        variableDef.set = descriptorSet;
        variableDef.binding = binding;
        variableDef.size = static_cast<Uint32>(vertReflection.get_declared_struct_size(type));
        variableDef.name = uniformBuffer.name;
        variableDef.type = ShaderResourceVariableDefinition::UniformBuffer;
        variableDef.members.clear();

        if (type.basetype == SPIRType::Struct)
        {
            variableDef.baseType = ShaderBaseType::Struct;

            for (int i = 0; i < type.member_types.size(); ++i)
            {
                ShaderBlockMember member = {};
                member.name = vertReflection.get_member_name(uniformBuffer.base_type_id, i);
                member.offset = vertReflection.type_struct_member_offset(type, i);
                member.size = static_cast<Uint32>(vertReflection.get_declared_struct_member_size(type, i));

                auto& memType = vertReflection.get_type(type.member_types[i]);
                member.baseType = GetShaderBaseType(memType);

                variableDef.members.push_back(member);
            }
        }
        m_ShaderVariableDefinitions.push_back(variableDef);
    }

    for (const auto& sampledImage: vertResources.sampled_images)
    {
        auto descriptorSet = vertReflection.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);
        auto binding = vertReflection.get_decoration(sampledImage.id, spv::DecorationBinding);
        auto& type = vertReflection.get_type(sampledImage.type_id);

        ShaderResourceVariableDefinition variableDef = {};
        variableDef.set = descriptorSet;
        variableDef.binding = binding;
        variableDef.size = 0;
        variableDef.name = sampledImage.name;
        variableDef.type = ShaderResourceVariableDefinition::SampledImage;
        variableDef.isArray = !type.array.empty();
        if (variableDef.isArray)
        {
            variableDef.members.resize(type.array[0]);
            auto elementBaseType = GetShaderBaseType(type);
            auto elementSize = GetShaderBaseTypeSize(elementBaseType);
            for (int i = 0; i < variableDef.members.size(); ++i)
            {
                ShaderBlockMember arrayElement = {};
                arrayElement.offset = i * elementSize;
                arrayElement.size = elementSize;
                arrayElement.baseType = elementBaseType;
                arrayElement.name = std::to_string(i);
                variableDef.members[i] = arrayElement;
            }
        }
        else
        {
            variableDef.members.clear();
        }

        m_ShaderVariableDefinitions.push_back(variableDef);
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

