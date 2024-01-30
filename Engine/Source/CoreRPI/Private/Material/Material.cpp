#include "CoreRPI.h"

namespace CE::RPI
{
	Material::Material()
	{
		
	}

	Material::~Material()
	{
		delete shaderResourceGroup; shaderResourceGroup = nullptr;
	}

	void Material::SetShader(Shader* shader)
	{
		if (this->shader == shader)
			return;

		delete shaderResourceGroup; shaderResourceGroup = nullptr;

		this->shader = shader;

	}
    
} // namespace CE::RPI
