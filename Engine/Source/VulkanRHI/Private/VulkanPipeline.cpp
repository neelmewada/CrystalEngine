
#include "VulkanPipeline.h"
#include "VulkanShaderModule.h"
#include "VulkanRenderPass.h"

namespace CE
{

    VulkanPipeline::VulkanPipeline(VulkanDevice* device) : device(device)
    {
        
    }

    VulkanPipeline::~VulkanPipeline()
    {
        
    }
    
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* device, VulkanRenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc)
		: VulkanPipeline(device)
	{
		Create(renderTarget, desc);
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		Destroy();
	}

	void VulkanGraphicsPipeline::Create(VulkanRenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc)
	{
		VkResult result = VK_SUCCESS;

		VkGraphicsPipelineCreateInfo pipelineCI{};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// -- Vertex Input State --
		VkPipelineVertexInputStateCreateInfo vertexInputCI{};
		vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkVertexInputBindingDescription vertexInputBindingDesc = {};
		vertexInputBindingDesc.binding = 0;
		vertexInputBindingDesc.stride = desc.vertexSizeInBytes;
		vertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		Array<VkVertexInputAttributeDescription> vertexAttribs = {};
		for (const auto& vertex : desc.vertexAttribs)
		{
			vertexAttribs.Add({});
			VkVertexInputAttributeDescription& vertexAttribDesc = vertexAttribs.Top();
			vertexAttribDesc.binding = 0;
			vertexAttribDesc.location = vertex.location;
			vertexAttribDesc.offset = vertex.offset;

			if (vertex.dataType == TYPEID(f32))
				vertexAttribDesc.format = VK_FORMAT_R32_SFLOAT;
			else if (vertex.dataType == TYPEID(Vec2))
				vertexAttribDesc.format = VK_FORMAT_R32G32_SFLOAT;
			else if (vertex.dataType == TYPEID(Vec3))
				vertexAttribDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
			else if (vertex.dataType == TYPEID(Vec4))
				vertexAttribDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			else if (vertex.dataType == TYPEID(f64))
				vertexAttribDesc.format = VK_FORMAT_R64_SFLOAT;
			else if (vertex.dataType == TYPEID(Vec2i))
				vertexAttribDesc.format = VK_FORMAT_R32G32_SINT;
			else if (vertex.dataType == TYPEID(Vec3i))
				vertexAttribDesc.format = VK_FORMAT_R32G32B32_SINT;
			else if (vertex.dataType == TYPEID(Vec4i))
				vertexAttribDesc.format = VK_FORMAT_R32G32B32A32_SINT;
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

		VkPipelineShaderStageCreateInfo shaderStages[2] = {};
		VulkanShaderModule* vertexShader = (VulkanShaderModule*)desc.vertexShader;
		VulkanShaderModule* fragmentShader = (VulkanShaderModule*)desc.fragmentShader;

		shaderStages[0] = {};
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].module = vertexShader->GetHandle();
		shaderStages[0].pName = desc.vertexEntry.GetCString();
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;

		shaderStages[1] = {};
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].module = fragmentShader->GetHandle();
		shaderStages[1].pName = desc.fragmentEntry.GetCString();
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		pipelineCI.stageCount = 2;
		pipelineCI.pStages = shaderStages;

		// - Dynamic States -

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCI{};
		dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCI.dynamicStateCount = (u32)COUNTOF(dynamicStates);
		dynamicStateCI.pDynamicStates = dynamicStates;
		
		pipelineCI.pDynamicState = &dynamicStateCI;

		// - Viewport State -

		VkViewport viewport{};
		viewport.x = viewport.y = 0;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		viewport.width = renderTarget->GetWidth();
		viewport.height = renderTarget->GetHeight();

		VkRect2D scissor{};
		scissor.offset.x = scissor.offset.y = 0;
		scissor.extent.width = viewport.width;
		scissor.extent.height = viewport.height;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		pipelineCI.pViewportState = &viewportState;

		// - Rasterization State -

		VkPipelineRasterizationStateCreateInfo rasterizerCI{};
		rasterizerCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCI.depthClampEnable = VK_FALSE;
		rasterizerCI.rasterizerDiscardEnable = VK_FALSE;
		rasterizerCI.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerCI.lineWidth = 1.0f;
		switch (desc.cullMode)
		{
		case RHI::CULL_MODE_NONE:
			rasterizerCI.cullMode = VK_CULL_MODE_NONE;
			break;
		case RHI::CULL_MODE_BACK:
			rasterizerCI.cullMode = VK_CULL_MODE_BACK_BIT;
			break;
		case RHI::CULL_MODE_FRONT:
			rasterizerCI.cullMode = VK_CULL_MODE_FRONT_BIT;
			break;
		case RHI::CULL_MODE_ALL:
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

		// - Render Pass -

		pipelineCI.renderPass = renderTarget->renderPass->GetHandle();

		// - Pipeline Layout -

		VkPipelineLayoutCreateInfo pipelineLayoutCI{};
		pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		
		setLayouts.Resize(2);

		// Set 0
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = 0;
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutCreateInfo setLayoutCI{};
			setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			setLayoutCI.bindingCount = 1;
			setLayoutCI.pBindings = &binding;
			
			auto result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayouts[0]);
			if (result != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create Descriptor Set Layout 0");
				return;
			}
		}

		// Set 1
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = 0;
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutCreateInfo setLayoutCI{};
			setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			setLayoutCI.bindingCount = 1;
			setLayoutCI.pBindings = &binding;

			result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayouts[1]);
			if (result != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create Descriptor Set Layout 0");
				return;
			}
		}

		pipelineLayoutCI.setLayoutCount = setLayouts.GetSize();
		pipelineLayoutCI.pSetLayouts = setLayouts.GetData();

		result = vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCI, nullptr, &pipelineLayout);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Pipeline Layout. Error code {}", (int)result);
			return;
		}

		pipelineCI.layout = pipelineLayout;
		pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCI.basePipelineIndex = -1;

		// - END -
		
		result = vkCreateGraphicsPipelines(device->GetHandle(), VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &pipeline);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Graphics Pipeline. Error code {}", (int)result);
			return;
		}
	}

	void VulkanGraphicsPipeline::Destroy()
	{
		if (pipeline != nullptr)
		{
			vkDestroyPipeline(device->GetHandle(), pipeline, nullptr);
			pipeline = nullptr;
		}

		if (pipelineLayout != nullptr)
		{
			vkDestroyPipelineLayout(device->GetHandle(), pipelineLayout, nullptr);
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

} // namespace CE
