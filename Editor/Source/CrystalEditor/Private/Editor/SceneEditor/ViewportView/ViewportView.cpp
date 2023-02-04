#include "ViewportView.h"
#include "ui_ViewportView.h"

#include "RHI/RHI.h"

struct VkInstance_T;
typedef VkInstance_T* VkInstance;

namespace CE::Editor
{

    ViewportView::ViewportView(QWidget* parent)
        : EditorViewBase(parent)
        , ui(new Ui::ViewportView)
    {
        ui->setupUi(this);

        setWindowTitle("Viewport");
        //windowHandle()->setSurfaceType(QSurface::VulkanSurface);
        
        instance = new QVulkanInstance();

        VkInstance vkInstance = nullptr;
        MBUS_EVENT(RHIBus, GetNativeHandle, (void**)&vkInstance);

        instance->setVkInstance(vkInstance);
    }

    ViewportView::~ViewportView()
    {
        //delete instance;
        delete ui;
    }

}
