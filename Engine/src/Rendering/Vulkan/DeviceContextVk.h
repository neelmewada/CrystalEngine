#pragma once

#include "Rendering/EngineFactoryVk.h"
#include "Rendering/IDeviceContext.h"
#include "EngineContextVk.h"
#include "BufferVk.h"
#include "TypesVk.h"

#include <SDL2/SDL.h>

#include <stdint.h>
#include <vector>

#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

namespace Vox
{

class DeviceContextVk : public IDeviceContext
{
private:
    friend class EngineFactoryVk;
    friend class SwapChainVk;
    DeviceContextVk(DeviceCreateInfoVk& deviceInfo);
    ~DeviceContextVk();

public:
    DeviceContextVk(const DeviceContextVk&) = delete;

public:
    // - Getters
    QueueFamilyInfo GetQueueFamilies() { return m_QueueFamilies; }
    SurfaceCompatInfo GetSurfaceCompatInfo() { return FetchSurfaceCompatInfo(m_PhysicalDevice); }
    VkSurfaceKHR GetSurface() { return m_Surface; }
    VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
    VkDevice GetDevice() { return m_Device; }
    VkCommandPool GetGraphicsCommandPool() { return m_GraphicsCommandPool; }
    VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() { return m_PresentQueue; }
    bool IsPortabilitySubsetEnabled() { return m_PortabilitySubsetEnabled; }
    VmaAllocator GetVmaAllocator() { return m_Allocator; }

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

public: // Public API
    void WaitUntilIdle() override { vkDeviceWaitIdle(m_Device); }
    IGraphicsPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& createInfo) override;
    IShader* CreateShader(const ShaderCreateInfo& createInfo) override;
    IBuffer* CreateBuffer(const BufferCreateInfo& createInfo, BufferData& bufferData) override;
    ITexture * CreateTexture(const TextureCreateInfo &createInfo) override;

    // - Getters
    // -- Support Info
    Uint64 GetUniformBufferOffsetAlignment() override { return m_UniformBufferOffsetAlignment; }

private: // Internal API
    // - Vulkan Main
    void CreateSurface();
    void FetchPhysicalDevice();
    void FetchSupportInfo();
    void CreateLogicalDevice();
    void CreateVmaAllocator();
    void CreateCommandPool();

    // - Supports
    bool CheckPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
    bool CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    VkDeviceSize GetPhysicalDeviceLocalMemory(VkPhysicalDevice physicalDevice);
    QueueFamilyInfo FetchQueueFamilies(VkPhysicalDevice physicalDevice);
    SurfaceCompatInfo FetchSurfaceCompatInfo(VkPhysicalDevice physicalDevice);

private: // Internal Members
    EngineContextVk* m_EngineContext = nullptr;
    QueueFamilyInfo m_QueueFamilies;
    // - Support Info
    SurfaceCompatInfo m_SurfaceCompatInfo;
    bool m_PortabilitySubsetEnabled = false;
    Uint64 m_UniformBufferOffsetAlignment;

private: // Vulkan Members
    VkSurfaceKHR m_Surface = nullptr;
    VkPhysicalDevice m_PhysicalDevice = nullptr;
    VkDevice m_Device = nullptr;
    VkQueue m_GraphicsQueue = nullptr;
    VkQueue m_PresentQueue = nullptr;
    VmaAllocator m_Allocator = nullptr;
    VkCommandPool m_GraphicsCommandPool = nullptr;
};

}
