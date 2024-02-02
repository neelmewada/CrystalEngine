#include "CoreRPI.h"

namespace CE::RPI
{

	ShaderVariant::ShaderVariant(const ShaderVariantDescriptor& desc)
		: defineFlags(desc.defineFlags)
		, pipelineDesc(desc.pipelineDesc)
		, materialSrgLayout(desc.materialSrgLayout)
	{
		variantId = 0;

		for (int i = 0; i < defineFlags.GetSize(); i++)
		{
			if (i == 0)
				variantId = defineFlags[i].GetHashValue();
			else
				variantId = GetCombinedHash(variantId, defineFlags[i].GetHashValue());

			if (defineFlags[i] == InstancingFlag)
			{
				flags |= ShaderVariantFlag::UseInstancing;
			}
		}

		for (const auto& shaderStage : pipelineDesc.shaderStages)
		{
			if (shaderStage.shaderModule)
			{
				modulesByStage[shaderStage.shaderModule->GetShaderStage()] = shaderStage.shaderModule;
			}
		}
		
		pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(pipelineDesc);
	}

	ShaderVariant::~ShaderVariant()
	{
		delete pipeline; pipeline = nullptr;

		for (auto [stage, module] : modulesByStage)
		{
			if (module == nullptr) 
				continue;
			RHI::gDynamicRHI->DestroyShaderModule(module);
		}
		modulesByStage.Clear();
	}

} // namespace CE::RPI
