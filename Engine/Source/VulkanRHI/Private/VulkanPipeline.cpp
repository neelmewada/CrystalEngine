
#include "VulkanPipeline.h"

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
		// -- Vertex Input State --
		VkVertexInputBindingDescription vertexInputDesc = {};
		vertexInputDesc.binding = 0;
		vertexInputDesc.stride = desc.vertexSizeInBytes;
		vertexInputDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		VkDescriptorSetLayoutBinding binding0{};
		binding0.binding = 0;
		binding0.descriptorCount = 1;
		binding0.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		binding0.pImmutableSamplers = nullptr;

		List<VkDescriptorSetLayoutCreateInfo> setLayoutInfos{};

		List<VkDescriptorSetLayout> setLayouts{};

		VkPipelineLayoutCreateInfo layoutCI{};
		layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		VkGraphicsPipelineCreateInfo pipelineCI{};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
	}

} // namespace CE
