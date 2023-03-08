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
        delete ui;
    }

    void ViewportView::showEvent(QShowEvent* event)
    {
        Super::showEvent(event);

        RHIRenderTargetLayout rtLayout{};
        rtLayout.numColorOutputs = 1;
        rtLayout.presentationRTIndex = -1;

        rtLayout.colorOutputs[0].sampleCount = 1;
        rtLayout.colorOutputs[0].preferredFormat = RHIColorFormat::Auto;
        rtLayout.colorOutputs[0].loadAction = RHIRenderPassLoadAction::Clear;
        rtLayout.colorOutputs[0].storeAction = RHIRenderPassStoreAction::Store;

        rtLayout.depthStencilFormat = RHIDepthStencilFormat::None;

        rtLayout.backBufferCount = 2;
        rtLayout.simultaneousFrameDraws = 1;

        //renderTarget = gDynamicRHI->CreateRenderTarget(width(), height(), rtLayout);

        auto id = (void*)winId();

        QWindow* window = this->windowHandle();
        id = (void*)window->winId();
        
        viewportRHI = gDynamicRHI->CreateViewport(id, width(), height(), isFullScreen(), rtLayout);
    }

    void ViewportView::hideEvent(QHideEvent* event)
    {
        Super::hideEvent(event);

        gDynamicRHI->DestroyViewport(viewportRHI);
        viewportRHI = nullptr;
        //gDynamicRHI->DestroyRenderTarget(renderTarget);
        //renderTarget = nullptr;
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, ViewportView)
