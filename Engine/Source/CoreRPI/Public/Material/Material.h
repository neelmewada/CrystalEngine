#pragma once

namespace CE
{
	class Material;
	class MaterialInstance;
	class MaterialInterface;
}

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
        
        void FlushProperties(u32 imageIndex);

		void FlushProperties();

		void Compile();
        
        bool IsCompiled() const
        {
            return shaderResourceGroup != nullptr && shaderResourceGroup->IsCompiled();
        }

		inline bool PropertyExists(Name name) const
		{
			return properties.KeyExists(name) && properties.Get(name).GetValueType() != MaterialPropertyDataType::None;
		}
        
		void ClearAllValues();

        void SetPropertyValue(Name propertyName, const MaterialPropertyValue& value);

		const MaterialPropertyValue& GetPropertyValue(Name propertyName);
        
        inline RHI::ShaderResourceGroup* GetShaderResourceGroup() const { return shaderResourceGroup; }

		void CopyPropertiesFrom(RPI::Material* other);

		inline void SetBaseMaterial(RPI::Material* mat) { baseMaterial = mat; }

	private:

		void RecreateShaderResourceGroup();
		
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		RPI::Material* baseMaterial = nullptr;

		MaterialPropertyValueMap properties{};
        
        StaticArray<HashMap<Name, RHI::Buffer*>, RHI::Limits::MaxSwapChainImageCount> buffersByVariableNamePerImage{};
		StaticArray<bool, RHI::Limits::MaxSwapChainImageCount> updateRequired{};

        HashMap<Name, Array<u64>> memberOffsetsByVariableName{};

		Shader* shader = nullptr;

		u32 shaderVariantIndex = 0;

		friend class MaterialInstance;
		friend class MaterialInterface;
		friend class CE::Material;
	};
    
} // namespace CE::RPI
