#include "CoreRPI.h"

namespace CE::RPI
{

	Material::Material(ShaderCollection* shaderCollection)
		: shaderCollection(shaderCollection)
	{

	}

	Material::~Material()
	{
		delete shaderResourceGroup; shaderResourceGroup = nullptr;

        for (int i = 0; i < buffersByVariableNamePerImage.GetSize(); i++)
        {
            for (auto [name, buffer] : buffersByVariableNamePerImage[i])
            {
                delete buffer;
            }
            buffersByVariableNamePerImage[i].Clear();
        }
	}

	ShaderCollection* Material::GetShaderCollection() const
	{
        return shaderCollection;
	}

	void Material::SetShaderCollection(ShaderCollection* shaderCollection)
	{
        this->shaderCollection = shaderCollection;

        delete shaderResourceGroup; shaderResourceGroup = nullptr;

        opaqueShader = GetOpaqueShader();
        if (!opaqueShader)
            return;

        this->shaderVariantIndex = opaqueShader->defaultVariantIndex;

        for (int i = 0; i < buffersByVariableNamePerImage.GetSize(); i++)
        {
            for (auto [name, buffer] : buffersByVariableNamePerImage[i])
            {
                delete buffer;
            }
            buffersByVariableNamePerImage[i].Clear();
        }

        RecreateShaderResourceGroup();
        FlushProperties();
	}

	Shader* Material::GetOpaqueShader() const
	{
        if (shaderCollection == nullptr)
            return nullptr;

        DrawListTag opaqueTag = RPISystem::Get().GetDrawListTagRegistry()->FindTag("opaque");
        if (!opaqueTag.IsValid())
            return nullptr;

        for (const auto& item : *shaderCollection)
        {
            if (item.shader->drawListTag == opaqueTag || item.drawListOverride == opaqueTag)
                return item.shader;
        }

        return nullptr;
	}

	ShaderVariant* Material::GetCurrentOpaqueShader()
	{
        if (!opaqueShader)
            opaqueShader = GetOpaqueShader();
        if (!opaqueShader)
            return nullptr;
		return opaqueShader->GetVariant(shaderVariantIndex);
	}

	void Material::SelectVariant(u32 variantIndex)
	{
		if (shaderVariantIndex != variantIndex)
		{
			shaderVariantIndex = variantIndex;
			RecreateShaderResourceGroup();
		}
	}

	bool Material::PropertyExists(Name name) const
	{
        return properties.KeyExists(name) && properties.Get(name).GetArrayCount() > 0 && properties.Get(name).GetValueType() != MaterialPropertyDataType::None;
	}

    void Material::ClearAllValues()
    {
        for (int i = 0; i < updateRequired.GetSize(); i++)
        {
            updateRequired[i] = true;
        }

        properties.Clear();
    }

    void Material::SetCustomShaderItem(int customItemIndex)
    {
        this->customShaderItem = customItemIndex;
    }

    RPI::Shader* Material::GetCustomShaderItem()
    {
        if (!shaderCollection)
            return nullptr;
		if (customShaderItem < 0 || customShaderItem >= shaderCollection->GetSize())
            return nullptr;

        return shaderCollection->At(customShaderItem).shader;
    }

    void Material::SetPropertyValue(Name propertyName, const MaterialPropertyValue& value)
    {
        if (properties[propertyName].Exists(value))
            return;

        for (int i = 0; i < updateRequired.GetSize(); i++)
        {
            updateRequired[i] = true;
        }

        properties[propertyName].Clear();
        properties[propertyName].Add(value);
    }

    void Material::InsertPropertyValue(Name propertyName, const MaterialPropertyValue& value, int index)
    {
        if (index < 0 || index > properties[propertyName].GetArrayCount())
            index = properties[propertyName].GetArrayCount();

        properties[propertyName].Insert(index, value);
    }

    void Material::RemovePropertyValue(Name propertyName, int index)
    {
        if (index < 0 || index >= properties[propertyName].GetArrayCount())
            return;

        properties[propertyName].RemoveAt(index);
    }

    void Material::ClearPropertyValue(Name propertyName)
    {
        properties[propertyName].Clear();
    }

    const MaterialPropertyValue& Material::GetPropertyValue(Name propertyName)
    {
        static const MaterialPropertyValue defaultValue{};

        if (PropertyExists(propertyName))
            return properties[propertyName].GetValue(0);

        if (baseMaterial != nullptr)
        {
            return baseMaterial->GetPropertyValue(propertyName);
        }

        return defaultValue;
    }

    const MaterialPropertyValue& Material::GetPropertyValue(Name propertyName, int index)
    {
        static const MaterialPropertyValue defaultValue{};

        if (PropertyExists(propertyName) && index >= 0 && index < properties[propertyName].GetArrayCount())
        {
	        return properties[propertyName].GetValue(index);
        }

        if (baseMaterial != nullptr)
        {
            return baseMaterial->GetPropertyValue(propertyName, index);
        }

        return defaultValue;
    }

    void Material::CopyPropertiesFrom(RPI::Material* other)
    {
        for (const auto& [name, value] : other->properties)
        {
            this->properties[name] = value;
        }
    }

    void Material::FlushProperties(u32 imageIndex)
    {
        if (shaderResourceGroup == nullptr)
            RecreateShaderResourceGroup();
        if (shaderResourceGroup == nullptr)
            return;

        if (!updateRequired[imageIndex])
            return;

        updateRequired[imageIndex] = false;

        const auto& layout = shaderResourceGroup->GetLayout();
        for (const auto& variable : layout.variables)
        {
            if (variable.type == RHI::ShaderResourceType::ConstantBuffer)
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
                
                RHI::Buffer* buffer = buffersByVariableNamePerImage[imageIndex][variable.name];
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
                    bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;

                    bufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;

                    buffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);
                }

                buffersByVariableNamePerImage[imageIndex][variable.name] = buffer;

                void* data = nullptr;
                if (buffer->Map(0, totalSize, &data))
                {
                    for (int i = 0; i < offsets.GetSize(); i++)
                    {
                        u8* ptr = (u8*)data + offsets[i];
                        Name propertyName = variable.structMembers[i].name;
                        if (properties.KeyExists(propertyName) && properties[propertyName].GetArrayCount() > 0)
                        {
                            const MaterialPropertyValue& value = properties[propertyName].GetValue(0);
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
                                    else if (value.IsOfType<u32>())
                                        *((f32*)ptr) = (f32)value.GetValue<u32>();
                                    else if (value.IsOfType<s32>())
                                        *((f32*)ptr) = (f32)value.GetValue<s32>();
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
                                        *((Vec4*)ptr) = value.GetValue<Color>().ToVec4();
                                    break;
                                case RHI::ShaderStructMemberType::Float4x4:
                                    if (value.IsOfType<Matrix4x4>())
                                        *((Matrix4x4*)ptr) = value.GetValue<Matrix4x4>();
                                    break;
                            };
                        }
                        else if (baseMaterial != nullptr)
                        {
                            const MaterialPropertyValue& value = baseMaterial->GetPropertyValue(propertyName);
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
                                else if (value.IsOfType<u32>())
                                    *((f32*)ptr) = (f32)value.GetValue<u32>();
                                else if (value.IsOfType<s32>())
                                    *((f32*)ptr) = (f32)value.GetValue<s32>();
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
                                    *((Vec4*)ptr) = value.GetValue<Color>().ToVec4();
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
                    shaderResourceGroup->Bind(imageIndex, variable.name, buffer);
                }
            }
            else if (variable.type == RHI::ShaderResourceType::StructuredBuffer ||
                variable.type == RHI::ShaderResourceType::RWStructuredBuffer)
            {
                if (properties.KeyExists(variable.name) && properties[variable.name].GetArrayCount() > 0)
                {
                    const MaterialPropertyValue& value = properties[variable.name].GetValue(0);
                    if (value.GetValueType() == MaterialPropertyDataType::Buffer)
                    {
                        shaderResourceGroup->Bind(imageIndex, variable.name, value.GetValue<RHI::BufferView>());
                    }
                }
                else if (baseMaterial != nullptr)
                {
                    const MaterialPropertyValue& value = baseMaterial->GetPropertyValue(variable.name);
                    if (value.GetValueType() == MaterialPropertyDataType::Buffer)
                    {
                        shaderResourceGroup->Bind(imageIndex, variable.name, value.GetValue<RHI::BufferView>());
                    }
                }
            }
            else if (variable.type == RHI::ShaderResourceType::Texture1D || 
                variable.type == RHI::ShaderResourceType::Texture2D || 
                variable.type == RHI::ShaderResourceType::TextureCube ||
                variable.type == RHI::ShaderResourceType::Texture3D ||
                variable.type == RHI::ShaderResourceType::RWTexture2D ||
                variable.type == RHI::ShaderResourceType::RWTexture3D)
            {
                if (properties.KeyExists(variable.name) && properties[variable.name].GetArrayCount() > 0)
                {
                    const MaterialPropertyValue& value = properties[variable.name].GetValue(0);
                    if (value.GetValueType() == MaterialPropertyDataType::Texture)
                    {
                        RPI::Texture* texture = value.GetValue<RPI::Texture*>();
                        if (texture != nullptr)
                        {
                            if (texture->GetRhiTextureView() != nullptr)
                            {
                                shaderResourceGroup->Bind(imageIndex, variable.name, texture->GetRhiTextureView());
                            }
                            else if (texture->GetRhiTexture() != nullptr)
                            {
                                shaderResourceGroup->Bind(imageIndex, variable.name, texture->GetRhiTexture());
                            }
                            RHI::Sampler* sampler = texture->GetSamplerState();
                            Name samplerPropertyName = variable.name.GetString() + "Sampler";
                            if (sampler != nullptr && (!properties.KeyExists(samplerPropertyName) || !properties[samplerPropertyName].IsOfType<RHI::Sampler*>()))
                            {
                                shaderResourceGroup->Bind(imageIndex, samplerPropertyName, sampler);
                            }
                        }
                    }
                    else if (value.GetValueType() == MaterialPropertyDataType::TextureView)
                    {
                        RHI::TextureView* textureView = value.GetValue<RHI::TextureView*>();
                        if (textureView != nullptr)
                        {
                            shaderResourceGroup->Bind(imageIndex, variable.name, textureView);
                        }
                    }
                }
                else if (baseMaterial != nullptr)
                {
                    const MaterialPropertyValue& value = baseMaterial->GetPropertyValue(variable.name);
                    if (value.GetValueType() == MaterialPropertyDataType::Texture)
                    {
                        RPI::Texture* texture = value.GetValue<RPI::Texture*>();
                        if (texture != nullptr)
                        {
                            if (texture->GetRhiTextureView() != nullptr)
                            {
                                shaderResourceGroup->Bind(imageIndex, variable.name, texture->GetRhiTextureView());
                            }
                            else if (texture->GetRhiTexture() != nullptr)
                            {
                                shaderResourceGroup->Bind(imageIndex, variable.name, texture->GetRhiTexture());
                            }
                            RHI::Sampler* sampler = texture->GetSamplerState();
                            if (sampler != nullptr)
                            {
                                shaderResourceGroup->Bind(imageIndex, variable.name.GetString() + "Sampler", sampler);
                            }
                        }
                    }
                    else if (value.GetValueType() == MaterialPropertyDataType::TextureView)
                    {
                        RHI::TextureView* textureView = value.GetValue<RHI::TextureView*>();
                        if (textureView != nullptr)
                        {
                            shaderResourceGroup->Bind(imageIndex, variable.name, textureView);
                        }
                    }
                }
            }
            else if (variable.type == RHI::ShaderResourceType::SamplerState)
            {
                if (properties.KeyExists(variable.name))
                {
                    const MaterialPropertyValue& value = properties[variable.name].GetValue(0);
                    if (value.GetValueType() == MaterialPropertyDataType::Sampler)
                    {
                        RHI::Sampler* sampler = value.GetValue<RHI::Sampler*>();
                        if (sampler != nullptr)
                        {
                            shaderResourceGroup->Bind(imageIndex, variable.name, sampler);
                        }
                    }
                }
                else if (baseMaterial != nullptr)
                {
                    const MaterialPropertyValue& value = baseMaterial->GetPropertyValue(variable.name);
                    if (value.GetValueType() == MaterialPropertyDataType::Sampler)
                    {
                        RHI::Sampler* sampler = value.GetValue<RHI::Sampler*>();
                        if (sampler != nullptr)
                        {
                            shaderResourceGroup->Bind(imageIndex, variable.name, sampler);
                        }
                    }
                }
            }
        }

        shaderResourceGroup->FlushBindings();
    }

    void Material::FlushProperties()
    {
        if (shaderResourceGroup == nullptr)
            RecreateShaderResourceGroup();
        if (shaderResourceGroup == nullptr)
            return;

        for (int imageIndex = 0; imageIndex < RHI::Limits::MaxSwapChainImageCount; imageIndex++)
        {
            FlushProperties(imageIndex);
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
        ShaderVariant* currentShader = GetCurrentOpaqueShader();
        if (!currentShader || !currentShader->GetPipeline())
            return;

		if (shaderResourceGroup)
		{
            delete shaderResourceGroup;
			shaderResourceGroup = nullptr;
		}

        memberOffsetsByVariableName.Clear();

        for (int i = 0; i < updateRequired.GetSize(); i++)
        {
            updateRequired[i] = true;
        }

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
