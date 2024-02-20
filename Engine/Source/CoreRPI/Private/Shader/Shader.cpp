#include "CoreRPI.h"

namespace CE::RPI
{
    
	Shader::Shader()
	{

	}

	Shader::~Shader()
	{
		for (RPI::ShaderVariant* variant : variants)
		{
			delete variant;
		}
		variants.Clear();
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
