#include "CrystalEditor.h"

namespace CE::Editor
{
    static SampleWidgetWindow* instance = nullptr;

    SampleWidgetModel::SampleWidgetModel()
    {
        m_ItemList = {
			"Item 0",
			"Item 1"
        };
    }

    void SampleWidgetModel::OnModelPropertyEdited(const CE::Name& propertyName, Object* modifyingObject)
    {
	    Super::OnModelPropertyEdited(propertyName, modifyingObject);
    }

    void SampleWidgetModel::ModifyTextInCode()
    {
        SetText(String::Format("Random {}", Random::Range(0, 100)));
    }

    SampleWidgetWindow::SampleWidgetWindow()
    {
        model = CreateDefaultSubobject<SampleWidgetModel>("Model");
    }

    void SampleWidgetWindow::Construct()
    {
        Super::Construct();

        titleBar->Height(30);
        titleBarLabel->FontSize(13);

        constexpr auto splashImage = "/Editor/Assets/Images/Splash";
        constexpr auto gridImage = "/Editor/Assets/Images/GridSmall";

        instance = this;

        FBrush splash = FBrush(splashImage);
        splash.SetBrushTiling(FBrushTiling::TileXY);
        splash.SetImageFit(FImageFit::Contain);
        splash.SetBrushPosition(Vec2(0.5f, 0.5f)); // 50%, 50%
        splash.SetBrushSize(Vec2(256, 144));

        FBrush grid = FBrush(gridImage);
        grid.SetBrushTiling(FBrushTiling::TileXY);
        grid.SetBrushSize(Vec2(20, 20));


        (*this)
            .Title("Fusion Samples")
            .MinimizeEnabled(false)
            .MaximizeEnabled(false)
            .ContentPadding(Vec4(1, 1, 1, 1) * 20)
			.ContentGap(5)
            .Content(
                FNew(FImage)
                .Background(splash)
                .BackgroundShape(FRoundedRectangle(5, 10, 15, 20))
                .HAlign(HAlign::Fill)
                .Height(300)
                .Margin(Vec4(0, 0, 0, 10)),

                FNew(FLabel)
                .FontSize(13)
                .Text("Version: " CE_ENGINE_VERSION_STRING_SHORT " pre-alpha [Build " CE_TOSTRING(CE_VERSION_BUILD) "]")
                .HAlign(HAlign::Left),

                FNew(FSplitBox)
                .ContentVAlign(VAlign::Center)
                .HAlign(HAlign::Fill)
                .Name("DebugSplitBox")
                (
                    FNew(FStyledWidget)
                    .Background(Color::Yellow())
                    .Height(25)
                    .FillRatio(0.25f),

                    FNew(FStyledWidget)
                    .Background(Color::Green())
                    .Height(25)
                    .FillRatio(0.25f),

                    FNew(FStyledWidget)
                    .Background(Color::Cyan())
                    .Height(25)
                    .FillRatio(0.5f)
                ),

                FNew(FOverlayStack)
                .ContentVAlign(VAlign::Center)
                .HAlign(HAlign::Fill)
                (
                    FNew(FStyledWidget)
                    .Background(grid)
                    .Height(40),

                    FNew(FLabel)
                    .Text("This is overlay text")
                    .FontSize(18)
                    .Bold(true)
                    .Foreground(Color::Black())
                    .HAlign(HAlign::Center)
                ),

                FNew(FHorizontalStack)
                .ContentVAlign(VAlign::Center)
                .ContentHAlign(HAlign::Left)
                (
                    FNew(FTextButton)
                    .Text("Add Item")
                    .OnClicked([this]
                    {
                        Array<String> list = model->GetItemList();
                        list.Add(String::Format("Item {}", list.GetSize()));
                        model->SetItemList(list);
                    }),

                    FNew(FComboBox)
                    .Bind_Items(BIND_PROPERTY_R(model, ItemList))
                    .MinWidth(60)
                ),

                FNew(FHorizontalStack)
                .ContentVAlign(VAlign::Center)
                .ContentHAlign(HAlign::Left)
                (
                    FNew(FTextInput)
                    .Bind_Text(BIND_PROPERTY_RW(model, Text))
                    .FontSize(13)
                    .Width(180)
                    .Margin(Vec4(0, 0, 10, 0)),

                    FNew(FTextButton)
                    .FontSize(13)
                    .Text("Randomize")
                    .OnClicked([this]
                        {
                            model->ModifyTextInCode();
                        }),

                    FNew(FLabel)
                    .FontSize(13)
                    .Bind_Text(BIND_PROPERTY_R(model, Text))
                ),

                FNew(FScrollBox)
                .VerticalScroll(true)
                .HorizontalScroll(true)
                .FillRatio(1.0f)
                (
                    FNew(FVerticalStack)
                    .VAlign(VAlign::Top)
                    .HAlign(HAlign::Left)
                    (
                        FNew(FLabel)
                        .Text("The quick brown fox jumps over the lazy dog, showcasing every letter in the English alphabet.")
                        .FontSize(16),

                        FNew(FLabel)
                        .Text("Beneath the starlit sky, a lively fox darted over fallen leaves, leaving soft echoes in the quiet woods.")
                        .FontSize(16),

                        FNew(FLabel)
                        .Text("The curious red fox dashed through the misty forest, weaving between ancient trees with nimble grace.")
                        .FontSize(16),

                        FNew(FLabel)
                        .Text("With a flick of its tail, the agile fox bounded across the meadow, vanishing into the evening shadows.")
                        .FontSize(16)
                    )
                )
            );
    }

    void SampleWidgetWindow::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (this == instance)
        {
            instance = nullptr;
        }
    }

    SampleWidgetWindow* SampleWidgetWindow::Show()
    {
        if (instance != nullptr)
        {
            FNativeContext* nativeContext = static_cast<FNativeContext*>(instance->GetContext());
            PlatformWindow* window = nativeContext->GetPlatformWindow();
            window->SetAlwaysOnTop(true);
            window->Show();
            return instance;
        }

        PlatformWindowInfo info{
            .maximised = false,
            .fullscreen = false,
            .resizable = false,
            .hidden = false,
            .windowFlags = PlatformWindowFlags::Utility | PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::SkipTaskbar
        };

        PlatformWindow* window = PlatformApplication::Get()->CreatePlatformWindow("SampleWidgets", 600, 600, info);
        window->SetAlwaysOnTop(true);
        window->SetBorderless(true);

        FRootContext* rootContext = FusionApplication::Get()->GetRootContext();
        FFusionContext* parentContext = rootContext;
        if (rootContext->GetChildContexts().NonEmpty())
        {
            // FRootContext should have only 1 NativeContext which is the primary Native Window
            parentContext = rootContext->GetChildContexts().GetFirst();
        }

        FNativeContext* context = FNativeContext::Create(window, "SampleWidgetWindow", parentContext);
        parentContext->AddChildContext(context);

        SampleWidgetWindow* sampleWindow = nullptr;

        FAssignNewOwned(SampleWidgetWindow, sampleWindow, context);
        context->SetOwningWidget(sampleWindow);

        return sampleWindow;
    }
}

