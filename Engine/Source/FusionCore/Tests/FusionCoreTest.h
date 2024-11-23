#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;

namespace RenderingTests
{
	class RendererSystem : public ApplicationMessageHandler
	{
		CE_NO_COPY(RendererSystem)
	public:

		static RendererSystem& Get()
		{
			static RendererSystem instance{};
			return instance;
		}

		void Init();
		void Shutdown();

		void Render();

		void BuildFrameGraph();
		void CompileFrameGraph();

		void OnWindowRestored(PlatformWindow* window) override;
		void OnWindowDestroyed(PlatformWindow* window) override;
		void OnWindowClosed(PlatformWindow* window) override;
		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
		void OnWindowMinimized(PlatformWindow* window) override;
		void OnWindowCreated(PlatformWindow* window) override;
		void OnWindowExposed(PlatformWindow* window) override;

	private:

		RendererSystem() {}

	public:

		bool rebuildFrameGraph = true;
		bool recompileFrameGraph = true;
		int curImageIndex = 0;

		RHI::DrawListContext drawList{};
	};

	CLASS()
	class RenderingTestWidget : public FWindow, public ApplicationMessageHandler
	{
		CE_CLASS(RenderingTestWidget, FWindow)
	public:

		RenderingTestWidget() = default;

		void Construct() override;

		void OnBeginDestroy() override;

		void OnWindowRestored(PlatformWindow* window) override;
		void OnWindowMaximized(PlatformWindow* window) override;
		void OnWindowExposed(PlatformWindow* window) override;

		FStackBox* rootBox = nullptr;
		FButton* button = nullptr;
		FTextInput* textInput = nullptr;
		FComboBox* comboBox = nullptr;
		FLabel* buttonLabel = nullptr;
		FStyledWidget* subWidget = nullptr;
		FPopup* btnPopup = nullptr;
		FPopup* nativePopup = nullptr;
		FImage* maximizeIcon = nullptr;
		FStyledWidget* borderWidget = nullptr;
		FTextInput* modelTextInput = nullptr;
		FVerticalStack* windowContent = nullptr;

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"
