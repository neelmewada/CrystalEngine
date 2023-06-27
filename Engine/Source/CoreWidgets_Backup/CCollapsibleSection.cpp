#include "CoreWidgets.h"

namespace CE::Widgets
{

	CCollapsibleSection::CCollapsibleSection()
	{

	}

	CCollapsibleSection::~CCollapsibleSection()
	{

	}

	void CCollapsibleSection::OnDrawGUI()
	{
		style.Push();

		bool hasPadding = false;
		Vec4 padding{};
		bool hasBorderRadius = false;
		Vec4 borderRadius{};
		bool hasBorderWidth = false;
		f32 borderWidth = 0;

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::Header)
				{
					hasPadding = true;
					padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::BorderRadius && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::Header)
				{
					if (styleValue.valueType == CStyleValue::Type_Vector)
					{
						hasBorderRadius = true;
						borderRadius = styleValue.vector;
					}
					else if (styleValue.valueType == CStyleValue::Type_Single)
					{
						hasBorderRadius = true;
						borderRadius = Vec4(1, 1, 1, 1) * styleValue.single;
					}
				}
				else if (property == CStylePropertyType::BorderWidth && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::Header && styleValue.valueType == CStyleValue::Type_Single)
				{
					hasBorderWidth = true;
					borderWidth = 0;
				}
			}
		}

		isOpen = GUI::CollapsibleHeader(internalTitle, hasPadding ? &padding : nullptr, 
			hasBorderRadius ? &borderRadius : nullptr, 
			hasBorderWidth ? &borderWidth : nullptr);

		PollEvents();

		style.Pop();
		
		if (isOpen)
		{
			for (int i = 0; i < indentLevel; i++)
				GUI::Indent();

			style.Push(CStylePropertyTypeFlags::Inherited);
			for (CWidget* subWidget : attachedWidgets)
			{
				subWidget->RenderGUI();
			}
			style.Pop();

			for (int i = 0; i < indentLevel; i++)
				GUI::Unindent();
		}
		
	}

	void CCollapsibleSection::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
