#pragma once

namespace CE::RPI
{
	class Shader;
	class ShaderVariant;

	typedef HashMap<Name, MaterialPropertyValue> MaterialPropertyValueMap;

	class CORERPI_API Material
	{
	public:

		Material(Shader* shader);
		~Material();

		void SetShader(Shader* shader);

		inline Shader* GetShader() const { return shader; }

		ShaderVariant* GetCurrentShader() const;

		inline u32 GetCurrentVariantIndex() const { return shaderVariantIndex; }

		void SelectVariant(u32 variantIndex);

		void Compile();

	private:

		void RecreateShaderResourceGroup();
		
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		MaterialPropertyValueMap properties{};
        
        HashMap<Name, RHI::Buffer*> buffers{};

		Shader* shader = nullptr;

		u32 shaderVariantIndex = 0;
	};
    
} // namespace CE::RPI
