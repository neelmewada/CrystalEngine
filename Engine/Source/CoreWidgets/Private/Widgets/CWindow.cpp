#include "CoreWidgets.h"

namespace CE::Widgets
{

    CWindow::CWindow()
    {
		SetTitle(GetName().GetString());
    }

    CWindow::~CWindow()
    {

    }

	Vec2 CWindow::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(windowSize.x, allowVerticalScroll ? 0 : windowSize.y);
	}

	void CWindow::Show()
    {
        isShown = true;
    }

    void CWindow::Hide()
    {
        isShown = false;
    }

    void CWindow::SetTitle(const String& title)
    {
        this->windowTitle = String::Format(title + "###{}", GetName());
    }

	Color CWindow::FetchBackgroundColor(CStateFlag state, CSubControl subControl)
	{
		auto style = stylesheet->SelectStyle(this, state, subControl);
		if (style.properties.KeyExists(CStylePropertyType::Background))
			return style.properties[CStylePropertyType::Background].color;

		return Color();
	}

	void CWindow::OnAfterComputeStyle()
	{
		titleBar = FetchBackgroundColor(CStateFlag::Default, CSubControl::TitleBar);
		titleBarActive = FetchBackgroundColor(CStateFlag::Active, CSubControl::TitleBar);

		tab = FetchBackgroundColor(CStateFlag::Default, CSubControl::Tab);
		tabActive = FetchBackgroundColor(CStateFlag::Active, CSubControl::Tab);
		tabHovered = FetchBackgroundColor(CStateFlag::Hovered, CSubControl::Tab);
	}

	void CWindow::SetAsDockSpaceWindow(bool set)
	{
		isDockSpaceWindow = set;

		if (isDockSpaceWindow && !StyleClassExists("DockSpace"))
		{
			AddStyleClass("DockSpace");
		}
		else if (!isDockSpaceWindow && StyleClassExists("DockSpace"))
		{
			RemoveStyleClass("DockSpace");
		}
	}

    void CWindow::OnDrawGUI()
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

	void CWindow::HandleEvent(CEvent* event)
	{
		if (event->GetEventType() == CEventType::ResizeEvent)
		{
			CResizeEvent* resizeEvent = (CResizeEvent*)event;
			emit OnWindowResized(resizeEvent->oldSize, resizeEvent->newSize);

			SetNeedsLayout();
		}

		Super::HandleEvent(event);
	}

	void CWindow::OnSubobjectAttached(Object* subobject)
	{
		Super::OnSubobjectAttached(subobject);

		if (subobject == nullptr)
			return;

		if (subobject->GetClass()->IsSubclassOf<CMenuBar>())
		{
			this->menuBar = (CMenuBar*)subobject;
		}
	}

    void CWindow::AddSubWidget(CWidget* subWidget)
    {
        AttachSubobject(subWidget);
    }

    void CWindow::RemoveSubWidget(CWidget* subWidget)
    {
        DetachSubobject(subWidget);
    }
    
} // namespace CE::Widgets
