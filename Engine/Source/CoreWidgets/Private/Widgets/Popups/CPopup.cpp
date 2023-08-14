#include "CoreWidgets.h"

namespace CE::Widgets
{

	CPopup::CPopup()
	{
		title = String::Format("{}###{}", GetName(), (u64)GetUuid());
		subControl = CSubControl::None;
	}

	CPopup::~CPopup()
	{

	}

	void CPopup::Show()
	{
		isShown = true;
		pushShow = true;
	}

	void CPopup::Hide()
	{
		isShown = false;
	}

	Color CPopup::FetchBackgroundColor(CStateFlag state, CSubControl subControl)
	{
		auto style = stylesheet->SelectStyle(this, state, subControl);
		return style.properties[CStylePropertyType::Background].color;
	}

	void CPopup::OnBeforeComputeStyle()
	{
		Super::OnBeforeComputeStyle();


	}

	void CPopup::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		titleBar = FetchBackgroundColor(CStateFlag::Default, CSubControl::TitleBar);
		titleBarActive = FetchBackgroundColor(CStateFlag::Active, CSubControl::TitleBar);
	}

	Vec2 CPopup::CalculateShowPosition()
	{
		SetNeedsLayout();
		UpdateLayoutIfNeeded();
		
		auto size = GetComputedLayoutSize();
		auto parentSize = PlatformApplication::Get()->GetMainScreenSize();

		Object* outer = GetOuter();

		if (outer != nullptr && outer->IsOfType<CWindow>())
		{
			CWindow* window = (CWindow*)outer;
			auto winPos = window->GetWindowPos();
			auto winSize = window->GetWindowSize();
			if (winSize.x > 0 && winSize.y > 0 && !isnan(winSize.x) && !isnan(winSize.y) && !isnan(winPos.x) && !isnan(winPos.y))
			{
				return winPos + winSize / 2 - size / 2;
			}
		}

		auto pos = Vec2(parentSize.x, parentSize.y) / 2 - size / 2;
		return pos;
	}

	void CPopup::OnDrawGUI()
	{
		if (pushShow)
		{
			pushShow = false;
			GUI::OpenPopup(GetUuid());

			GUI::SetNextWindowPos(CalculateShowPosition());
		}

		if (isShown)
		{
			auto size = GetComputedLayoutSize();
			auto viewportSize = GUI::GetMainViewport()->size;

			if (!isnan(size.x) && !isnan(size.y))
			{
				GUI::SetNextWindowSize(size);
			}

			GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoResize;
			if (!showsTitle)
				flags |= GUI::WF_NoTitleBar;
			if (isScrollDisabled)
				flags |= GUI::WF_NoScrollWithMouse;

			GUI::PushStyleColor(GUI::StyleCol_TitleBg, titleBar);
			GUI::PushStyleColor(GUI::StyleCol_TitleBgActive, titleBarActive);
			GUI::PushStyleColor(GUI::StyleCol_TitleBgCollapsed, titleBar);

			int pushedColors = 3;
			int pushedVars = 0;

			if (defaultStyleState.background.a > 0)
			{
				GUI::PushStyleColor(GUI::StyleCol_PopupBg, defaultStyleState.background);
				pushedColors++;
			}

			if (defaultStyleState.borderColor.a > 0)
			{
				GUI::PushStyleColor(GUI::StyleCol_Border, defaultStyleState.borderColor);
				pushedColors++;
			}

			if (defaultStyleState.borderThickness > 0.2f)
			{
				GUI::PushStyleVar(GUI::StyleVar_WindowBorderSize, defaultStyleState.borderThickness);
				pushedVars++;
			}

			bool isOpen = GUI::BeginModalPopup(title, GetUuid(), closeButtonShown ? &isShown : NULL, flags);
			
			if (isOpen)
			{
				if (!isShown)
					GUI::CloseCurrentPopup();

				GUI::PushZeroingChildCoordinateSpace();
				{
					for (auto child : attachedWidgets)
					{
						child->Render();
					}
				}
				GUI::PopChildCoordinateSpace();

				GUI::EndModalPopup();
				PollEvents();
			}

			GUI::PopStyleColor(pushedColors);
			GUI::PopStyleVar(pushedVars);
		}
	}

} // namespace CE::Widgets
