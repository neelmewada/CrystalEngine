#pragma once

namespace CE::RPI
{
	class Shader;

	typedef HashMap<Name, MaterialPropertyValue> MaterialPropertyValueMap;

	class CORERPI_API Material
	{
	public:

		Material();
		~Material();

		void SetShader(Shader* shader);

		inline Shader* GetShader() const { return shader; }

	private:
		
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		MaterialPropertyValueMap properties{};

		Shader* shader = nullptr;

		u32 shaderVariantIndex = 0;
	};
    
} // namespace CE::RPI
