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

		inline RPI::ShaderVariant* GetVariant(u32 index) const { return variants[index]; }

		RPI::ShaderVariant* AddVariant(const ShaderVariantDescriptor& variantDesc);

	private:

		u32 defaultVariantIndex = 0;

		Array<RPI::ShaderVariant*> variants{};

	};

} // namespace CE::RPI
