
#include "CoreMinimal.h"
#include "System.h"

#include "RenderViewport.h"

#include "vulkan/vulkan.h"

#include "VulkanRHI.h"

namespace CE::Editor
{
	VulkanViewportRenderer::VulkanViewportRenderer(QVulkanWindow* w)
	{
		this->window = w;
		this->vulkanRHI = (VulkanRHI*)gDynamicRHI;
	}

	void VulkanViewportRenderer::initResources()
	{
		CE_LOG(Info, All, "initResources()");
	}

	void VulkanViewportRenderer::initSwapChainResources()
	{
		CE_LOG(Info, All, "initSwapChainResources()");
	}

	void VulkanViewportRenderer::releaseSwapChainResources()
	{

	}

	void VulkanViewportRenderer::releaseResources()
	{

	}

	void VulkanViewportRenderer::startNextFrame()
	{
		CE_LOG(Info, All, "startNextFrame()");
	}

	RenderViewport::RenderViewport(QWindow* parent)
		: QVulkanWindow(parent)
	{
		instance = new QVulkanInstance;
		instance->setVkInstance((VkInstance)gDynamicRHI->GetNativeHandle());

		this->setVulkanInstance(instance);
	}

	RenderViewport::~RenderViewport()
	{
		delete instance;
	}

	QVulkanWindowRenderer* RenderViewport::createRenderer()
	{
		CE_LOG(Info, All, "createRenderer()");
		return new VulkanViewportRenderer(this);
	}

} // namespace CE::Editor
