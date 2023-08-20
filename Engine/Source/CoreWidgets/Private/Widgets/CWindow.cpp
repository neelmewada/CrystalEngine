#include "CoreWidgets.h"

#include "imgui.h"

namespace CE::Widgets
{

    CWindow::CWindow()
    {
		
    }

    CWindow::~CWindow()
    {
		
    }

	Vec2 CWindow::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(windowSize.x <= 0 ? YGUndefined : windowSize.x, (allowVerticalScroll || windowSize.y <= 0) ? YGUndefined : windowSize.y);
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

		//auto style = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Tab);
		//tabPadding = style.properties[CStylePropertyType::Padding].vector;
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

	void CWindow::Construct()
	{
		Super::Construct();

		if (GetWidgetDebugger() != nullptr && GetOwner() == nullptr)
			GetWidgetDebugger()->registeredWindows.Add(this);

		SetTitle(GetName().GetString());
	}

	void CWindow::OnBeforeDestroy()
	{
		if (GetWidgetDebugger() != nullptr)
			GetWidgetDebugger()->registeredWindows.Remove(this);
	}

	void CWindow::OnDrawGUI()
    {
        if (isShown)
        {
			auto& io = ImGui::GetIO();

			GUI::WindowFlags windowFlags = GUI::WF_None;
			if (allowHorizontalScroll)
				windowFlags |= GUI::WF_HorizontalScrollbar;
			if (!allowVerticalScroll)
				windowFlags |= GUI::WF_NoScrollWithMouse | GUI::WF_NoScrollbar;
			if (isFullscreen)
				windowFlags |= GUI::WF_FullScreen | GUI::WF_NoPadding;
			if (IsDockSpaceWindow())
				windowFlags |= GUI::WF_NoPadding;
			if (IsDockSpaceWindow() && isFullscreen)
				windowFlags |= GUI::WF_NoBringToFrontOnFocus;
			if (menuBar != nullptr)
				windowFlags |= GUI::WF_MenuBar;

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

			GUI::PushStyleVar(GUI::StyleVar_FramePadding, tabPadding);

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
				auto dockId = GUI::DockSpace(dockSpaceId, {}, GUI::DockFlags_NoCloseButton);
				
				if (setDefaultDocking)
				{
					setDefaultDocking = false;

					GUI::DockBuilderRemoveNode(dockId);
					GUI::DockBuilderAddNode(dockId);
					bool fillFound = false;

					for (CWidget* subWidget : attachedWidgets)
					{
						if (subWidget->IsOfType<CWindow>())
						{
							CWindow* window = (CWindow*)subWidget;
							if (window->defaultDockPosition == CDockPosition::Fill)
							{
								fillFound = true;
								GUI::DockBuilderDockWindow(window->GetTitle(), dockId);
							}
						}
					}

					if (!fillFound)
					{
						auto rightDockId = GUI::DockBuilderSplitNode(dockId, GUI::Dir_Right, 0.25f, nullptr, &dockId);
						auto centreDockId = GUI::DockBuilderSplitNode(dockId, GUI::Dir_Left, 0.75f, nullptr, &dockId);
						auto bottomDockId = GUI::DockBuilderSplitNode(centreDockId, GUI::Dir_Down, 0.35f, nullptr, &centreDockId);
						auto leftDockId = GUI::DockBuilderSplitNode(centreDockId, GUI::Dir_Left, 0.25f, nullptr, &centreDockId);
						auto rightBottomDockId = GUI::DockBuilderSplitNode(rightDockId, GUI::Dir_Down, 0.6f, nullptr, &rightDockId);

						for (CWidget* subWidget : attachedWidgets)
						{
							if (subWidget->IsOfType<CWindow>())
							{
								CWindow* window = (CWindow*)subWidget;
								String title = window->GetTitle();
								switch (window->defaultDockPosition)
								{
								case CDockPosition::Center:
									GUI::DockBuilderDockWindow(title, centreDockId);
									break;
								case CDockPosition::Left:
									GUI::DockBuilderDockWindow(title, leftDockId);
									break;
								case CDockPosition::Right:
									GUI::DockBuilderDockWindow(title, rightDockId);
									break;
								case CDockPosition::Bottom:
									GUI::DockBuilderDockWindow(title, bottomDockId);
									break;
								case CDockPosition::RightBottom:
									GUI::DockBuilderDockWindow(title, rightBottomDockId);
									break;
								}
							}
						}
					}

					GUI::DockBuilderFinish(dockId);
				}
			}

			GUI::PopStyleVar();

			if (color.a > 0)
				GUI::PopStyleColor(2);

			auto winSize = GUI::GetWindowSize();
			auto winPos = GUI::GetWindowPos();

			if (backgroundImage != nullptr)
			{
				auto imageRect = Rect(winPos, winPos + winSize);

				GUI::PushChildCoordinateSpace(Rect(-winPos, winSize));
				GUI::Image(imageRect, backgroundImage, {});
				GUI::PopChildCoordinateSpace();
			}

			if (toolBar != nullptr)
			{
				auto toolBarHeight = toolBar->GetComputedLayoutSize().y;
				if (!IsNan(toolBarHeight) && toolBarHeight > 1)
				{
					GUI::SetWindowToolBarHeight(toolBarHeight);
				}
			}

            for (CWidget* subWidget : attachedWidgets)
            {
				subWidget->Render();
            }
			
			GUI::PushStyleVar(GUI::StyleVar_FramePadding, tabPadding);

            GUI::EndWindow();

			GUI::PopStyleVar();

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

			for (auto popup : attachedPopups)
			{
				if (popup->IsShown())
					popup->Render();
			}
        }
    }

	void CWindow::HandleEvent(CEvent* event)
	{
		if (event->GetEventType() == CEventType::ResizeEvent)
		{
			CResizeEvent* resizeEvent = (CResizeEvent*)event;
			emit OnWindowResized(resizeEvent->oldSize, resizeEvent->newSize);

			SetNeedsLayoutRecursive();
		}

		Super::HandleEvent(event);
	}

	void CWindow::OnSubobjectAttached(Object* subobject)
	{
		Super::OnSubobjectAttached(subobject);

		if (subobject == nullptr)
			return;

		if (subobject->IsOfType<CMenuBar>())
		{
			this->menuBar = (CMenuBar*)subobject;
		}
		else if (subobject->IsOfType<CToolBar>())
		{
			this->toolBar = (CToolBar*)toolBar;
		}
		else if (subobject->IsOfType<CPopup>())
		{
			this->attachedPopups.Add((CPopup*)subobject);
		}
	}

	void CWindow::OnSubobjectDetached(Object* subobject)
	{
		Super::OnSubobjectDetached(subobject);

		if (subobject == nullptr)
			return;

		if (subobject->IsOfType<CPopup>())
		{
			this->attachedPopups.Remove((CPopup*)subobject);
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
