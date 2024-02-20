#pragma once

namespace CE::RPI
{
	/// @brief RPI::Shader is effectively a collection of shader variants.
	class CORERPI_API Shader final
	{
	public:

		Shader();
		~Shader();

		inline u32 GetVariantCount() const { return variants.GetSize(); }

		inline RPI::ShaderVariant* GetVariant(u32 index) const 
		{ 
			if (index >= variants.GetSize()) 
				return nullptr;
			return variants[index]; 
		}

		RPI::ShaderVariant* AddVariant(const ShaderVariantDescriptor& variantDesc);

		RPI::ShaderVariant* AddVariant(const ShaderVariantDescriptor2& variantDesc);

		inline u32 GetDefaultVariantIndex() const { return defaultVariantIndex; }

		inline void SetDefaultVariantIndex(u32 index) { defaultVariantIndex = index; }

	private:

		u32 defaultVariantIndex = 0;

		Array<RPI::ShaderVariant*> variants{};

		friend class Material;
	};

} // namespace CE::RPI
