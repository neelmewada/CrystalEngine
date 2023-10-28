#pragma once

namespace CE
{
	class Shader;

	CLASS()
	class SYSTEM_API MaterialInterface : public Object
	{
		CE_CLASS(MaterialInterface, Object)
	public:

		MaterialInterface();

		inline Shader* GetShader() const { return shader; }

		inline void SetShader(Shader* shader) { this->shader = shader; }

	protected:

		FIELD()
		Shader* shader = nullptr;

		Shader* errorShader = nullptr;
	};
    
} // namespace CE

#include "MaterialInterface.rtti.h"