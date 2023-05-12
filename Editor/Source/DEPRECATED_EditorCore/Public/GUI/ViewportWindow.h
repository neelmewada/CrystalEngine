#pragma once

#include "System.h"

#include <QWindow>

namespace CE::Editor
{
    
    class EDITORCORE_API ViewportWindow : public QWindow
    {
    public:
        ViewportWindow(QWindow* parent = nullptr) : QWindow(parent)
        {
            if (gDynamicRHI == nullptr)
                return;
            if (gDynamicRHI->GetGraphicsBackend() == RHIGraphicsBackend::Vulkan)
                setSurfaceType(QSurface::VulkanSurface);
            else if (gDynamicRHI->GetGraphicsBackend() == RHIGraphicsBackend::DX12)
                setSurfaceType(QSurface::Direct3DSurface);
            else if (gDynamicRHI->GetGraphicsBackend() == RHIGraphicsBackend::Metal)
                setSurfaceType(QSurface::MetalSurface);
        }

        virtual ~ViewportWindow() {}

        void* GetWindowHandle()
        {
            return (void*)winId();
        }


    private:
    };

} // namespace CE::Editor
