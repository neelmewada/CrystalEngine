#include "CoreRPI.h"

namespace CE::RPI
{
	Material::Material(Shader* shader)
	{
		SetShader(shader);
	}

	Material::~Material()
	{
		if (shaderResourceGroup)
		{
            delete shaderResourceGroup;
		}
        shaderResourceGroup = nullptr;

		for (auto [name, buffer] : buffersByVariableName)
		{
			delete buffer;
		}
        buffersByVariableName.Clear();
	}

	void Material::SetShader(Shader* shader)
	{
		if (this->shader == shader)
			return;

        if (shaderResourceGroup)
        {
            delete shaderResourceGroup;
        }
        shaderResourceGroup = nullptr;

		this->shader = shader;
		this->shaderVariantIndex = shader->defaultVariantIndex;

        for (auto [name, buffer] : buffersByVariableName)
        {
            delete buffer;
        }
        buffersByVariableName.Clear();

		RecreateShaderResourceGroup();
        FlushProperties();
	}

	ShaderVariant* Material::GetCurrentShader() const
	{
		return shader->GetVariant(shaderVariantIndex);
	}

	void Material::SelectVariant(u32 variantIndex)
	{
		if (shaderVariantIndex != variantIndex)
		{
			shaderVariantIndex = variantIndex;
			RecreateShaderResourceGroup();
		}
	}

    void Material::SetPropertyValue(Name propertyName, const MaterialPropertyValue& value)
    {
        properties[propertyName] = value;
    }

    void Material::FlushProperties()
    {
        if (shaderResourceGroup == nullptr)
            RecreateShaderResourceGroup();
        if (shaderResourceGroup == nullptr)
            return;

        const auto& layout = shaderResourceGroup->GetLayout();
        for (const auto& variable : layout.variables)
        {
            if (variable.type == RHI::ShaderResourceType::ConstantBuffer ||
                variable.type == RHI::ShaderResourceType::StructuredBuffer ||
                variable.type == RHI::ShaderResourceType::RWStructuredBuffer)
            {
                if (variable.structMembers.IsEmpty())
                    continue;

                Array<u64>& offsets = memberOffsetsByVariableName[variable.name];

                if (offsets.IsEmpty())
                {
                    RHI::gDynamicRHI->GetShaderStructMemberOffsets(variable.structMembers, offsets);
                }

                u64 totalSize = 0;
                if (!offsets.IsEmpty())
                {
                    totalSize = offsets.Top() + RHI::gDynamicRHI->GetShaderStructMemberSize(variable.structMembers.Top());
                }
                
                RHI::Buffer* buffer = buffersByVariableName[variable.name];
                bool bufferRecreated = false;

                if (buffer != nullptr && buffer->GetBufferSize() < totalSize)
                {
                    delete buffer;
                    buffer = nullptr;
                }
                if (buffer == nullptr) // Create the buffer if it does not exist
                {
                    bufferRecreated = true;
                    RHI::BufferDescriptor bufferDesc{};
                    bufferDesc.name = variable.name;
                    bufferDesc.bufferSize = totalSize;
                        
                    switch (variable.type) {
                        case RHI::ShaderResourceType::ConstantBuffer:
                            bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
                            break;
                        case RHI::ShaderResourceType::StructuredBuffer:
                        case RHI::ShaderResourceType::RWStructuredBuffer:
                            bufferDesc.bindFlags = RHI::BufferBindFlags::StructuredBuffer;
                            break;
                        default:
                            continue;
                    }
                        
                    bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
                        
                    buffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);
                }

                buffersByVariableName[variable.name] = buffer;

                void* data = nullptr;
                if (buffer->Map(0, totalSize, &data))
                {
                    for (int i = 0; i < offsets.GetSize(); i++)
                    {
                        u8* ptr = (u8*)data + offsets[i];
                        Name propertyName = variable.structMembers[i].name;
                        if (properties.KeyExists(propertyName))
                        {
                            const MaterialPropertyValue& value = properties[propertyName];
                            switch (variable.structMembers[i].dataType) {
                                case RHI::ShaderStructMemberType::UInt:
                                    if (value.IsOfType<u32>())
                                        *((u32*)ptr) = value.GetValue<u32>();
                                    else if (value.IsOfType<s32>())
                                        *((u32*)ptr) = (u32)value.GetValue<s32>();
                                    break;
                                case RHI::ShaderStructMemberType::Int:
                                    if (value.IsOfType<s32>())
                                        *((s32*)ptr) = value.GetValue<s32>();
                                    else if (value.IsOfType<u32>())
                                        *((s32*)ptr) = (s32)value.GetValue<u32>();
                                    break;
                                case RHI::ShaderStructMemberType::Float:
                                    if (value.IsOfType<f32>())
                                        *((f32*)ptr) = value.GetValue<f32>();
                                    break;
                                case RHI::ShaderStructMemberType::Float2:
                                    if (value.IsOfType<Vec2>())
                                        *((Vec2*)ptr) = value.GetValue<Vec2>();
                                    break;
                                case RHI::ShaderStructMemberType::Float3:
                                    if (value.IsOfType<Vec3>())
                                        *((Vec3*)ptr) = value.GetValue<Vec3>();
                                    break;
                                case RHI::ShaderStructMemberType::Float4:
                                    if (value.IsOfType<Vec4>())
                                        *((Vec4*)ptr) = value.GetValue<Vec4>();
                                    else if (value.IsOfType<Color>())
                                    {
                                        Color color = value.GetValue<Color>();
                                        *((Vec4*)ptr) = Vec4(color.r, color.g, color.b, color.a);
                                    }
                                    break;
                                case RHI::ShaderStructMemberType::Float4x4:
                                    if (value.IsOfType<Matrix4x4>())
                                        *((Matrix4x4*)ptr) = value.GetValue<Matrix4x4>();
                                    break;
                            };
                                
                        }
                    }
                    buffer->Unmap();
                }

                if (bufferRecreated)
                {
                    shaderResourceGroup->Bind(variable.name, buffer);
                }
            }
            else if (variable.type == RHI::ShaderResourceType::Texture1D || 
                variable.type == RHI::ShaderResourceType::Texture2D || 
                variable.type == RHI::ShaderResourceType::TextureCube ||
                variable.type == RHI::ShaderResourceType::Texture3D ||
                variable.type == RHI::ShaderResourceType::RWTexture2D ||
                variable.type == RHI::ShaderResourceType::RWTexture3D)
            {
                if (properties.KeyExists(variable.name))
                {
                    const MaterialPropertyValue& value = properties[variable.name];
                    if (value.GetValueType() == MaterialPropertyDataType::Texture)
                    {
                        RHI::Texture* texture = value.GetValue<RHI::Texture*>();
                        if (texture != nullptr)
                        {
                            shaderResourceGroup->Bind(variable.name, texture);
                        }
                    }
                }
            }
            else if (variable.type == RHI::ShaderResourceType::SamplerState)
            {
                if (properties.KeyExists(variable.name))
                {
                    const MaterialPropertyValue& value = properties[variable.name];
                    if (value.GetValueType() == MaterialPropertyDataType::Sampler)
                    {
                        RHI::Sampler* sampler = value.GetValue<RHI::Sampler*>();
                        if (sampler != nullptr)
                        {
                            shaderResourceGroup->Bind(variable.name, sampler);
                        }
                    }
                }
            }
        }

        shaderResourceGroup->FlushBindings();
    }

	void Material::Compile()
	{
		if (shaderResourceGroup)
		{
            FlushProperties();
            
			shaderResourceGroup->Compile();
		}
	}

	void Material::RecreateShaderResourceGroup()
	{
		if (shaderResourceGroup)
		{
            delete shaderResourceGroup;
			shaderResourceGroup = nullptr;
		}

        memberOffsetsByVariableName.Clear();

		ShaderVariant* currentShader = GetCurrentShader();
		if (!currentShader || !currentShader->GetPipeline())
			return;

		const auto& graphicsDesc = currentShader->GetPipeline()->GetGraphicsDescriptor();

		for (const auto& srgLayout : graphicsDesc.srgLayouts)
		{
			if (srgLayout.srgType == RHI::SRGType::PerMaterial)
			{
				shaderResourceGroup = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
				
				break;
			}
		}
	}
    
} // namespace CE::RPI
