
#include "FusionCore.h"
#include "VulkanRHI.h"

#include "../../Utils/clipper.svg.h"
#include "../../Utils/ClipFileLoad.h"
#include "../../Utils/clipper.svg.utils.h"
#include "../../Utils/Colors.h"
#include "../../Utils/Timer.h"

#include "FusionCoreTest.h"

#include <gtest/gtest.h>

#include "FusionCore_Test.private.h"

#define TEST_BEGIN TestBegin(false)
#define TEST_END TestEnd(false)

#define TEST_BEGIN_GUI TestBegin(true)
#define TEST_END_GUI TestEnd(true)

static CE::JobManager* gJobManager = nullptr;
static CE::JobContext* gJobContext = nullptr;

using namespace CE;

static void TestBegin(bool gui)
{
	gProjectPath = PlatformDirectories::GetLaunchDir();
	gProjectName = MODULE_NAME;

	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");
	ModuleManager::Get().LoadModule("CoreInput");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreShader");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("VulkanRHI");
	ModuleManager::Get().LoadModule("CoreRPI");
	ModuleManager::Get().LoadModule("FusionCore");
	CERegisterModuleTypes();

	PlatformApplication* app = PlatformApplication::Get();

	app->Initialize();

	if (gui)
	{
		PlatformWindowInfo windowInfo{};
		windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
		windowInfo.resizable = true;
		windowInfo.hidden = true;
		windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

		u32 w = 1024, h = 768;
#if PLATFORM_MAC
		w = 540; h = 400;
#endif

		PlatformWindow* window = app->InitMainWindow("MainWindow", w, h, windowInfo);
		window->SetBorderless(true);

		InputManager::Get().Initialize(app);
	}

	RHI::gDynamicRHI = new Vulkan::VulkanRHI();

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();

	if (gui)
	{
		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;
		RHI::FrameScheduler::Create(desc);
	}

	RPI::RPISystem::Get().Initialize();

	FusionApplication* fApp = FusionApplication::Get();

	FusionInitInfo initInfo = {};
	initInfo.assetLoader = nullptr;
	fApp->Initialize(initInfo);

	RendererSystem::Get().Init();

	JobManagerDesc desc{};
	desc.defaultTag = JOB_THREAD_WORKER;
	desc.totalThreads = 0; // auto set optimal number of threads
	desc.threads.Resize(4, { .threadName = "FusionThread", .tag = JOB_THREAD_FUSION });

	gJobManager = new JobManager("JobSystemManager", desc);
	gJobContext = new JobContext(gJobManager);
	JobContext::PushGlobalContext(gJobContext);

	Logger::Initialize();
}
static void TestEnd(bool gui)
{
	Logger::Shutdown();

	gJobManager->DeactivateWorkersAndWait();

	JobContext::PopGlobalContext();
	delete gJobContext;
	gJobContext = nullptr;
	delete gJobManager;
	gJobManager = nullptr;

	RendererSystem::Get().Shutdown();

	FusionApplication* fApp = FusionApplication::Get();

	fApp->PreShutdown();
	fApp->Shutdown();
	fApp->BeginDestroy();
	
	if (gui)
	{
		delete RHI::FrameScheduler::Get();
	}

	RPI::RPISystem::Get().Shutdown();

	RHI::gDynamicRHI->PreShutdown();
	RHI::gDynamicRHI->Shutdown();
	delete RHI::gDynamicRHI; RHI::gDynamicRHI = nullptr;

	PlatformApplication* app = PlatformApplication::Get();

	if (gui)
	{
		InputManager::Get().Shutdown(app);
	}

	app->PreShutdown();
	app->Shutdown();
	delete app;

	CEDeregisterModuleTypes();
	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("FusionCore");
	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
}

namespace cl = Clipper2Lib;

void System(const std::string& filename)
{
#ifdef _WIN32
  system(filename.c_str());
#else
  system(("firefox " + filename).c_str());
#endif
}


cl::Path64 MakeRandomRectangle(int minWidth, int minHeight, int maxWidth, int maxHeight,
  int maxRight, int maxBottom)
{
    using namespace Clipper2Lib;
    
    int w = maxWidth > minWidth ? minWidth + rand() % (maxWidth - minWidth): minWidth;
    int h = maxHeight > minHeight ? minHeight + rand() % (maxHeight - minHeight): minHeight;
    int l = rand() % (maxRight - w);
    int t = rand() % (maxBottom - h);
    Path64 result;
    result.reserve(4);
    result.push_back(Point64(l, t));
    result.push_back(Point64(l+w, t));
    result.push_back(Point64(l+w, t+h));
    result.push_back(Point64(l, t+h));
    return result;
}

void DoRectangles(int cnt)
{
    using namespace Clipper2Lib;
    const int width = 800, height = 600, margin = 120;
    
    Paths64 sub, clp, sol, store;
    //Rect64 rect = Rect64(margin, margin, width - margin, height - margin);
    //clp.push_back(rect.AsPath());
    //for (int i = 0; i < cnt; ++i)
    //    sub.push_back(MakeRandomRectangle(10, 10, 100, 100, width, height));
    sub.push_back(Rect64(100, 100, 200, 200).AsPath());
    clp.push_back(Rect64(50, 50, 250, 250).AsPath());

    //sol = RectClip(rect, sub);
    sol = Intersect(sub, clp, FillRule::EvenOdd);

    String::IsAlphabet('a');
}

void TestClipping()
{
    DoRectangles(1);
    
    String::IsAlphabet('a');
}

static void DoPaint(FusionRenderer2* renderer)
{
	renderer->Begin();

	renderer->SetBrush(FBrush(Color::Black()));
	renderer->FillRect(CE::Rect::FromSize(100, 100, 200, 200));

	FPen pen = Color::Blue();

	renderer->PushChildCoordinateSpace(Matrix4x4::Translation(Vec2(100, 100)) * Matrix4x4::Angle(0) * Matrix4x4::Scale(Vec3(1, 1, 1)));
	renderer->PushClipRect(Matrix4x4::Angle(0), Vec2(100, 100));
	{
		pen.SetThickness(2.0f);
		renderer->SetPen(pen);
		renderer->SetBrush(Color::Cyan());

		renderer->FillRect(CE::Rect::FromSize(30, 30, 100, 60), Vec4(5, 10, 15, 20));
		renderer->StrokeRect(CE::Rect::FromSize(30, 30, 100, 60), Vec4(5, 10, 15, 20));

		f32 angle = 0;

		Vec2 arcPos = Vec2(60, 60);
		renderer->PushChildCoordinateSpace(Matrix4x4::Identity());
		renderer->PushClipRect(Matrix4x4::Translation(Vec2(70, 70)), Vec2(300, 300));

		renderer->SetBrush(Color::Red());
		renderer->PathLineTo(arcPos);
		renderer->PathArcToFast(arcPos, 50, 0, 3); // 0 to 90 degree
		renderer->PathFillStroke(true);

		renderer->PopClipRect();
		renderer->PopChildCoordinateSpace();
	}
	renderer->PopClipRect();
	renderer->PopChildCoordinateSpace();

	pen.SetColor(Color::Cyan());
	pen.SetThickness(2.0f);
	renderer->SetPen(pen);
	renderer->FillCircle(Vec2(300, 300), 25);
	renderer->StrokeCircle(Vec2(300, 300), 25);

	pen.SetColor(Color::Red());
	pen.SetThickness(2.5f);
	renderer->SetPen(pen);

	renderer->PathLineTo(Vec2(25, 25));
	renderer->PathBezierCubicCurveTo(Vec2(50, 50), Vec2(0, 75), Vec2(25, 100));
	renderer->PathStroke();

	renderer->End();
}

TEST(FusionCore, Rendering)
{
	TEST_BEGIN_GUI;
	using namespace RenderingTests;

	f32 deltaTime = 0;
	clock_t previousTime = {};

	FusionApplication* app = FusionApplication::Get();
	FStyleManager* styleManager = app->GetStyleManager();

	FRootContext* rootContext = FusionApplication::Get()->GetRootContext();

	FStyleSet* rootStyle = CreateObject<FStyleSet>(rootContext, "RootStyleSet");
	styleManager->RegisterStyleSet(rootStyle->GetName(), rootStyle);
	rootContext->SetDefaultStyleSet(rootStyle);

	// - Styling -
	{
		{
			auto primaryBtn = CreateObject<FCustomButtonStyle>(rootStyle, "PrimaryButton");
			rootStyle->Add("Button.Primary", primaryBtn);

			primaryBtn->background = Color::RGBA(56, 56, 56);
			primaryBtn->hoveredBackground = Color::RGBA(95, 95, 95);
			primaryBtn->pressedBackground = Color::RGBA(50, 50, 50);
			primaryBtn->cornerRadius = Vec4(1, 1, 1, 1) * 5;
			primaryBtn->borderColor = Color::RGBA(24, 24, 24);
			primaryBtn->borderWidth = 1.0f;

			GetDefaultWidget<FButton>()
				.Padding(Vec4(10, 5, 10, 5))
				.Style(rootStyle, "Button.Primary")
				;
			GetDefaultWidget<FTextButton>()
				.Padding(Vec4(10, 5, 10, 5))
				.Style(rootStyle, "Button.Primary")
				;

			GetDefaultWidget<FScrollBox>()
				.ScrollBarBackground(Color::RGBA(50, 50, 50))
				.ScrollBarBrush(Color::RGBA(60, 60, 60))
				;
		}

		{
			auto windowCloseBtn = CreateObject<FCustomButtonStyle>(rootStyle, "WindowWindowClose");
			rootStyle->Add("Button.WindowClose", windowCloseBtn);

			windowCloseBtn->background = Color::Clear();
			windowCloseBtn->hoveredBackground = Color::RGBA(161, 57, 57);
			windowCloseBtn->hoveredBackground = Color::RGBA(196, 43, 28);
			windowCloseBtn->pressedBackground = Color::RGBA(178, 43, 30);
			windowCloseBtn->borderColor = Color::Clear();
			windowCloseBtn->borderWidth = 0.0f;
			windowCloseBtn->cornerRadius = Vec4();
			windowCloseBtn->contentMoveY = 0;

			auto windowControlBtn = CreateObject<FCustomButtonStyle>(rootStyle, "WindowControlButton");
			rootStyle->Add("Button.WindowControl", windowControlBtn);

			windowControlBtn->background = Color::Clear();
			windowControlBtn->hoveredBackground = Color::RGBA(58, 58, 58);
			windowControlBtn->pressedBackground = Color::RGBA(48, 48, 48);
			windowControlBtn->borderColor = Color::Clear();
			windowControlBtn->borderWidth = 0.0f;
			windowControlBtn->cornerRadius = Vec4();
			windowControlBtn->contentMoveY = 0;
		}
		
		{
			auto primaryTextInput = CreateObject<FTextInputPlainStyle>(rootStyle, "PrimaryTextInput");
			rootStyle->Add("TextInput.Primary", primaryTextInput);

			primaryTextInput->background = primaryTextInput->hoverBackground = primaryTextInput->editingBackground = Color::RGBA(15, 15, 15);
			primaryTextInput->borderColor = Color::RGBA(60, 60, 60);
			primaryTextInput->editingBorderColor = Color::RGBA(0, 112, 224);
			primaryTextInput->hoverBorderColor = Color::RGBA(74, 74, 74);
			primaryTextInput->borderWidth = 1.0f;
			primaryTextInput->cornerRadius = Vec4(5, 5, 5, 5);

			GetDefaultWidget<FTextInput>()
				.Style(rootStyle, "TextInput.Primary");
		}

		{
			auto primaryComboBox = CreateObject<FComboBoxStyle>(rootStyle, "PrimaryComboBoxStyle");
			rootStyle->Add("ComboBox.Primary", primaryComboBox);

			primaryComboBox->background = Color::RGBA(15, 15, 15);
			primaryComboBox->borderColor = Color::RGBA(60, 60, 60);
			primaryComboBox->pressedBorderColor = primaryComboBox->hoverBorderColor = Color::RGBA(74, 74, 74);
			primaryComboBox->borderWidth = 1.0f;
			primaryComboBox->cornerRadius = Vec4(5, 5, 5, 5);

			auto primaryComboBoxItem = CreateObject<FComboBoxItemStyle>(rootStyle, "PrimaryComboBoxItemStyle");
			rootStyle->Add("ComboBoxItem.Primary", primaryComboBoxItem);

			primaryComboBoxItem->background = Color::Clear();
			primaryComboBoxItem->hoverBackground = Color::RGBA(0, 112, 224);
			primaryComboBoxItem->selectedBackground = Color::Clear();
			primaryComboBoxItem->selectedShape = FShapeType::RoundedRect;
			primaryComboBoxItem->shapeCornerRadius = Vec4(1, 1, 1, 1) * 3;
			primaryComboBoxItem->selectedBorderColor = primaryComboBoxItem->hoverBackground.GetFillColor();
			primaryComboBoxItem->borderWidth = 1.0f;

			GetDefaultWidget<FComboBox>()
				.ItemStyle(primaryComboBoxItem)
				.Style(rootStyle, "ComboBox.Primary");

			auto primaryComboBoxPopup = CreateObject<FComboBoxPopupStyle>(rootStyle, "PrimaryComboBoxPopupStyle");
			rootStyle->Add("ComboBoxPopup.Primary", primaryComboBoxPopup);

			primaryComboBoxPopup->background = Color::RGBA(26, 26, 26);
			primaryComboBoxPopup->borderWidth = 0.0f;

			GetDefaultWidget<FComboBoxPopup>()
				.Style(rootStyle, "ComboBoxPopup.Primary");
		}

		{
			auto primaryTabView = CreateObject<FTabViewStyle>(rootStyle, "TabViewStyle");
			rootStyle->Add("TabView.Primary", primaryTabView);

			primaryTabView->tabItemActiveBackground = Color::RGBA(50, 50, 50);
			primaryTabView->tabItemHoverBackground = Color::RGBA(40, 40, 40);
			primaryTabView->tabItemBackground = Color::RGBA(36, 36, 36);
			primaryTabView->tabItemShape = FRoundedRectangle(3, 3, 0, 0);

			GetDefaultWidget<FTabView>()
				.Style(rootStyle, "TabView.Primary");
		}

		{
			auto menuStyle = CreateObject<FMenuPopupStyle>(rootStyle, "MenuPopupStyle");
			rootStyle->Add("MenuPopup.Primary", menuStyle);

			menuStyle->background = Color::RGBA(56, 56, 56);
			menuStyle->itemPadding = Vec4(10, 5, 10, 5);
			menuStyle->itemHoverBackground = Color::RGBA(0, 112, 224);

			GetDefaultWidget<FMenuPopup>()
				.Style(rootStyle, "MenuPopup.Primary");

			auto menuBarStyle = CreateObject<FMenuBarStyle>(rootStyle, "MenuBarStyle");
			rootStyle->Add("MenuBar.Primary", menuBarStyle);

			menuBarStyle->background = Color::Black();
			menuBarStyle->itemPadding = Vec4(5, 5, 5, 5);
			menuBarStyle->itemHoverBackground = Color::RGBA(0, 112, 224);

			GetDefaultWidget<FMenuBar>()
				.Style(rootStyle, "MenuBar.Primary");
		}
	}

	PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();

	FNativeContext* nativeContext = FNativeContext::Create(mainWindow, "TestWindow", rootContext);
	rootContext->AddChildContext(nativeContext);

	RenderingTestWidget* mainWidget;
	FAssignNewOwned(RenderingTestWidget, mainWidget, nativeContext);

	nativeContext->SetOwningWidget(mainWidget);

	auto exposedTick = [&]
		{
			FusionApplication::Get()->SetExposed();

			FusionApplication::Get()->Tick();

			RendererSystem::Get().Render();
		};

	DelegateHandle handle = PlatformApplication::Get()->AddTickHandler(exposedTick);

	if (mainWidget->comboBox)
	{
		mainWidget->comboBox->ApplyStyle();
	}

	mainWindow->Show();

	int frameCounter = 0;
    
    TestClipping();

	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

		FusionApplication::Get()->ResetExposed();

		// App & Input Tick
		PlatformApplication::Get()->Tick();
		InputManager::Get().Tick();

		FusionApplication::Get()->Tick();

		DoPaint(nativeContext->GetPainter()->GetRenderer());

		RendererSystem::Get().Render();

		previousTime = curTime;
		frameCounter++;
	}

	PlatformApplication::Get()->RemoveTickHandler(handle);

	styleManager->DeregisterStyleSet(rootStyle->GetName());

	TEST_END_GUI;
}

