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
	class ShaderCollection;
	class ShaderVariant;

	typedef HashMap<Name, MaterialPropertyValueArray> MaterialPropertyValueMap;

	class CORERPI_API Material
	{
	public:

		Material(ShaderCollection* shaderCollection);
		~Material();

		ShaderCollection* GetShaderCollection() const;

		void SetShaderCollection(ShaderCollection* shaderCollection);

		Shader* GetOpaqueShader() const;

		ShaderVariant* GetCurrentOpaqueShader();

		inline u32 GetCurrentVariantIndex() const { return shaderVariantIndex; }

		void SelectVariant(u32 variantIndex);
        
        void FlushProperties(u32 imageIndex);

		void FlushProperties();

		void Compile();
        
        bool IsCompiled() const
        {
            return shaderResourceGroup != nullptr && shaderResourceGroup->IsCompiled();
        }

		bool PropertyExists(Name name) const;
        
		void ClearAllValues();

		void SetCustomShaderItem(int customItemIndex);

		RPI::Shader* GetCustomShaderItem();

        void SetPropertyValue(Name propertyName, const MaterialPropertyValue& value);

		//! Used for shader resource arrays
		void InsertPropertyValue(Name propertyName, const MaterialPropertyValue& value, int index = -1);

		//! Used for shader resource arrays
		void RemovePropertyValue(Name propertyName, int index);

		void ClearPropertyValue(Name propertyName);

		const MaterialPropertyValue& GetPropertyValue(Name propertyName);

		//! Used for shader resource arrays
		const MaterialPropertyValue& GetPropertyValue(Name propertyName, int index);
        
        RHI::ShaderResourceGroup* GetShaderResourceGroup() const { return shaderResourceGroup; }

		void CopyPropertiesFrom(RPI::Material* other);

		void SetBaseMaterial(RPI::Material* mat) { baseMaterial = mat; }

	private:

		void RecreateShaderResourceGroup();
		
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		RPI::Material* baseMaterial = nullptr;

		MaterialPropertyValueMap properties{};
        
        StaticArray<HashMap<Name, RHI::Buffer*>, RHI::Limits::MaxSwapChainImageCount> buffersByVariableNamePerImage{};
		StaticArray<bool, RHI::Limits::MaxSwapChainImageCount> updateRequired{};

        HashMap<Name, Array<u64>> memberOffsetsByVariableName{};

		ShaderCollection* shaderCollection = nullptr;

		RPI::Shader* opaqueShader = nullptr;

		u32 shaderVariantIndex = 0;

		int customShaderItem = -1;

		friend class MaterialInstance;
		friend class MaterialInterface;
		friend class CE::Material;
	};
    
} // namespace CE::RPI
