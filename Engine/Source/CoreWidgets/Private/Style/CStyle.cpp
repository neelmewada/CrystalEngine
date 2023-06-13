#include "CoreWidgets.h"

namespace CE::Widgets
{
	CStyleValue::CStyleValue()
	{
		enumValue = Auto;
		valueType = Type_None;

		
	}

	CStyleValue::CStyleValue(f32 single, bool isPercent)
		: valueType(Type_Single), single(single), isPercent(isPercent)
	{

	}

	CStyleValue::CStyleValue(const Vec4& vector, bool isPercent)
		: valueType(Type_Vector), vector(vector), isPercent(isPercent)
	{

	}

	CStyleValue::CStyleValue(const Color& color)
		: valueType(Type_Color), color(color)
	{
		
	}

	CStyleValue::CStyleValue(const Gradient& gradient)
		: valueType(Type_Gradient), gradient(gradient)
	{

	}

	CStyleValue::CStyleValue(const Name& path)
		: valueType(Type_Asset), assetPath(path)
	{

	}

	CStyleValue::CStyleValue(const CStyleValue& copy)
	{
		valueType = copy.valueType;
		enumValue = copy.enumValue;
		isPercent = copy.isPercent;

		state = copy.state;
		subControl = copy.subControl;

		if (valueType == Type_Single)
		{
			single = copy.single;
		}
		else if (valueType == Type_Color)
		{
			color = copy.color;
		}
		else if (valueType == Type_Vector)
		{
			vector = copy.vector;
		}
		else if (valueType == Type_Gradient)
		{
			gradient = copy.gradient;
		}
		else if (valueType == Type_Asset)
		{
			assetPath = copy.assetPath;
		}
	}

	CStyleValue& CStyleValue::operator=(const CStyleValue& copy)
	{
		valueType = copy.valueType;
		enumValue = copy.enumValue;
		isPercent = copy.isPercent;

		state = copy.state;
		subControl = copy.subControl;

		if (valueType == Type_Single)
		{
			single = copy.single;
		}
		else if (valueType == Type_Color)
		{
			color = copy.color;
		}
		else if (valueType == Type_Vector)
		{
			vector = copy.vector;
		}
		else if (valueType == Type_Gradient)
		{
			gradient = copy.gradient;
		}
		else if (valueType == Type_Asset)
		{
			assetPath = copy.assetPath;
		}

		return *this;
	}

	void CStyleValue::Release()
	{
		if (valueType == Type_Gradient)
		{
			gradient.Clear();
			gradient.~Gradient();
		}
		else if (valueType == Type_Asset)
		{
			assetPath.~Name();
		}
	}

	Array<CStylePropertyType> CStyle::GetInheritedProperties()
	{
		static Array<CStylePropertyType> inheritedProperties{
			CStylePropertyType::ForegroundColor,
			CStylePropertyType::TextAlign,
		};

		return inheritedProperties;
	}

	CStylePropertyTypeFlags CStyle::GetPropertyTypeFlags(CStylePropertyType type)
	{
		static Array<CStylePropertyType> inheritedProperties = GetInheritedProperties();
		CStylePropertyTypeFlags flags{};

		if (inheritedProperties.Exists(type))
			flags |= CStylePropertyTypeFlags::Inherited;
		else
			flags |= CStylePropertyTypeFlags::NonInherited;

		return flags;
	}

	bool CStyle::IsInheritedProperty(CStylePropertyType property)
	{
		return GetInheritedProperties().Exists(property);
	}

	void CStyle::AddProperty(CStylePropertyType property, const CStyleValue& value, CStateFlag state, CSubControl subControl)
	{
		CStyleValue& modValue = const_cast<CStyleValue&>(value);
		modValue.state = state;
		modValue.subControl = subControl;

		auto& arr = styleMap[property];

		for (int i = arr.GetSize() - 1; i >= 0; i--)
		{
			// Remove 'duplicate-like' states, because they will be fully overriden anyways.
			if (arr[i].state == value.state &&
				arr[i].subControl == value.subControl && 
				arr[i].valueType == value.valueType)
			{
				arr.RemoveAt(i);
			}
		}

		arr.Add(value);
	}

	Array<CStyleValue>& CStyle::GetProperties(CStylePropertyType property)
	{
		return styleMap[property];
	}

	CStyleValue& CStyle::GetProperty(CStylePropertyType property, CStateFlag forState, CSubControl forSubControl)
	{
		static CStyleValue invalid{};
		auto& arr = styleMap[property];

		for (auto& value : arr)
		{
			if (value.subControl == forSubControl && value.state == forState)
				return value;
		}

		return invalid;
	}

	void CStyle::Push(CStylePropertyTypeFlags flags)
	{
		PushedData pushedData{};

		for (auto& [property, array] : styleMap)
		{
			if ((GetPropertyTypeFlags(property) & flags) == 0) // Flags mismatch
				continue;

			for (const auto& styleValue : array)
			{
				if (property == CStylePropertyType::Background && styleValue.valueType == CStyleValue::Type_Color)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (styleValue.subControl == CSubControl::Tab)
						{
							GUI::PushStyleColor(GUI::StyleCol_Tab, styleValue.color);
							pushedData.pushedColors++;
						}
						else if (styleValue.subControl == CSubControl::TitleBar)
						{
							GUI::PushStyleColor(GUI::StyleCol_TitleBg, styleValue.color);
							pushedData.pushedColors++;
						}
						else if (styleValue.subControl == CSubControl::MenuBar)
						{
							GUI::PushStyleColor(GUI::StyleCol_MenuBarBg, styleValue.color);
							pushedData.pushedColors++;
						}
						else
						{
							GUI::PushStyleColor(GUI::StyleCol_FrameBg, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_WindowBg, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_MenuBarBg, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_ChildBg, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_Button, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_Header, styleValue.color);
							pushedData.pushedColors += 6;
						}
					}
					else if ((styleValue.state & CStateFlag::Hovered) != 0)
					{
						if (styleValue.subControl == CSubControl::Tab)
						{
							GUI::PushStyleColor(GUI::StyleCol_TabHovered, styleValue.color);
							pushedData.pushedColors++;
						}
						else
						{
							GUI::PushStyleColor(GUI::StyleCol_FrameBgHovered, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_ButtonHovered, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_HeaderHovered, styleValue.color);
							pushedData.pushedColors += 3;
						}
					}
					else if ((styleValue.state & CStateFlag::Pressed) != 0)
					{
						if (styleValue.subControl == CSubControl::Tab)
						{
							GUI::PushStyleColor(GUI::StyleCol_TabActive, styleValue.color);
							pushedData.pushedColors++;
						}
						else
						{
							GUI::PushStyleColor(GUI::StyleCol_FrameBgActive, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_ButtonActive, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_HeaderActive, styleValue.color);
							pushedData.pushedColors += 3;
						}
					}

					if ((styleValue.state & CStateFlag::Collapsed) != 0)
					{
						if (styleValue.subControl == CSubControl::TitleBar)
						{
							GUI::PushStyleColor(GUI::StyleCol_TitleBgCollapsed, styleValue.color);
							pushedData.pushedColors++;
						}
					}

					if ((styleValue.state & CStateFlag::Active) != 0)
					{
						if (styleValue.subControl == CSubControl::TitleBar)
						{
							GUI::PushStyleColor(GUI::StyleCol_TitleBgActive, styleValue.color);
							pushedData.pushedColors++;
						}
						else if (styleValue.subControl == CSubControl::Tab)
						{
							GUI::PushStyleColor(GUI::StyleCol_TabActive, styleValue.color);
							GUI::PushStyleColor(GUI::StyleCol_TabUnfocusedActive, styleValue.color);
							pushedData.pushedColors += 2;
						}
					}
					
					if ((styleValue.state & CStateFlag::Disabled) != 0)
					{
						GUI::PushStyleVar(GUI::StyleVar_DisabledAlpha, styleValue.color.a);
						pushedData.pushedVars += 1;
					}
					
					if ((styleValue.state & CStateFlag::Unfocused) != 0)
					{
						if (styleValue.subControl == CSubControl::Tab)
						{
							GUI::PushStyleColor(GUI::StyleCol_TabUnfocused, styleValue.color);
							pushedData.pushedColors += 1;
						}
					}
				}
				else if (property == CStylePropertyType::ForegroundColor && styleValue.valueType == CStyleValue::Type_Color)
				{
					GUI::PushStyleColor(GUI::StyleCol_Text, styleValue.color);
					pushedData.pushedColors += 1;
				}
			}
		}

		pushedStack.Push(pushedData);
	}

	void CStyle::Pop()
	{
		if (pushedStack.IsEmpty())
			return;

		GUI::PopStyleColor(pushedStack.Top().pushedColors);
		GUI::PopStyleVar(pushedStack.Top().pushedVars);
		pushedStack.Pop();
	}

	void CStyle::ApplyStyle(const CStyle& from)
	{
		for (const auto& [property, fromArray] : from.styleMap)
		{
			auto& thisArray = styleMap[property];

			for (const auto& fromStyle : fromArray)
			{
				bool matchFound = false;

				for (int i = thisArray.GetSize() - 1; i >= 0; i--)
				{
					auto& thisStyle = thisArray[i];

					if (thisStyle.valueType == fromStyle.valueType && thisStyle.state == fromStyle.state && thisStyle.subControl == fromStyle.subControl)
					{
						matchFound = true;
						thisStyle = fromStyle;
						break;
					}
				}

				if (!matchFound)
				{
					thisArray.Add(fromStyle);
				}
			}
		}
	}

	
	/*
	static std::unordered_set<CStyleProperty> gInheritedProperties{
			CStyleProperty::ForegroundColor,
			CStyleProperty::ForegroundColor_Disabled,
			CStyleProperty::TextAlignment,
	};

	static std::unordered_set<CStyleProperty> gAutoInitializedProperties{
			
	};

	CStylePropertyFlags CStyle::GetStylePropertyFlags(CStyleProperty property)
	{
		CStylePropertyFlags flags = CStylePropertyFlags::None;
		if (gInheritedProperties.contains(property))
			flags |= CStylePropertyFlags::Inherited;
		else
			flags |= CStylePropertyFlags::NonInherited;

		return flags;
	}

	CStyleProperty CStyle::GetAllProperties(CStylePropertyFlags flags)
	{
		return CStyleProperty();
	}

    Array<GUI::StyleVar> CStyle::GetStyleVar(CStyleProperty variable)
    {
        static HashMap<CStyleProperty, Array<GUI::StyleVar>> map{
            { CStyleProperty::Opacity, { GUI::StyleVar_Alpha } },
			{ CStyleProperty::DisabledOpacity, { GUI::StyleVar_DisabledAlpha } }
        };
        
        if (!map.KeyExists(variable))
            return {};
        
        return map[variable];
    }

    Array<GUI::StyleCol> CStyle::GetStyleColorVar(CStyleProperty variable)
    {
        static HashMap<CStyleProperty, Array<GUI::StyleCol>> map{
            { CStyleProperty::ForegroundColor, { GUI::StyleCol_Text } },
            { CStyleProperty::BackgroundColor,
				{ GUI::StyleCol_FrameBg, GUI::StyleCol_WindowBg, GUI::StyleCol_MenuBarBg, GUI::StyleCol_ChildBg, GUI::StyleCol_Button } },
			{ CStyleProperty::BackgroundColor_Hovered, { GUI::StyleCol_ButtonHovered } },
			{ CStyleProperty::BackgroundColor_Pressed, { GUI::StyleCol_ButtonActive } },
			{ CStyleProperty::ForegroundColor_Disabled, { GUI::StyleCol_TextDisabled } },
        };
        
        if (!map.KeyExists(variable))
            return {};
        
        return map[variable];
    }

    void CStyle::AddProperty(CStyleProperty property, const CStyleValue& styleVar)
    {
        styleMap[property] = styleVar;
    }

    void CStyle::RemoveProperty(CStyleProperty property)
    {
        styleMap.Remove(property);
    }

	CStyleValue& CStyle::GetProperty(CStyleProperty property)
	{
		return styleMap[property];
	}

	bool CStyle::HasProperty(CStyleProperty property)
	{
		return styleMap.KeyExists(property);
	}

	void CStyle::ApplyStyle(const CStyle& style)
	{
		for (const auto& [property, value] : style.styleMap)
		{
			this->styleMap[property] = value;
		}
	}

	CStyleManager::CStyleManager()
	{
	}

	CStyleManager::~CStyleManager()
	{
	}

	void CStyleManager::PushGlobal()
	{
		for (const auto& [variable, value] : globalStyle.styleMap)
		{
			auto styleVars = CStyle::GetStyleVar(variable);
			for (GUI::StyleVar styleVar : styleVars)
			{
				if (styleVar != GUI::StyleVar_COUNT)
				{
					if (GUI::IsStyleVarOfVectorType(styleVar))
						GUI::PushStyleVar(styleVar, (Vec2)value.vector);
					else
						GUI::PushStyleVar(styleVar, value.single);
					pushedVars++;
				}
			}
			auto styleColors = CStyle::GetStyleColorVar(variable);
			for (GUI::StyleCol styleCol : styleColors)
			{
				if (styleCol != GUI::StyleCol_COUNT)
				{
					GUI::PushStyleColor(styleCol, value.color);
					pushedColors++;
				}
			}
		}
	}

	void CStyleManager::PopGlobal()
	{
		if (pushedColors > 0)
			GUI::PopStyleColor(pushedColors);
		if (pushedVars > 0)
			GUI::PopStyleVar(pushedVars);
		pushedColors = pushedVars = 0;
	}*/

} // namespace CE::Widgets

