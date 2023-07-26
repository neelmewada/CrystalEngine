#include "CrystalEditor.h"

namespace CE::Editor
{
	ImageTestPanel::ImageTestPanel()
	{
		SetAllowHorizontalScroll(false);
		SetAllowVerticalScroll(false);

		SetTitle("Image Test Panel");
	}

	ImageTestPanel::~ImageTestPanel()
	{

	}

	void ImageTestPanel::Construct()
	{
		Super::Construct();


	}

	void ImageTestPanel::OnDrawGUI()
	{
		if (isShown)
		{
			GUI::WindowFlags windowFlags = GUI::WF_None;
			if (allowHorizontalScroll)
				windowFlags |= GUI::WF_HorizontalScrollbar;
			if (!allowVerticalScroll)
				windowFlags |= GUI::WF_NoScrollWithMouse | GUI::WF_NoScrollbar;
			if (isFullscreen)
				windowFlags |= GUI::WF_FullScreen | GUI::WF_NoPadding;
			if (IsDockSpaceWindow())
				windowFlags |= GUI::WF_NoPadding;

			auto color = defaultStyleState.background;

			GUI::PushStyleColor(GUI::StyleCol_TitleBg, titleBar);
			GUI::PushStyleColor(GUI::StyleCol_TitleBgActive, titleBarActive);
			GUI::PushStyleColor(GUI::StyleCol_TitleBgCollapsed, titleBar);

			GUI::PushStyleColor(GUI::StyleCol_Tab, tab);
			GUI::PushStyleColor(GUI::StyleCol_TabActive, tabActive);
			GUI::PushStyleColor(GUI::StyleCol_TabUnfocused, tab);
			GUI::PushStyleColor(GUI::StyleCol_TabHovered, tabHovered);
			GUI::PushStyleColor(GUI::StyleCol_TabUnfocusedActive, tabActive);

			if (color.a > 0)
			{
				GUI::PushStyleColor(GUI::StyleCol_WindowBg, color);
				GUI::PushStyleColor(GUI::StyleCol_DockingEmptyBg, color);
			}

			if (defaultStyleState.borderColor.a > 0)
			{
				GUI::PushStyleColor(GUI::StyleCol_Border, defaultStyleState.borderColor);
			}

			if (defaultStyleState.borderThickness > 0.2f)
			{
				GUI::PushStyleVar(GUI::StyleVar_WindowBorderSize, defaultStyleState.borderThickness);
			}

			GUI::BeginWindow(windowTitle, &isShown, windowFlags);

			this->platformHandle = GUI::GetCurrentViewport()->platformHandle;
			if (this->platformHandle != nullptr)
			{
				// TODO: move this function to CoreApplication instead of VulkanRHI
				this->screenSize = RHI::gDynamicRHI->GetScreenSizeForWindow(this->platformHandle);
			}

			auto topLeftPos = GUI::GetCursorPos();
			GUI::PushZeroingChildCoordinateSpace();
			GUI::PushChildCoordinateSpace(Rect(5, 10, 0, 0));

			if (IsDockSpaceWindow())
			{
				if (dockSpaceId.IsEmpty())
					dockSpaceId = String::Format("DockSpace##{}", GetName());
				GUI::DockSpace(dockSpaceId);
			}

			if (color.a > 0)
				GUI::PopStyleColor(2);

			for (CWidget* subWidget : attachedWidgets)
			{
				subWidget->Render();
			}

			auto winSize = GUI::GetWindowSize();
			auto winPos = GUI::GetWindowPos();

			if (!icon.IsValid())
				icon = GetStyleManager()->SearchImageResource("/Icons/folder.png");

			if (icon.IsValid())
			{
				Rect rect = Rect(Vec2(), Vec2(150, 150));

				GUI::Image(rect, icon.id, {});
			}

			GUI::PopChildCoordinateSpace();
			GUI::PopChildCoordinateSpace();

			GUI::EndWindow();

			if (windowSize.x > 0 && windowSize.y > 0 && windowSize != winSize)
			{
				CResizeEvent event{};
				event.name = "WindowResized";
				event.type = CEventType::ResizeEvent;
				event.oldSize = windowSize;
				event.newSize = winSize;
				event.sender = this;
				HandleEvent(&event);
			}

			this->windowSize = winSize;
			this->windowPos = winPos;

			PollEvents();

			if (defaultStyleState.borderThickness > 0.2f)
			{
				GUI::PopStyleVar(1);
			}

			if (defaultStyleState.borderColor.a > 0)
			{
				GUI::PopStyleColor();
			}

			GUI::PopStyleColor(8);
		}
	}

} // namespace CE::Editor
