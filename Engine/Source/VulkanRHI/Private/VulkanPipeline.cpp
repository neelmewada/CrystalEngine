
#include "VulkanPipeline.h"
#include "VulkanShaderModule.h"

namespace CE
{

    VulkanPipeline::VulkanPipeline(VulkanDevice* device)
    {
        
    }

    VulkanPipeline::~VulkanPipeline()
    {
        
    }
    
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDesc& desc)
		: VulkanPipeline(device)
	{
		Create(desc);
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
	}

	void VulkanGraphicsPipeline::Create(const RHI::GraphicsPipelineDesc& desc)
	{
		List<VkDescriptorSetLayoutCreateInfo> setLayoutInfos{};

		List<VkDescriptorSetLayout> setLayouts{};

		VkPipelineLayoutCreateInfo layoutCI{};
		layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		VkGraphicsPipelineCreateInfo pipelineCI{};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// -- Vertex Input State --
		VkPipelineVertexInputStateCreateInfo vertexInputCI{};
		vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkVertexInputBindingDescription vertexInputBindingDesc = {};
		vertexInputBindingDesc.binding = 0;
		vertexInputBindingDesc.stride = desc.vertexSizeInBytes;
		vertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		vertexInputCI.pVertexBindingDescriptions = &vertexInputBindingDesc;
		vertexInputCI.vertexBindingDescriptionCount = 1;

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
	}

} // namespace CE
