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
			shaderResourceGroup->QueueDestroy();
			shaderResourceGroup = nullptr;
		}
	}

	void Material::SetShader(Shader* shader)
	{
		if (this->shader == shader)
			return;

		if (shaderResourceGroup)
		{
			shaderResourceGroup->QueueDestroy();
			shaderResourceGroup = nullptr;
		}

		this->shader = shader;
		this->shaderVariantIndex = shader->defaultVariantIndex;

		RecreateShaderResourceGroup();
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

	void Material::Compile()
	{
		if (shaderResourceGroup)
		{
			shaderResourceGroup->Compile();
		}
	}

	void Material::RecreateShaderResourceGroup()
	{
		if (shaderResourceGroup)
		{
			shaderResourceGroup->QueueDestroy();
			shaderResourceGroup = nullptr;
		}

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
