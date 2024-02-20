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

	ShaderVariant::ShaderVariant(const ShaderVariantDescriptor2& desc)
	{
		variantId = 0;

		const auto& subShaderEntry = desc.preprocessData->subShaders[desc.subShaderIndex];
		const auto& passEntry = subShaderEntry.passes[desc.passIndex];

		pipelineDesc = {};
		pipelineDesc.name = desc.preprocessData->shaderName;

		bool blendFactorFound = false;
		EnumType* blendFactorEnum = GetStaticEnum<RHI::BlendFactor>();
		EnumType* blendOpEnum = GetStaticEnum<RHI::BlendOp>();
		EnumType* depthOpEnum = GetStaticEnum<RHI::CompareOp>();
		EnumType* cullModeEnum = GetStaticEnum<RHI::CullMode>();

		RHI::ColorBlendState colorBlend = {};
		
		if (passEntry.TagExists("Blend"))
		{
			String value = passEntry.GetTagValue("Blend").RemoveWhitespaces();
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
		else if (subShaderEntry.TagExists("Blend"))
		{
			String value = subShaderEntry.GetTagValue("Blend").RemoveWhitespaces();
			Array<String> splits = value.Split(',');
			if (splits.GetSize() == 2)
			{
				RHI::ColorBlendState colorBlend = {};
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

		if (passEntry.TagExists("BlendOp"))
		{
			String value = passEntry.GetTagValue("BlendOp").RemoveWhitespaces();
			EnumConstant* enumConstant = blendOpEnum->FindConstantWithName(value);
			
			if (enumConstant)
			{
				colorBlend.colorBlendOp = (RHI::BlendOp)enumConstant->GetValue();
				blendFactorFound = true;
			}
		}
		else if (subShaderEntry.TagExists("BlendOp"))
		{
			String value = subShaderEntry.GetTagValue("BlendOp").RemoveWhitespaces();
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

		if (passEntry.TagExists("ZWrite"))
		{
			String zwriteValue = passEntry.GetTagValue("ZWrite");
			
			if (zwriteValue == "On")
			{
				pipelineDesc.depthStencilState.depthState.writeEnable = true;
			}
			else
			{
				pipelineDesc.depthStencilState.depthState.writeEnable = false;
			}
		}
		else if (subShaderEntry.TagExists("ZWrite"))
		{
			String zwriteValue = subShaderEntry.GetTagValue("ZWrite");

			if (zwriteValue == "On")
			{
				pipelineDesc.depthStencilState.depthState.writeEnable = true;
			}
			else
			{
				pipelineDesc.depthStencilState.depthState.writeEnable = false;
			}
		}

		if (passEntry.TagExists("ZTest"))
		{
			String ztestValue = passEntry.GetTagValue("ZTest");
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
		else if (subShaderEntry.TagExists("ZTest"))
		{
			String ztestValue = subShaderEntry.GetTagValue("ZTest");
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

		if (passEntry.TagExists("Cull"))
		{
			String cullValue = passEntry.GetTagValue("Cull");
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
		else if (subShaderEntry.TagExists("Cull"))
		{
			String cullValue = subShaderEntry.GetTagValue("Cull");
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
			const RHI::ShaderSemantic& shaderSemantic = desc.reflectionInfo.vertexInputs[i];
			
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

		for (const auto& moduleDesc : desc.moduleDesc)
		{
			auto module = RHI::gDynamicRHI->CreateShaderModule(moduleDesc);
			modulesByStage[moduleDesc.stage] = module;

			Name entryPoint = "Main";
			if (moduleDesc.stage == RHI::ShaderStage::Vertex)
			{
				entryPoint = desc.preprocessData->subShaders[desc.subShaderIndex].passes[desc.passIndex].vertexEntry;
			}
			else if (moduleDesc.stage == RHI::ShaderStage::Fragment)
			{
				entryPoint = desc.preprocessData->subShaders[desc.subShaderIndex].passes[desc.passIndex].fragmentEntry;
			}

			pipelineDesc.shaderStages.Add({});
			pipelineDesc.shaderStages.Top().entryPoint = entryPoint;
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
