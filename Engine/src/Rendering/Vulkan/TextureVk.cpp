
#include "TextureVk.h"

#include "Utils/Image/stb_lib.h"
#include "Utils/Image/stb_image_resize.h"

using namespace Vox;

TextureVk::TextureVk(const TextureCreateInfo& createInfo, DeviceContextVk* pDevice)
{
    m_pDevice = pDevice;
    m_pName = createInfo.pName;
    m_TextureType = createInfo.textureType;

    CreateTexture(createInfo);
    CreateDefaultTextureView(createInfo);
}

TextureVk::~TextureVk()
{
    delete m_pDefaultView;

    // Image
    vmaDestroyImage(m_pDevice->GetVmaAllocator(), m_Image, m_ImageAlloc);
}

void TextureVk::CreateTexture(const TextureCreateInfo& createInfo)
{
    auto width = createInfo.width;
    auto height = createInfo.height;
    auto format = createInfo.format;
    auto uint64_max = std::numeric_limits<uint64_t>::max();

    stbi_uc* pixels = nullptr;
    int texWidth = 0, texHeight = 0, texChannels = 0;

    if (createInfo.pImageData != nullptr && createInfo.imageDataSize > 0)
    {
        pixels = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(createInfo.pImageData), static_cast<int>(createInfo.imageDataSize), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        VOX_ASSERT(pixels != nullptr, "Failed to load STB image from memory buffer!");

        width = static_cast<Uint32>(texWidth);
        height = static_cast<Uint32>(texHeight);

        if (texChannels == 1) format = IMG_FORMAT_R8_SRGB;
        else if (texChannels == 2) format = IMG_FORMAT_R8G8_SRGB;
        else if (texChannels == 3) format = IMG_FORMAT_R8G8B8_SRGB;
        else if (texChannels == 4) format = IMG_FORMAT_R8G8B8A8_SRGB;
        else throw std::runtime_error("Failed to fetch format from STB Image! Invalid no. of texChannels: " + std::to_string(texChannels));
    }

    // Store important values for later use
    m_Format = format;
    m_MipLevels = createInfo.mipLevels;

    Uint64 imageSize = static_cast<Uint64>(width * height * 4);

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent = VkExtent3D{width, height, 1};
    imageCreateInfo.mipLevels = createInfo.mipLevels;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageCreateInfo.arrayLayers = 1;

    // No sharing between queues
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;

    // Image Tiling, i.e. Layout of image in memory
    if (createInfo.textureLayout == TEXTURE_LAYOUT_LINEAR) // Texels are laid out linearly in memory like a pixel array
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    else if (createInfo.textureLayout == TEXTURE_LAYOUT_OPTIMAL) // Texels are laid out
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    imageCreateInfo.format = static_cast<VkFormat>(format);
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = static_cast<VmaMemoryUsage>(createInfo.allocType);

    VK_ASSERT(vmaCreateImage(m_pDevice->GetVmaAllocator(), &imageCreateInfo, &allocInfo,&m_Image, &m_ImageAlloc, nullptr),
              "Failed to create image " + std::string(createInfo.pName) + " using vmaCreateImage!");

    m_ImageLayout = imageCreateInfo.initialLayout;

    if (pixels != nullptr)
    {
        BufferCreateInfo stagingBufferInfo = {};
        stagingBufferInfo.pName = "Texture Staging Buffer";
        stagingBufferInfo.size = imageSize;
        stagingBufferInfo.allocType = DEVICE_MEM_CPU_ONLY;
        stagingBufferInfo.bindFlags = BIND_STAGING_BUFFER;
        stagingBufferInfo.transferFlags = BUFFER_TRANSFER_SRC_BIT;
        stagingBufferInfo.optimizationFlags = BUFFER_OPTIMIZE_TEMPORARY_BIT;

        BufferData stagingBufferData = {};
        stagingBufferData.dataSize = imageSize;
        stagingBufferData.pData = pixels;

        BufferVk* stagingBuffer = new BufferVk(stagingBufferInfo, stagingBufferData, m_pDevice);

        // -- Create Vulkan Objects --
        VkFence uploadFence = nullptr;
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VK_ASSERT(vkCreateFence(m_pDevice->GetDevice(), &fenceCreateInfo, nullptr, &uploadFence),
                  "Failed to Create Upload VkFence for a GPU-ONLY Buffer transfer!");

        VkCommandPool uploadCmdPool = nullptr;
        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = m_pDevice->GetQueueFamilies().graphicsFamilyIndex;

        VK_ASSERT(vkCreateCommandPool(m_pDevice->GetDevice(), &cmdPoolInfo, nullptr, &uploadCmdPool),
                  "Failed to create Upload CommandPool for a GPU-ONLY Buffer transfer!");

        VkCommandBuffer uploadCmdBuffer = nullptr;
        VkCommandBufferAllocateInfo cmdBufferInfo = {};
        cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufferInfo.commandBufferCount = 1;
        cmdBufferInfo.commandPool = uploadCmdPool;

        VK_ASSERT(vkAllocateCommandBuffers(m_pDevice->GetDevice(), &cmdBufferInfo, &uploadCmdBuffer),
                  "Failed to allocate Upload CommandBuffer for a GPU-ONLY Texture Buffer transfer!");

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_ASSERT(vkBeginCommandBuffer(uploadCmdBuffer, &beginInfo),
                  "Failed to Begin Command Buffer for a GPU-ONLY Texture Buffer transfer!");

        // Transition Image Layout to TRANSFER_DST
        TransitionImageLayout(uploadCmdBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Copy Buffer To Image
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferImageHeight = 0;
        region.bufferRowLength = 0;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0; // The current mip level to copy
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        vkCmdCopyBufferToImage(uploadCmdBuffer, stagingBuffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        // Transition Image Layout to SHADER_READ
        TransitionImageLayout(uploadCmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VK_ASSERT(vkEndCommandBuffer(uploadCmdBuffer),
                  "Failed to End Command Buffer for a GPU-ONLY Texture Buffer transfer!");

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &uploadCmdBuffer;
        VK_ASSERT(vkQueueSubmit(m_pDevice->GetGraphicsQueue(), 1, &submitInfo, uploadFence),
                  "Failed to submit Texture Copy Commands to Graphics Queue!");

        vkWaitForFences(m_pDevice->GetDevice(), 1, &uploadFence, VK_TRUE, uint64_max);
        vkResetFences(m_pDevice->GetDevice(), 1, &uploadFence);

        // Store image layout
        m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        // Destroy UploadCommandPool
        vkDestroyCommandPool(m_pDevice->GetDevice(), uploadCmdPool, nullptr);
        // Destroy UploadFence
        vkDestroyFence(m_pDevice->GetDevice(), uploadFence, nullptr);
        // Destroy Staging Buffer
        delete stagingBuffer;

        stbi_image_free(pixels);
        pixels = nullptr;
    }
}

void TextureVk::CreateDefaultTextureView(const TextureCreateInfo& createInfo)
{
    TextureViewCreateInfo textureViewInfo = {};
    textureViewInfo.textureType = m_TextureType;
    textureViewInfo.image = m_Image;
    textureViewInfo.format = static_cast<VkFormat>(m_Format);
    textureViewInfo.mipLevels = m_MipLevels;
    textureViewInfo.imageLayout = m_ImageLayout;

    m_pDefaultView = new TextureViewVk(textureViewInfo, m_pDevice);
}

void TextureVk::TransitionImageLayout(VkCommandBuffer cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_MipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition in TextureVk named: " + std::string(m_pName));
    }

    vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage,0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
}

Uint32 TextureVk::GetNumOfBytesPerPixel()
{
    Uint32 formatInt = static_cast<Uint32>(m_Format);

    if (formatInt >= 37 && formatInt <= 57) return 4;
    if (formatInt >= 23 && formatInt <= 36) return 3;
    if (formatInt >= 16 && formatInt <= 22) return 2;
    if (formatInt >= 9 && formatInt <= 15) return 1;

    throw std::runtime_error("Invalid Image Format value: " + std::to_string(m_Format) + " in Texture named " + m_pName + "!");
}
