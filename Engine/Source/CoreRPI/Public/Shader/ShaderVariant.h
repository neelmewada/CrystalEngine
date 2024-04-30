#pragma once

namespace CE::RPI
{

	struct ShaderVariantDescriptor2
	{
		Name shaderName{};
		ShaderReflection reflectionInfo;
		Array<ShaderTagEntry> tags{};
		Array<RHI::ShaderModuleDescriptor> moduleDesc{};
		Array<Name> entryPoints{};

		inline bool TagExists(const Name& key) const
		{
			for (int i = tags.GetSize() - 1; i >= 0; i--)
			{
				if (tags[i].key == key)
					return true;
			}
			return false;
		}

		inline String GetTagValue(const Name& key) const
		{
			for (int i = tags.GetSize() - 1; i >= 0; i--)
			{
				if (tags[i].key == key)
					return tags[i].value;
			}
			return "";
		}
	};


	class CORERPI_API ShaderVariant final
	{
	public:

		ShaderVariant(const ShaderVariantDescriptor2& desc);

		~ShaderVariant();

		inline SIZE_T GetVariantId() const { return variantId; }

        inline RHI::PipelineState* GetPipeline() const { return pipelineCollection->GetPipeline(); }

		RHI::PipelineState* GetPipeline(const RHI::GraphicsPipelineVariant& variant);

		RHI::PipelineState* GetPipeline(const RHI::MultisampleState& multisampleState);

		const RHI::ShaderResourceGroupLayout& GetSrgLayout(RHI::SRGType srgType);

		const ShaderReflection& GetShaderReflection() const { return reflectionInfo; }

	private:

		SIZE_T variantId = 0;
		Array<Name> defineFlags{};

		RHI::GraphicsPipelineDescriptor pipelineDesc{};

		GraphicsPipelineCollection* pipelineCollection = nullptr;
		
		HashMap<RHI::ShaderStage, RHI::ShaderModule*> modulesByStage{};

		//RHI::PipelineState* pipeline = nullptr;
		ShaderReflection reflectionInfo;

		friend class Shader;
	};
    
} // namespace CE::RPI
