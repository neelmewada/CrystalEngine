#pragma once

#include <QVulkanWindow>

namespace CE
{
    class VulkanRHI;
}

namespace CE::Editor
{
    class RenderViewport;
    class VulkanViewportRenderer;

    class VulkanViewportRenderer : public QVulkanWindowRenderer
    {
    public:
        VulkanViewportRenderer(QVulkanWindow* w);
        virtual ~VulkanViewportRenderer() = default;

        void initResources() override;
        void initSwapChainResources() override;
        void releaseSwapChainResources() override;
        void releaseResources() override;

        void startNextFrame() override;

    private:
        VulkanRHI* vulkanRHI = nullptr;
        QVulkanWindow* window = nullptr;
    };

    class RenderViewport : public QVulkanWindow
    {
    public:
        RenderViewport(QWindow* parent = nullptr);
        virtual ~RenderViewport();

        QVulkanWindowRenderer* createRenderer() override;

    private:
        QVulkanInstance* instance = nullptr;
    };
    
} // namespace CE::Editor
