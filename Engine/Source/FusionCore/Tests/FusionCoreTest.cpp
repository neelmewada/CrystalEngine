#include "FusionCoreTest.h"


namespace RenderingTests
{


#pragma region Renderer System

	void RendererSystem::Init()
	{
        PlatformApplication::Get()->AddMessageHandler(this);
	}

	void RendererSystem::Shutdown()
	{
        PlatformApplication::Get()->RemoveMessageHandler(this);
	}

    void RendererSystem::Render()
    {
        FusionApplication* app = FusionApplication::TryGet();

        int submittedImageIndex = -1;

        if (app)
        {
            app->Tick();
        }

        if (IsEngineRequestingExit())
        {
            return;
        }

        if (rebuildFrameGraph)
        {
            rebuildFrameGraph = false;
            recompileFrameGraph = true;

            BuildFrameGraph();
            submittedImageIndex = curImageIndex;
        }

        if (recompileFrameGraph)
        {
            recompileFrameGraph = false;

            CompileFrameGraph();
        }

        if (IsEngineRequestingExit())
        {
            return;
        }

        auto scheduler = FrameScheduler::Get();

        int imageIndex = scheduler->BeginExecution();

        if (imageIndex >= RHI::Limits::MaxSwapChainImageCount || rebuildFrameGraph || recompileFrameGraph)
        {
            rebuildFrameGraph = recompileFrameGraph = true;
            return;
        }

        curImageIndex = imageIndex;

        // ---------------------------------------------------------
        // - Enqueue draw packets to views

        if (submittedImageIndex != curImageIndex)
        {
            RPI::RPISystem::Get().SimulationTick(curImageIndex);
            RPI::RPISystem::Get().RenderTick(curImageIndex);
        }

        // ---------------------------------------------------------
        // - Submit draw lists to scopes for execution

        drawList.Shutdown();

        RHI::DrawListMask drawListMask{};
        HashSet<RHI::DrawListTag> drawListTags{};

        // - Setup draw list mask

        if (app)
        {
            app->UpdateDrawListMask(drawListMask);
        }

        // - Enqueue additional draw packets

        for (int i = 0; i < drawListMask.GetSize(); ++i)
        {
            if (drawListMask.Test(i))
            {
                drawListTags.Add((u8)i);
            }
        }

        drawList.Init(drawListMask);

        if (app)
        {
            app->EnqueueDrawPackets(drawList, curImageIndex);
        }

        drawList.Finalize();

        // - Set scope draw lists

        if (app) // FWidget Scopes & DrawLists
        {
            app->FlushDrawPackets(drawList, curImageIndex);
        }


        scheduler->EndExecution();
    }

    void RendererSystem::RebuildFrameGraph()
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::BuildFrameGraph()
    {
        rebuildFrameGraph = false;
        recompileFrameGraph = true;

        RPI::RPISystem::Get().SimulationTick(curImageIndex);
        RPI::RPISystem::Get().RenderTick(curImageIndex);

        auto scheduler = RHI::FrameScheduler::Get();

        RHI::FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

        scheduler->BeginFrameGraph();
        {
            auto app = FusionApplication::TryGet();

            if (app)
            {
                app->EmplaceFrameAttachments();

                app->EnqueueScopes();
            }
        }
        scheduler->EndFrameGraph();
    }

    void RendererSystem::CompileFrameGraph()
    {
        recompileFrameGraph = false;

        auto scheduler = RHI::FrameScheduler::Get();

        scheduler->Compile();

        RHI::TransientMemoryPool* pool = scheduler->GetTransientPool();
        RHI::MemoryHeap* imageHeap = pool->GetImagePool();
    }


    void RendererSystem::OnWindowRestored(PlatformWindow* window)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::OnWindowDestroyed(PlatformWindow* window)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::OnWindowClosed(PlatformWindow* window)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::OnWindowMinimized(PlatformWindow* window)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::OnWindowCreated(PlatformWindow* window)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSystem::OnWindowExposed(PlatformWindow* window)
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

#pragma endregion

    void RenderingTestWidget::Construct()
    {
        Super::Construct();

        FBrush transparentPattern = FBrush("/Engine/Resources/Icons/TransparentPattern", Color::White());
        transparentPattern.SetBrushTiling(FBrushTiling::TileXY);
        transparentPattern.SetBrushSize(Vec2(16, 16));

        FButton* openPopupBtn = nullptr;
        FTextButton* nativePopupBtn = nullptr;

        PlatformApplication::Get()->AddMessageHandler(this);

        Child(
            FAssignNew(FStyledWidget, borderWidget)
            .Background(FBrush(Color::RGBA(36, 36, 36)))
            .BorderWidth(1.0f)
            .BorderColor(Color::RGBA(15, 15, 15))
            .Padding(Vec4(1, 1, 1, 1))
            .Name("RootStyle")
            (
                FAssignNew(FVerticalStack, rootBox)
                .ContentHAlign(HAlign::Fill)
                .Name("RootBox")
                (
                    FNew(FTitleBar)
                    .Background(Color::RGBA(26, 26, 26))
                    .Height(40)
                    .HAlign(HAlign::Fill)
                    (
                        FNew(FOverlayStack)
                        .VAlign(VAlign::Fill)
                        .HAlign(HAlign::Fill)
                        (
                            FNew(FHorizontalStack)
                            .ContentVAlign(VAlign::Center)
                            .HAlign(HAlign::Fill)
                            .VAlign(VAlign::Fill)
                            (
                                FNew(FWidget)
                                .FillRatio(1.0f),

                                FNew(FLabel)
                                .FontSize(15)
                                .Text("Widget Demo")
                                .HAlign(HAlign::Center)
                                .VAlign(VAlign::Center),

                                FNew(FWidget)
                                .FillRatio(1.0f)
                            ),

                            FNew(FHorizontalStack)
                            .HAlign(HAlign::Fill)
                            .VAlign(VAlign::Fill)
                            (
                                FNew(FWidget)
                                .FillRatio(1.0f),

                                FNew(FButton)
                                .OnClicked([this]
                                    {
                                        static_cast<FNativeContext*>(GetContext())->Minimize();
                                    })
                                .Padding(Vec4(17, 8, 17, 8))
                                .Name("WindowMinimizeButton")
                                .Style("Button.WindowControl")
                                .VAlign(VAlign::Top)
                                (
                                    FNew(FImage)
                                    .Background(FBrush("/Engine/Resources/Icons/MinimizeIcon"))
                                    .Width(11)
                                    .Height(11)
                                    .HAlign(HAlign::Center)
                                    .VAlign(VAlign::Center)
                                ),

                                FNew(FButton)
                                .OnClicked([this]
                                    {
                                        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());
                                        if (nativeContext->IsMaximized())
                                        {
                                            nativeContext->Restore();
                                        }
                                        else
                                        {
                                            nativeContext->Maximize();
                                        }
                                    })
                                .Padding(Vec4(17, 8, 17, 8))
                                .Name("WindowMaximizeButton")
                                .Style("Button.WindowControl")
                                .VAlign(VAlign::Top)
                                (
                                    FAssignNew(FImage, maximizeIcon)
                                    .Background(FBrush("/Engine/Resources/Icons/MaximizeIcon"))
                                    .Width(11)
                                    .Height(11)
                                    .HAlign(HAlign::Center)
                                    .VAlign(VAlign::Center)
                                ),

                                FNew(FButton)
                                .OnClicked([this]
                                    {
                                        RequestEngineExit("USER_QUIT");
                                    })
                                .Padding(Vec4(18, 8, 18, 8))
                                .Name("WindowCloseButton")
                                .Style("Button.WindowClose")
                                .VAlign(VAlign::Top)
                                (
                                    FNew(FImage)
                                    .Background(FBrush("/Engine/Resources/Icons/CrossIcon"))
                                    .Width(10)
                                    .Height(10)
                                    .HAlign(HAlign::Center)
                                    .VAlign(VAlign::Center)
                                )
                            )
                        )
                    ),

                    // Window Content Begins

                    FAssignNew(FVerticalStack, windowContent)
                    .Padding(Vec4(10, 10, 10, 10))
                    .Name("ContentVStack")
                    (
                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Fill)
                        .Name("HStack1")
                        (
                            FNew(FStyledWidget)
                            .Background(transparentPattern)
                            .BackgroundShape(FRoundedRectangle(2.5f, 5, 7.5f, 10))
                            .FillRatio(1.0f)
                            .MinWidth(60)
                            .MinHeight(30)
                        )
                        .Margin(Vec4(0, 0, 0, 5)),

                        FAssignNew(FButton, button)
                        .OnClicked([this]
                            {
                                buttonLabel->Text(String::Format("Click Count {}", ++hitCounter));
                            })
                        .Name("Button")
                        (
                            FAssignNew(FLabel, buttonLabel)
                            .FontSize(13)
                            .Text("Click Count 0")
                        ),

                        FAssignNew(FButton, openPopupBtn)
                        .OnClicked([this, openPopupBtn]
                            {
                                GetContext()->PushLocalPopup(btnPopup, openPopupBtn->GetGlobalPosition() + Vec2(0, openPopupBtn->GetComputedSize().y));
                            })
                        .Name("PopupButton")
                        (
                            FNew(FLabel)
                            .FontSize(13)
                            .Text("Open Popup")
                        ),

                        FAssignNew(FTextButton, nativePopupBtn)
                        .Text("Open Native Popup")
                        .OnClicked([this, nativePopupBtn]
                            {
                                GetContext()->PushNativePopup(nativePopup, nativePopupBtn->GetGlobalPosition() + Vec2(0, nativePopupBtn->GetComputedSize().y));
                            })
                        .Name("NativePopupButton"),

                        FAssignNew(FTextInput, textInput)
                        .Text("This is a very long text box")
                        .Style("TextInput.Primary")
                        .MaxWidth(120)
                        .HAlign(HAlign::Left)
                        .Margin(Vec4(0, 0, 0, 5)),

                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Center)
                        .Name("HStack2")
                        (
                            FAssignNew(FStyledWidget, subWidget)
                            .Background(FBrush(Color::Green()))
                            .BackgroundShape(FRectangle())
                            .FillRatio(1.0f)
                            .MinWidth(60)
                            .MinHeight(15),

                            FNew(FStyledWidget)
                            .Background(FBrush(Color::Cyan()))
                            .BackgroundShape(FRectangle())
                            .FillRatio(2.0f)
                            .MinWidth(60)
                            .MinHeight(40),

                            FNew(FStyledWidget)
                            .Background(FBrush(Color::Yellow()))
                            .BackgroundShape(FRectangle())
                            .FillRatio(1.0f)
                            .MinWidth(60)
                            .MinHeight(25)
                        ),

                        FNew(FLabel)
                        .FontSize(18)
                        .Text("Demo Title")
                        .HAlign(HAlign::Left)

                    )
                )
            )
        );

        //windowContent->Enabled(false);
    }

    void RenderingTestWidget::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        PlatformApplication::Get()->RemoveMessageHandler(this);
    }

    void RenderingTestWidget::OnWindowRestored(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window)
        {
            //maximizeIcon->Background(FBrush("/Engine/Resources/Icons/MaximizeIcon"));
            this->Padding(Vec4());
        }
    }

    void RenderingTestWidget::OnWindowMaximized(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window)
        {
            //maximizeIcon->Background(FBrush("/Engine/Resources/Icons/RestoreIcon"));
            this->Padding(Vec4(1, 1, 1, 1) * 4.0f);
        }
    }

    void RenderingTestWidget::OnWindowExposed(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window && !window->IsMaximized())
        {
            OnWindowRestored(window);
        }
    }

}
