#include "CoreRPI.h"

namespace CE::RPI
{

	ShaderVariant::ShaderVariant(const ShaderVariantDescriptor& desc)
		: defineFlags(desc.defineFlags)
		, pipelineDesc(desc.pipelineDesc)
	{
		variantId = 0;

		for (int i = 0; i < defineFlags.GetSize(); i++)
		{
			if (i == 0)
				variantId = defineFlags[i].GetHashValue();
			else
				variantId = GetCombinedHash(variantId, defineFlags[i].GetHashValue());
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

	ShaderVariant::ShaderVariant(const ShaderVariantDescriptor2& desc)
	{
		variantId = 0;

		//const auto& subShaderEntry = desc.preprocessData->subShaders[desc.subShaderIndex];
		//const auto& passEntry = subShaderEntry.passes[desc.passIndex];

		pipelineDesc = {};
		pipelineDesc.name = desc.shaderName;

		bool blendFactorFound = false;
		EnumType* blendFactorEnum = GetStaticEnum<RHI::BlendFactor>();
		EnumType* blendOpEnum = GetStaticEnum<RHI::BlendOp>();
		EnumType* depthOpEnum = GetStaticEnum<RHI::CompareOp>();
		EnumType* cullModeEnum = GetStaticEnum<RHI::CullMode>();

		RHI::ColorBlendState colorBlend = {};
		
		if (desc.TagExists("Blend"))
		{
			String value = desc.GetTagValue("Blend").RemoveWhitespaces();
			Array<String> splits = value.Split(',');
			if (splits.GetSize() == 2)
			{
				EnumConstant* srcBlend = blendFactorEnum->FindConstantWithName(splits[0]);
				EnumConstant* dstBlend = blendFactorEnum->FindConstantWithName(splits[1]);
				
				if (srcBlend != nullptr && dstBlend != nullptr)
				{
					colorBlend.srcColorBlend = (RHI::BlendFactor)srcBlend->GetValue();
					colorBlend.dstColorBlend = (RHI::BlendFactor)dstBlend->GetValue();
					colorBlend.srcAlphaBlend = RHI::BlendFactor::One;
					colorBlend.dstAlphaBlend = RHI::BlendFactor::Zero;
					blendFactorFound = true;
				}
			}
		}

		if (desc.TagExists("BlendOp"))
		{
			String value = desc.GetTagValue("BlendOp").RemoveWhitespaces();
			EnumConstant* enumConstant = blendOpEnum->FindConstantWithName(value);
			
			if (enumConstant)
			{
				colorBlend.colorBlendOp = (RHI::BlendOp)enumConstant->GetValue();
				blendFactorFound = true;
			}
		}

		if (!blendFactorFound)
		{
			colorBlend = RHI::ColorBlendState(); // Default value
		}

		pipelineDesc.blendState.colorBlends.Add(colorBlend);
		
		pipelineDesc.srgLayouts = desc.reflectionInfo.srgLayouts;

		pipelineDesc.multisampleState.sampleCount = 1;

		pipelineDesc.depthStencilState.depthState.enable = true;
		pipelineDesc.depthStencilState.depthState.testEnable = true;
		pipelineDesc.depthStencilState.depthState.writeEnable = false;

		if (desc.TagExists("ZWrite"))
		{
			String zwriteValue = desc.GetTagValue("ZWrite");
			
			if (zwriteValue == "On")
			{
				pipelineDesc.depthStencilState.depthState.writeEnable = true;
			}
			else
			{
				pipelineDesc.depthStencilState.depthState.writeEnable = false;
			}
		}

		if (desc.TagExists("ZTest"))
		{
			String ztestValue = desc.GetTagValue("ZTest");
			EnumConstant* enumConstant = depthOpEnum->FindConstantWithName(ztestValue);

			if (enumConstant)
			{
				pipelineDesc.depthStencilState.depthState.testEnable = true;
				pipelineDesc.depthStencilState.depthState.compareOp = (RHI::CompareOp)enumConstant->GetValue();
			}
			else
			{
				pipelineDesc.depthStencilState.depthState.testEnable = false;
			}
		}

		if (!pipelineDesc.depthStencilState.depthState.testEnable)
		{
			pipelineDesc.depthStencilState.depthState.enable = false;
		}

		if (desc.TagExists("Cull"))
		{
			String cullValue = desc.GetTagValue("Cull");
			EnumConstant* enumConstant = cullModeEnum->FindConstantWithName(cullValue);

			if (enumConstant)
			{
				pipelineDesc.rasterState.cullMode = (RHI::CullMode)enumConstant->GetValue();
			}
			else
			{
				pipelineDesc.rasterState.cullMode = RHI::CullMode::Back;
			}
		}

		for (int i = 0; i < desc.reflectionInfo.vertexInputs.GetSize(); i++)
		{
			RHI::ShaderSemantic shaderSemantic = RHI::ShaderSemantic::Parse(desc.reflectionInfo.vertexInputs[i].GetString());
			
			RHI::VertexInputSlotDescriptor inputSlotDesc{};
			inputSlotDesc.inputSlot = i;
			inputSlotDesc.inputRate = RHI::VertexInputRate::PerVertex;

			RHI::VertexAttributeDescriptor vertexAttrib{};
			vertexAttrib.inputSlot = i;
			vertexAttrib.location = i;
			vertexAttrib.offset = 0;

			switch (shaderSemantic.attribute)
			{
			case VertexInputAttribute::Position:
				inputSlotDesc.stride = sizeof(Vec4);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float4;
				break;
			case VertexInputAttribute::UV:
				inputSlotDesc.stride = sizeof(Vec2);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float2;
				break;
			case VertexInputAttribute::Normal:
				inputSlotDesc.stride = sizeof(Vec4);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float4;
				break;
			case VertexInputAttribute::Tangent:
				inputSlotDesc.stride = sizeof(Vec4);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float4;
				break;
			case VertexInputAttribute::Binormal:
				inputSlotDesc.stride = sizeof(Vec4);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float4;
				break;
			case VertexInputAttribute::Color:
				inputSlotDesc.stride = sizeof(Vec4);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float4;
				break;
			case VertexInputAttribute::BlendIndices:
				inputSlotDesc.stride = sizeof(u32);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::UInt;
				break;
			case VertexInputAttribute::BlendWeight:
				inputSlotDesc.stride = sizeof(f32);
				vertexAttrib.dataType = RHI::VertexAttributeDataType::Float;
				break;
			default:
				continue;
			}

			pipelineDesc.vertexInputSlots.Add(inputSlotDesc);
			pipelineDesc.vertexAttributes.Add(vertexAttrib);
		}

		modulesByStage.Clear();

		for (int i = 0; i < desc.moduleDesc.GetSize(); i++)
		{
			auto module = RHI::gDynamicRHI->CreateShaderModule(desc.moduleDesc[i]);
			modulesByStage[desc.moduleDesc[i].stage] = module;

			pipelineDesc.shaderStages.Add({});
			pipelineDesc.shaderStages.Top().entryPoint = desc.entryPoints[i];
			pipelineDesc.shaderStages.Top().shaderModule = module;
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
