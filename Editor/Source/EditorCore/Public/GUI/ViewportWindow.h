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
            setSurfaceType(QSurface::VulkanSurface);
        }

        virtual ~ViewportWindow() {}

        void* GetWindowHandle()
        {
            return (void*)winId();
        }


    private:
    };

} // namespace CE::Editor
