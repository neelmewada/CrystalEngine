#include "CoreWidgets.h"

namespace CE::Widgets
{
	CCollapsibleSection::CCollapsibleSection()
	{
		internalId = GenerateRandomU32();
		isCollapsed = false;
	}

	CCollapsibleSection::~CCollapsibleSection()
	{

	}

	void CCollapsibleSection::SetCollapsed(bool collapsed)
	{
		isCollapsed = collapsed;

		if (isCollapsed)
		{
			ClearChildNodes();
		}
		else
		{
			ReAddChildNodes();
		}

		SetNeedsStyleRecursive();
		SetNeedsLayout();
	}

	void CCollapsibleSection::Construct()
	{
		Super::Construct();

	}

	void CCollapsibleSection::OnSubobjectAttached(Object* subobject)
	{
		Super::OnSubobjectAttached(subobject);

		SetCollapsed(IsCollapsed());
	}

	Vec2 CCollapsibleSection::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		CFontManager::Get().PushFont(headerStyleState.fontSize, headerStyleState.fontName);
		Vec2 result = Vec2(width, GUI::CalculateTextSize("label").height);
		CFontManager::Get().PopFont();
		return result;
	}

	void CCollapsibleSection::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		headerStyleState.fontSize = defaultStyleState.fontSize;

		auto defaultHeaderStyle = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Header);
		for (const auto& [property, styleValue] : defaultHeaderStyle.properties)
		{
			LoadGuiStyleStateProperty(property, styleValue, headerStyleState);
			if (property == CStylePropertyType::Padding && styleValue.IsVector())
				headerPadding = styleValue.vector;
			else if (property == CStylePropertyType::Padding && styleValue.IsSingle())
				headerPadding = Vec4(1, 1, 1, 1) * styleValue.single;
		}

		if (headerState != CStateFlag::Default)
		{
			auto headerStyle = stylesheet->SelectStyle(this, headerState, CSubControl::Header);
			for (const auto& [property, styleValue] : headerStyle.properties)
			{
				LoadGuiStyleStateProperty(property, styleValue, headerStyleState);
			}
		}

		if (!isCollapsed)
		{
			auto bottomMargin = YGNodeStyleGetMargin(node, YGEdgeBottom);
			if (bottomMargin.unit == YGUnitUndefined || std::isnan(bottomMargin.value))
				bottomMargin.value = 0;
			auto headerHeight = CalculateIntrinsicContentSize(0, 0).height + headerPadding.top + headerPadding.bottom;

			YGNodeStyleSetMargin(node, YGEdgeBottom, bottomMargin.value + headerHeight);
		}
	}

	void CCollapsibleSection::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto contentPadding = GetComputedLayoutPadding();

		CFontManager::Get().PushFont(headerStyleState.fontSize, headerStyleState.fontName);

		f32 headerHeight = CalculateIntrinsicContentSize(0, 0).height + headerPadding.top + headerPadding.bottom;

		if (!IsCollapsed())
		{
			DrawDefaultBackground(rect + Rect(0, headerHeight, 0, headerHeight));
		}

		GUI::TreeNodeFlags treeFlags = GUI::TNF_None;
		if (!IsCollapsed())
		{
			treeFlags |= GUI::TNF_DefaultOpen;
		}

		bool hovered = false, held = false;
		bool active = GUI::CollapsibleHeader(rect, title, internalId, headerStyleState, hovered, held, headerPadding, contentPadding, treeFlags);
		PollEvents();

		CFontManager::Get().PopFont();

		if (!isCollapsed)
		{
			//GUI::PushChildCoordinateSpace(rect + Rect(0, headerHeight, 0, headerHeight));

			GUI::WindowFlags childFlags = GUI::WF_NoMove | GUI::WF_NoResize | GUI::WF_NoBackground | 
				GUI::WF_NoScrollbar | GUI::WF_NoScrollWithMouse | GUI::WF_NoDecoration;
			bool open = GUI::BeginChild(rect + Rect(0, headerHeight, 0, headerHeight), GetUuid(), "untitled", {}, {}, childFlags);

			if (open)
			{
				GUI::PushZeroingChildCoordinateSpace();
				for (auto child : attachedWidgets)
				{
					child->Render();
				}
				GUI::PopChildCoordinateSpace();
			}

			GUI::EndChild();

			//GUI::PopChildCoordinateSpace();
		}

		if (isCollapsed != !active)
		{
			SetCollapsed(!active);
			headerState = !IsCollapsed() ? CStateFlag::Active : (hovered ? CStateFlag::Hovered : CStateFlag::Default);
			SetNeedsStyle();
			SetNeedsLayout();
		}
	}

} // namespace CE::Widgets
