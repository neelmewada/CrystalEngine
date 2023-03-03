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
        
        //instance = new QVulkanInstance();

        //VkInstance vkInstance = nullptr;
        //gDynamicRHI->GetNativeHandle((void**)&vkInstance);

        //instance->setVkInstance(vkInstance);
    }

    ViewportView::~ViewportView()
    {
        //delete instance;
        delete ui;
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, ViewportView)
