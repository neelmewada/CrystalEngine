#pragma once

namespace CE::RPI
{
	struct ShaderVariantDescriptor
	{
		RHI::GraphicsPipelineDescriptor pipelineDesc{};
		Array<Name> defineFlags{};
	};

	class CORERPI_API ShaderVariant final
	{
	public:

		ShaderVariant(const ShaderVariantDescriptor& desc);
		~ShaderVariant();

		inline SIZE_T GetVariantId() const { return variantId; }

	private:

		SIZE_T variantId = 0;
		Array<Name> defineFlags{};

		RHI::GraphicsPipelineDescriptor pipelineDesc{};
		
		HashMap<RHI::ShaderStage, RHI::ShaderModule*> modulesByStage{};

		RHI::PipelineState* pipeline = nullptr;

		friend class Shader;
	};
    
} // namespace CE::RPI
