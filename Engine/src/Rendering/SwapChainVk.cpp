
#include "SwapChainVk.h"
#include "RenderContextVk.h"

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <array>

using namespace Vox;

SwapChainVk::SwapChainVk(SwapChainCreateInfoVk &swapChainInfo)
{
    m_pEngine = dynamic_cast<EngineContextVk*>(swapChainInfo.engineContext);
    m_pDevice = dynamic_cast<DeviceContextVk*>(swapChainInfo.deviceContext);

    CreateSwapChain();
    CreateRenderPass();
    CreateFramebuffers();
    CreateCommandBuffers();
    CreateSyncObjects();

    std::cout << "Created SwapChainVk" << std::endl;
}

SwapChainVk::~SwapChainVk() noexcept
{
    // Wait until pDevice finishes running draw commands, executing graphics queue, etc. Should always be called on the same thread as the rendering thread.
    vkDeviceWaitIdle(m_pDevice->GetDevice());

    // Sync Objects: Semaphores & Fences
    for (int i = 0; i < m_MaxSimultaneousFrameDraws; ++i)
    {
        vkDestroyFence(m_pDevice->GetDevice(), m_DrawFinishedFences[i], nullptr);
        vkDestroySemaphore(m_pDevice->GetDevice(), m_RenderFinished[i], nullptr);
        vkDestroySemaphore(m_pDevice->GetDevice(), m_ImageAvailableForRendering[i], nullptr);
    }

    // De-allocate (free) command buffers
    vkFreeCommandBuffers(m_pDevice->GetDevice(), m_pDevice->GetGraphicsCommandPool(),
                         static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());

    // Framebuffers
    for (const auto& framebuffer: m_SwapchainFramebuffers)
    {
        vkDestroyFramebuffer(m_pDevice->GetDevice(), framebuffer, nullptr);
    }

    // RenderPass
    vkDestroyRenderPass(m_pDevice->GetDevice(), m_RenderPass, nullptr);

    // Swapchain Images
    for (const auto& image: m_SwapchainImages)
    {
        vkDestroyImageView(m_pDevice->GetDevice(), image.imageView, nullptr);
    }

    // Swapchain
    vkDestroySwapchainKHR(m_pDevice->GetDevice(), m_Swapchain, nullptr);

    std::cout << "Destroyed SwapChainVk" << std::endl;
}

#pragma region Public API

void SwapChainVk::Submit()
{
    auto uint64_max = std::numeric_limits<uint64_t>::max();

    // -- Waiting for Fences --
    // Wait for given fence to signal open from last draw before continuing
    vkWaitForFences(m_pDevice->GetDevice(),
                    1, &m_DrawFinishedFences[m_CurrentFrameIndex],
                    VK_TRUE, uint64_max);
    // Manually reset (close) the fence
    vkResetFences(m_pDevice->GetDevice(), 1, &m_DrawFinishedFences[m_CurrentFrameIndex]);

    // -- Get Next Image --
    auto result = vkAcquireNextImageKHR(m_pDevice->GetDevice(), m_Swapchain, uint64_max,
                                        m_ImageAvailableForRendering[m_CurrentFrameIndex], VK_NULL_HANDLE,
                                        &m_CurrentImageIndex);

    // -- Submit To Queue --
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_ImageAvailableForRendering[m_CurrentFrameIndex]; // List of semaphores to wait on before running the command buffer on GPU

    // Execute the whole command buffer until the fragment pShader (so we don't output any data to framebuffer yet)
    // and THEN wait for the image to be available (m_ImageAvailable) so fragment pShader can output pixels to it.
    VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.pWaitDstStageMask = waitStages; // Stages to check semaphores at

    // Only submit the command buffer we want to render to. Coz they're 1:1 with SwapChain Images
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentImageIndex];

    // Semaphores to signal when command buffer finishes executing (finished rendering)
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_RenderFinished[m_CurrentFrameIndex]; // 1:1 with frames (0..<=MaxSimultaneousDraws)

    result = vkQueueSubmit(m_pDevice->m_GraphicsQueue, 1, &submitInfo, m_DrawFinishedFences[m_CurrentFrameIndex]);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit Command Buffer to the Graphics Queue!");
    }
}

void SwapChainVk::Present()
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_RenderFinished[m_CurrentFrameIndex];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_Swapchain;
    // Index of images inside the respective swapchains to present. Count is 1:1 with pSwapchains
    presentInfo.pImageIndices = &m_CurrentImageIndex;

    auto result = vkQueuePresentKHR(m_pDevice->m_PresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        std::cout << "Recreating swap chain " << std::endl;
        RecreateSwapChainObjects();
        return;
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present rendered image!");
    }

    // Set to next frame index (loops back to 0)
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxSimultaneousFrameDraws;
}

#pragma endregion

void SwapChainVk::RecreateSwapChainObjects()
{
    vkDeviceWaitIdle(m_pDevice->GetDevice());

    CreateSwapChain();
    CreateFramebuffers();
    m_pRenderContext->ReRecordCommands();
    CreateSyncObjects(); // Recreate sync objects, so they can be set to correct state (signalled or unsignalled)

    std::cout << "Recreated Swap Chain" << std::endl;
}

void SwapChainVk::CreateSwapChain()
{
    auto oldSwapchain = m_Swapchain;

    SurfaceCompatInfo surfaceCompatInfo = m_pDevice->GetSurfaceCompatInfo();

    // Find optimal surface values for swapchain
    VkSurfaceFormatKHR selectedFormat = ChooseBestSurfaceFormat(surfaceCompatInfo.surfaceFormats);
    VkPresentModeKHR selectedPresentMode = ChooseBestPresentationMode(surfaceCompatInfo.presentationModes);
    VkExtent2D extent = ChooseSwapExtent(surfaceCompatInfo.surfaceCapabilities);

    // Add 1 extra image to allow triple buffering
    uint32_t imageCount = surfaceCompatInfo.surfaceCapabilities.minImageCount + 1;

    // If imageCount is higher than max allowed image count. If it's 0 then limitless
    if (surfaceCompatInfo.surfaceCapabilities.maxImageCount > 0 &&
        surfaceCompatInfo.surfaceCapabilities.maxImageCount < imageCount)
    {
        imageCount = surfaceCompatInfo.surfaceCapabilities.maxImageCount;
    }

    m_MaxSimultaneousFrameDraws = imageCount - 1;

    // Swapchain Create Info
    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = m_pDevice->GetSurface();
    swapchainInfo.imageFormat = selectedFormat.format;
    swapchainInfo.imageColorSpace = selectedFormat.colorSpace;
    swapchainInfo.presentMode = selectedPresentMode;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageArrayLayers = 1;       // Number of layers in each image in swap chain
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // What attachment the image will be used as
    // Transform to perform on swap chain images
    swapchainInfo.preTransform = surfaceCompatInfo.surfaceCapabilities.currentTransform;
    // How to handle blending images with external graphics (e.g. other windows)
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // Whether to clip parts of image not in view (e.g. behind an open window, offscreen, etc)
    swapchainInfo.clipped = VK_TRUE;

    QueueFamilyInfo queueFamilies = m_pDevice->GetQueueFamilies();
    if (queueFamilies.graphicsFamilyIndex != queueFamilies.presentationFamilyIndex)
    {
        uint32_t queueFamilyIndices[] = {
                (uint32_t)queueFamilies.graphicsFamilyIndex,
                (uint32_t)queueFamilies.presentationFamilyIndex
        };
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Share image between families
        swapchainInfo.queueFamilyIndexCount = 2;  // No. of queue families that have to share images between each other
        swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Exclusive to a single family
        swapchainInfo.queueFamilyIndexCount = 0;  // 0 because the images are not shared between any families
        swapchainInfo.pQueueFamilyIndices = nullptr; // nullptr because count is 0
    }

    // If old swap chain needs to be destroyed (due to resizing) and this one replaces it,
    // then link the old one to hand over responsibilities
    swapchainInfo.oldSwapchain = oldSwapchain;

    // Create swap chain
    auto result = vkCreateSwapchainKHR(m_pDevice->GetDevice(), &swapchainInfo, nullptr, &m_Swapchain);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Swap Chain!");
    }

    if (oldSwapchain != nullptr)
    {
        vkDestroySwapchainKHR(m_pDevice->GetDevice(), oldSwapchain, nullptr);
        oldSwapchain = nullptr;
    }

    // Store for later reference
    m_SwapchainImageFormat = selectedFormat.format;
    m_SwapchainExtent = extent;

    std::cout << "Swapchain Extent: " << m_SwapchainExtent.width << " ; " << m_SwapchainExtent.height << std::endl;

    uint32_t swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(m_pDevice->GetDevice(), m_Swapchain, &swapchainImageCount, nullptr);
    std::vector<VkImage> images(swapchainImageCount);
    vkGetSwapchainImagesKHR(m_pDevice->GetDevice(), m_Swapchain, &swapchainImageCount, images.data());

    // Destroy old image views if they exist
    if (!m_SwapchainImages.empty())
    {
        vkDeviceWaitIdle(m_pDevice->GetDevice()); // Wait for GPU to go idle to be able to destroy image views
        for (auto& item: m_SwapchainImages)
        {
            if (item.imageView != nullptr)
                vkDestroyImageView(m_pDevice->GetDevice(), item.imageView, nullptr);
            item.imageView = nullptr;
        }
    }

    m_SwapchainImages.clear();

    for (const auto& image: images)
    {
        // Store the image handle & the created image view
        SwapchainImage swapchainImage = {};
        swapchainImage.image = image;
        swapchainImage.imageView = CreateImageView(image, m_SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        // Add to swapchain image list
        m_SwapchainImages.push_back(swapchainImage);
    }
}

void SwapChainVk::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_SwapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // Image data layout before entering RenderPass. Could be any layout, we don't care.
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // Image data layout to change to after the RenderPass. It is used as a source of presentation.
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0; // attachment index in renderPassCreateInfo.pAttachments
    // initialLayout is converted to this layout before passing it to Subpass 0
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Don't care about storing depth after render pass has finished
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    //subpass.pDepthStencilAttachment = &depthAttachmentRef; // TODO: Add Depth attachment

    // Need to determine when layout transitions occur using subpass dependencies
    std::array<VkSubpassDependency, 2> subpassDependencies{};
    // Transition must happen after...
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    // But before...
    subpassDependencies[0].dstSubpass = 0;
    // Wait before outputting the colors to the color attachment until MEMORY_READ is finished from the src.
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = 0;

    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags = 0;

    // RenderPass Create Info
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassInfo.pDependencies = subpassDependencies.data();

    auto result = vkCreateRenderPass(m_pDevice->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create RenderPass!");
    }
}

void SwapChainVk::CreateFramebuffers()
{
    if (!m_SwapchainFramebuffers.empty())
    {
        for (int i = 0; i < m_SwapchainFramebuffers.size(); ++i)
        {
            if (m_SwapchainFramebuffers[i] != nullptr)
            {
                vkDestroyFramebuffer(m_pDevice->GetDevice(), m_SwapchainFramebuffers[i], nullptr);
                m_SwapchainFramebuffers[i] = nullptr;
            }
        }
    }

    m_SwapchainFramebuffers.resize(m_SwapchainImages.size());

    for (int i = 0; i < m_SwapchainFramebuffers.size(); ++i)
    {
        // Make sure the position indices in this array is same as in renderPassCreateInfo.pAttachments array
        std::array<VkImageView, 1> attachments = {
                m_SwapchainImages[i].imageView // Maps to ColorAttachment
                // TODO: Add Depth Attachment image view
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_SwapchainExtent.width;
        framebufferInfo.height = m_SwapchainExtent.height;
        framebufferInfo.layers = 1; // framebuffer layers

        auto result = vkCreateFramebuffer(m_pDevice->GetDevice(), &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Swapchain Framebuffer at index " + std::to_string(i));
        }
    }
}

void SwapChainVk::CreateCommandBuffers()
{
    if (!m_CommandBuffers.empty())
    {
        vkFreeCommandBuffers(m_pDevice->GetDevice(), m_pDevice->GetGraphicsCommandPool(),
                             static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.resize(0);
    }

    m_CommandBuffers.resize(m_SwapchainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_pDevice->GetGraphicsCommandPool();
    // VK_COMMAND_BUFFER_LEVEL_PRIMARY : Buffer you submit to queue (can't be called by other buffers)
    // VK_COMMAND_BUFFER_LEVEL_SECONDARY : Buffer can't be called directly. But can be called by other buffers by vkCmdExecuteCommands(buffer)
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    auto result = vkAllocateCommandBuffers(m_pDevice->GetDevice(), &allocInfo, m_CommandBuffers.data());
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Command Buffers");
    }
}

void SwapChainVk::CreateSyncObjects()
{
    auto device = m_pDevice->GetDevice();

    for (auto &item: m_ImageAvailableForRendering)
    {
        if (item != nullptr)
            vkDestroySemaphore(device, item, nullptr);
        item = nullptr;
    }
    for (auto &item: m_RenderFinished)
    {
        if (item != nullptr)
            vkDestroySemaphore(device, item, nullptr);
        item = nullptr;
    }
    for (auto &item: m_DrawFinishedFences)
    {
        if (item != nullptr)
            vkDestroyFence(device, item, nullptr);
        item = nullptr;
    }

    m_ImageAvailableForRendering.resize(m_MaxSimultaneousFrameDraws);
    m_RenderFinished.resize(m_MaxSimultaneousFrameDraws);
    m_DrawFinishedFences.resize(m_MaxSimultaneousFrameDraws);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create the fence in signaled state

    for (int i = 0; i < m_MaxSimultaneousFrameDraws; ++i)
    {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_ImageAvailableForRendering[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_RenderFinished[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceCreateInfo, nullptr, &m_DrawFinishedFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Semaphores and/or Fences!");
        }
    }
}

#pragma region HELPERS

VkImageView SwapChainVk::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image = image;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // Type of image (1D, 2D, 3D, Cube, etc)
    viewCreateInfo.format = format;

    // Allows remapping of RGBA components to other channel values
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Subresources allow the view to view only a part of an image
    viewCreateInfo.subresourceRange.aspectMask = aspectFlags;  // Which aspect of the image to view (e.g. color bit for viewing color)
    viewCreateInfo.subresourceRange.baseMipLevel = 0;          // Start mipmap level to view from
    viewCreateInfo.subresourceRange.levelCount = 1;            // No. of mipmap levels to view
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;        // Start array layer to view from
    viewCreateInfo.subresourceRange.layerCount = 1;            // No. of array layers to view

    // Create image view and return it
    VkImageView imageView;
    VkResult result = vkCreateImageView(m_pDevice->GetDevice(), &viewCreateInfo, nullptr, &imageView);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create an Image View!");
    }
    return imageView;
}

// Format  : VK_FORMAT_R8G8B8A8_UNORM
// Color space : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR SwapChainVk::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
{
    if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    // RGBA-8 is first preferred format
    for (const auto &surfaceFormat: surfaceFormats)
        if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
            return {surfaceFormat.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

    // BGRA-8 is second preferred format
    for (const auto &surfaceFormat: surfaceFormats)
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
            return {surfaceFormat.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

    return surfaceFormats[0];
}

// Looks for Mailbox presentation mode. If it doesn't exist, it returns FIFO mode which is always available
VkPresentModeKHR SwapChainVk::ChooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes)
{
    if (std::count(presentationModes.begin(), presentationModes.end(), VK_PRESENT_MODE_MAILBOX_KHR))
    {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChainVk::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    // If current extent is at max limit, then we can pick our own extent
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }

    // Pick our own extent
    int width, height;
    SDL_Vulkan_GetDrawableSize(m_pEngine->GetWindow(), &width, &height);

    VkExtent2D newExtent = {};
    newExtent.width = static_cast<uint32_t>(width);
    newExtent.height = static_cast<uint32_t>(height);

    // Surface also defines max & min. So make sure the values are clamped
    newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
    newExtent.height = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

    return newExtent;
}

VkFormat SwapChainVk::FindDepthFormat()
{
    return m_pDevice->FindSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

#pragma endregion
