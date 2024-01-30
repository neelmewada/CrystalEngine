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

	void Shader::AddVariant(const ShaderVariantDescriptor& variantDesc)
	{
		variants.Add(new ShaderVariant(variantDesc));
	}

} // namespace CE::RPI
