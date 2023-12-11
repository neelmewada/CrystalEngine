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

} // namespace CE::RPI
