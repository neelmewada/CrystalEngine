#include "ViewportView.h"
#include "ui_ViewportView.h"


#include "RHI/RHI.h"

#include <QTimer>

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

        renderViewport = new ViewportWindow;
        renderViewportWidget = QWidget::createWindowContainer(renderViewport);
        layout()->addWidget(renderViewportWidget);
        
        //instance = new QVulkanInstance();
        //VkInstance vkInstance = (VkInstance)gDynamicRHI->GetNativeHandle();
        //instance->setVkInstance(vkInstance);

        //vulkanWindow = new QVulkanWindow;
        //vulkanWindow->setVulkanInstance(instance);

        //layout()->addWidget(QWidget::createWindowContainer(vulkanWindow));
    }

    ViewportView::~ViewportView()
    {
        delete renderViewport;
        delete ui;
    }

    void ViewportView::showEvent(QShowEvent* event)
    {
        Super::showEvent(event);

        RHIRenderTargetLayout rtLayout{};
        rtLayout.numColorOutputs = 1;
        rtLayout.presentationRTIndex = 0;

        rtLayout.colorOutputs[0].sampleCount = 1;
        rtLayout.colorOutputs[0].preferredFormat = RHIColorFormat::Auto;
        rtLayout.colorOutputs[0].loadAction = RHIRenderPassLoadAction::Clear;
        rtLayout.colorOutputs[0].storeAction = RHIRenderPassStoreAction::Store;

        rtLayout.depthStencilFormat = RHIDepthStencilFormat::Auto;

        rtLayout.backBufferCount = 2;
        rtLayout.simultaneousFrameDraws = 1;

        renderViewport->show();

        viewportRHI = gDynamicRHI->CreateViewport(renderViewport->GetWindowHandle(), width(), height(), isFullScreen(), rtLayout);
        viewportRHI->SetClearColor(Color::Blue());

        cmdList = gDynamicRHI->CreateGraphicsCommandList(viewportRHI);

        QTimer::singleShot(2000, this, &ViewportView::OnRenderLoop);
    }

    void ViewportView::hideEvent(QHideEvent* event)
    {
        Super::hideEvent(event);

        renderViewport->hide();

        gDynamicRHI->DestroyCommandList(cmdList);
        cmdList = nullptr;

        gDynamicRHI->DestroyViewport(viewportRHI);
        viewportRHI = nullptr;
    }

    void ViewportView::OnRenderLoop()
    {
        CE_LOG(Info, All, "Render Loop");

        cmdList->Begin();

        cmdList->End();

        gDynamicRHI->ExecuteCommandList(cmdList);
        gDynamicRHI->PresentViewport(cmdList);

        if (!stopRenderLoop)
        {
            QTimer::singleShot(1000, this, [this]() -> void
            {
                CE_LOG(Info, All, "Update!");
            });
        }
    }
}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, ViewportView)
