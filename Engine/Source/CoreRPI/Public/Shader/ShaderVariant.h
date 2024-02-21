#pragma once

namespace CE::RPI
{
	struct ShaderVariantDescriptor
	{
		RHI::GraphicsPipelineDescriptor pipelineDesc{};
		Array<Name> defineFlags{};
	};

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

	enum class ShaderVariantFlag
	{
		None = 0,
		UseInstancing = BIT(0),
	};
	ENUM_CLASS_FLAGS(ShaderVariantFlag);

	class CORERPI_API ShaderVariant final
	{
	public:

		static constexpr char InstancingFlag[] = "USE_INSTANCING";

		ShaderVariant(const ShaderVariantDescriptor& desc);
		ShaderVariant(const ShaderVariantDescriptor2& desc);

		~ShaderVariant();

		inline SIZE_T GetVariantId() const { return variantId; }

		inline bool UsesInstancing() const { return EnumHasFlag(flags, ShaderVariantFlag::UseInstancing); }
        
        inline RHI::PipelineState* GetPipeline() const { return pipeline; }

	private:

		SIZE_T variantId = 0;
		Array<Name> defineFlags{};

		ShaderVariantFlag flags{};

		RHI::GraphicsPipelineDescriptor pipelineDesc{};
		
		HashMap<RHI::ShaderStage, RHI::ShaderModule*> modulesByStage{};

		RHI::PipelineState* pipeline = nullptr;

		friend class Shader;
	};
    
} // namespace CE::RPI
