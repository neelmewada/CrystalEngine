#include "CoreRPI.h"

namespace CE::RPI
{

	Shader::Shader(RHI::DrawListTag drawListTag) : drawListTag(drawListTag)
	{

	}

	Shader::~Shader()
	{
		if (drawListTag.IsValid())
		{
			RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
			drawListTag = {};
		}

		for (RPI::ShaderVariant* variant : variants)
		{
			delete variant;
		}
		variants.Clear();
	}

	Name Shader::GetName()
	{
		if (variants.NonEmpty())
		{
			return variants[0]->pipelineDesc.name;
		}

		return Name();
	}

    RPI::ShaderVariant* Shader::AddVariant(const ShaderVariantDescriptor& variantDesc)
	{
		variants.Add(new ShaderVariant(variantDesc));
        return variants.Top();
	}

	RPI::ShaderVariant* Shader::AddVariant(const ShaderVariantDescriptor2& variantDesc)
	{
		variants.Add(new ShaderVariant(variantDesc));
		return variants.Top();
	}

} // namespace CE::RPI
