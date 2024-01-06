
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

    PipelineState::PipelineState(VulkanDevice* device) : device(device)
    {
        
    }

    PipelineState::~PipelineState()
    {
        
    }
    
	GraphicsPipelineState::GraphicsPipelineState(VulkanDevice* device, RenderTarget* renderTarget, const RHI::GraphicsPipelineDescriptor& desc)
		: PipelineState(device)
	{
		Create(renderTarget, desc);
	}

	GraphicsPipelineState::~GraphicsPipelineState()
	{
		Destroy();
	}

	void GraphicsPipelineState::Create(RenderTarget* renderTarget, const RHI::GraphicsPipelineDescriptor& desc)
	{
		VkResult result = VK_SUCCESS;

		VkGraphicsPipelineCreateInfo pipelineCI{};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// -- Vertex Input State --
		VkPipelineVertexInputStateCreateInfo vertexInputCI{};
		vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkVertexInputBindingDescription vertexInputBindingDesc = {};
		vertexInputBindingDesc.binding = 0;
		vertexInputBindingDesc.stride = desc.vertexStrideInBytes;
		vertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		FixedArray<VkVertexInputAttributeDescription, RHI::Limits::Pipeline::MaxVertexAttribCount> vertexAttribs = {};
		for (const auto& vertex : desc.vertexAttribs)
		{
			VkVertexInputAttributeDescription vertexAttribDesc = {};
			vertexAttribDesc.binding = 0;
			vertexAttribDesc.location = vertex.location;
			vertexAttribDesc.offset = vertex.offset;

			switch (vertex.dataType)
			{
			case VertexAttributeDataType::Float:
				vertexAttribDesc.format = VK_FORMAT_R32_SFLOAT;
				break;
			case VertexAttributeDataType::Float2:
				vertexAttribDesc.format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case VertexAttributeDataType::Float3:
				vertexAttribDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case VertexAttributeDataType::Float4:
				vertexAttribDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case VertexAttributeDataType::Int:
				vertexAttribDesc.format = VK_FORMAT_R32_SINT;
				break;
			case VertexAttributeDataType::Int2:
				vertexAttribDesc.format = VK_FORMAT_R32G32_SINT;
				break;
			case VertexAttributeDataType::Int3:
				vertexAttribDesc.format = VK_FORMAT_R32G32B32_SINT;
				break;
			case VertexAttributeDataType::Int4:
				vertexAttribDesc.format = VK_FORMAT_R32G32B32A32_SINT;
				break;
			}

			vertexAttribs.Add(vertexAttribDesc);
		}
		
		vertexInputCI.pVertexBindingDescriptions = &vertexInputBindingDesc;
		vertexInputCI.vertexBindingDescriptionCount = 1;
		vertexInputCI.pVertexAttributeDescriptions = vertexAttribs.GetData();
		vertexInputCI.vertexAttributeDescriptionCount = vertexAttribs.GetSize();

		pipelineCI.pVertexInputState = &vertexInputCI;

		// - Input Assember -

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		pipelineCI.pInputAssemblyState = &inputAssembly;

		// - Shader Stages -

		FixedArray<VkPipelineShaderStageCreateInfo, (SIZE_T)RHI::ShaderStage::COUNT> shaderStages{};

		for (const auto& shaderStage : desc.shaderStages)
		{
			VkPipelineShaderStageCreateInfo stageInfo{};
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

			Vulkan::ShaderModule* module = (Vulkan::ShaderModule*)shaderStage.shaderModule;
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

			switch (shaderStage.stage)
			{
			case RHI::ShaderStage::Vertex:
				stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case RHI::ShaderStage::Fragment:
				stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case RHI::ShaderStage::Geometry:
				stageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;
			}

			stageInfo.pName = shaderStage.entryPoint.GetCString();
			stageInfo.module = module->GetHandle();

			shaderStages.Add(stageInfo);
		}
		
		pipelineCI.stageCount = shaderStages.GetSize();
		pipelineCI.pStages = shaderStages.GetData();

		// - Dynamic States -

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCI{};
		dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCI.dynamicStateCount = static_cast<u32>(COUNTOF(dynamicStates));
		dynamicStateCI.pDynamicStates = dynamicStates;
		
		pipelineCI.pDynamicState = &dynamicStateCI;

		// - Viewport State -

		/*VkViewport viewport{};
		viewport.x = viewport.y = 0;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		viewport.width = renderTarget->GetWidth();
		viewport.height = renderTarget->GetHeight();

		VkRect2D scissor{};
		scissor.offset.x = scissor.offset.y = 0;
		scissor.extent.width = viewport.width;
		scissor.extent.height = viewport.height;*/

		/*VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;*/

		//pipelineCI.pViewportState = &viewportState;

		// - Rasterization State -

		VkPipelineRasterizationStateCreateInfo rasterizerCI{};
		rasterizerCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCI.depthClampEnable = VK_FALSE;
		rasterizerCI.rasterizerDiscardEnable = VK_FALSE;
		rasterizerCI.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerCI.lineWidth = 1.0f;
		switch (desc.cullMode)
		{
		case RHI::CullMode::None:
			rasterizerCI.cullMode = VK_CULL_MODE_NONE;
			break;
		case RHI::CullMode::Back:
			rasterizerCI.cullMode = VK_CULL_MODE_BACK_BIT;
			break;
		case RHI::CullMode::Front:
			rasterizerCI.cullMode = VK_CULL_MODE_FRONT_BIT;
			break;
		case RHI::CullMode::All:
			rasterizerCI.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
			break;
		}
		rasterizerCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerCI.depthBiasEnable = VK_FALSE;
		rasterizerCI.depthBiasClamp = 0;
		rasterizerCI.depthBiasConstantFactor = 0;
		rasterizerCI.depthBiasSlopeFactor = 0;

		pipelineCI.pRasterizationState = &rasterizerCI;

		// - Multisample -
		
		VkPipelineMultisampleStateCreateInfo multisampleCI{};
		multisampleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleCI.sampleShadingEnable = VK_FALSE;
		multisampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleCI.minSampleShading = 1.0f;
		multisampleCI.pSampleMask = nullptr;
		multisampleCI.alphaToCoverageEnable = VK_FALSE;
		multisampleCI.alphaToOneEnable = VK_FALSE;

		pipelineCI.pMultisampleState = &multisampleCI;

		// - Color Blending -

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.blendConstants[0] = 0;
		colorBlending.blendConstants[1] = 0;
		colorBlending.blendConstants[2] = 0;
		colorBlending.blendConstants[3] = 0;
		
		pipelineCI.pColorBlendState = &colorBlending;

		// - Depth Stencil -

		VkPipelineDepthStencilStateCreateInfo depthStencilCI{};
		depthStencilCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCI.depthTestEnable = VK_TRUE;
		depthStencilCI.depthWriteEnable = VK_TRUE;
		depthStencilCI.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilCI.depthBoundsTestEnable = VK_FALSE;
		depthStencilCI.minDepthBounds = 0.0f;
		depthStencilCI.maxDepthBounds = 1.0f;
		depthStencilCI.stencilTestEnable = VK_FALSE;
		depthStencilCI.front = {};
		depthStencilCI.back = {};

		pipelineCI.pDepthStencilState = &depthStencilCI;

		// - Pipeline Layout -

		auto srgManager = device->GetShaderResourceManager();

		VkPipelineLayoutCreateInfo pipelineLayoutCI{};
		pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		setLayouts.Clear();
		setLayoutBindingsMap.Clear();

		/*for (int i = 0; i < desc.shaderResourceGroups.GetSize(); i++)
		{
			auto srg = desc.shaderResourceGroups[i];

			Array<VkDescriptorSetLayoutBinding> setLayoutBindings{};

			for (const auto& variable : srg.variables)
			{
				VkDescriptorType descriptorType;
				u32 bindingSlot = 0;
				if (desc.variableNameBindingMap.KeyExists(variable.name))
					bindingSlot = desc.variableNameBindingMap.Get(variable.name);
				
				switch (variable.type)
				{
				case RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER:
					descriptorType = variable.isDynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					break;
				case RHI::SHADER_RESOURCE_TYPE_STRUCTURED_BUFFER:
					descriptorType = variable.isDynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					break;
				case RHI::SHADER_RESOURCE_TYPE_SAMPLED_IMAGE:
					descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					break;
				case RHI::SHADER_RESOURCE_TYPE_SAMPLER_STATE:
					descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
					break;
				default:
					continue;
				}

				VkShaderStageFlags stageFlags = 0;
				if (EnumHasFlag(variable.stageFlags, RHI::ShaderStage::Vertex))
					stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
				if (EnumHasFlag(variable.stageFlags, RHI::ShaderStage::Fragment))
					stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

				setLayoutBindings.Add({
					.binding = bindingSlot,
					.descriptorType = descriptorType,
					.descriptorCount = variable.arrayCount,
					.stageFlags = stageFlags
				});
			}

			setLayoutBindingsMap.Add(srgManager->GetDescriptorSetNumber(srg.srgType), setLayoutBindings);

			VkDescriptorSetLayoutCreateInfo setLayoutCI{};
			setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			setLayoutCI.bindingCount = setLayoutBindings.GetSize();
			setLayoutCI.pBindings = setLayoutBindings.GetData();

			VkDescriptorSetLayout setLayout = nullptr;
			
			auto result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayout);
			if (result != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create Descriptor Set Layout. Error code {}", (int)result);
				return;
			}

			setLayouts.Add(setLayout);
		}*/

		pipelineLayoutCI.setLayoutCount = setLayouts.GetSize();
		pipelineLayoutCI.pSetLayouts = setLayouts.GetData();
		
		pipelineLayoutCI.pushConstantRangeCount = pushConstantRanges.GetSize();
		pipelineLayoutCI.pPushConstantRanges = pushConstantRanges.GetData();

		VkPipelineLayout pipelineLayout = nullptr;

		result = vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCI, nullptr, &pipelineLayout);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Pipeline Layout. Error code {}", (int)result);
			return;
		}

		if (this->pipelineLayout == nullptr)
			this->pipelineLayout = new PipelineLayout(device, this);
		
		this->pipelineLayout->handle = pipelineLayout;

		pipelineCI.layout = pipelineLayout;
        
        // No need to use pipeline derivatives
		pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCI.basePipelineIndex = -1;
        
        // - Render Pass -
        
        pipelineCI.subpass = 0;
        //pipelineCI.renderPass = renderTarget->renderPass->GetHandle();

		// - END -
		
		result = vkCreateGraphicsPipelines(device->GetHandle(), VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &pipeline);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Graphics Pipeline. Error code {}", (int)result);
			return;
		}
	}

	void GraphicsPipelineState::Destroy()
	{
		if (pipeline != nullptr)
		{
			vkDestroyPipeline(device->GetHandle(), pipeline, nullptr);
			pipeline = nullptr;
		}

		if (pipelineLayout != nullptr)
		{
			delete pipelineLayout;
			pipelineLayout = nullptr;
		}

		for (auto layout : setLayouts)
		{
			if (layout != nullptr)
			{
				vkDestroyDescriptorSetLayout(device->GetHandle(), layout, nullptr);
			}
		}
		setLayouts.Clear();
	}

	PipelineLayout::PipelineLayout(VulkanDevice* device, PipelineState* copyFrom)
		: device(device)
	{
		CopyFrom(device, copyFrom);
	}

	PipelineLayout::PipelineLayout(VulkanDevice* device, VkPipelineLayout pipelineLayout, RHI::PipelineType pipelineType)
		: device(device), handle(pipelineLayout), pipelineType(pipelineType)
	{

	}

	PipelineLayout::~PipelineLayout()
	{
		if (handle != nullptr)
		{
			vkDestroyPipelineLayout(device->GetHandle(), handle, nullptr);
			handle = nullptr;
		}

		device = nullptr;
	}

	void PipelineLayout::CopyFrom(VulkanDevice* device, PipelineState* copyFrom)
	{
		if (copyFrom->IsGraphicsPipelineState())
		{
			pipelineType = RHI::PipelineType::Graphics;
		}
		else
		{
			pipelineType = RHI::PipelineType::Compute;
		}

		pushConstantRanges = copyFrom->pushConstantRanges;
		setLayouts = copyFrom->setLayouts;
		setLayoutBindingsMap = copyFrom->setLayoutBindingsMap;
	}

} // namespace CE
