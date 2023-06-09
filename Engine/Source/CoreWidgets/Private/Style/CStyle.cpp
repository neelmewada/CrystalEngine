#include "CoreWidgets.h"

namespace CE::Widgets
{

	
	/*CStyleValue::CStyleValue(f32 single) : isSingle(true), single(single)
    {
        
    }

	CStyleValue::CStyleValue(const Vec2& vector) : isVector(true), vector(Vec4(vector.x, vector.y, 0, 0))
	{

	}

    CStyleValue::CStyleValue(const Vec4& vector) : isVector(true), vector(vector)
    {
        
    }

    CStyleValue::CStyleValue(const Color& color) : isColor(true), color(color)
    {
        
    }

	CStyleValue::CStyleValue(std::initializer_list<Gradient::Key> gradient)
		: isGradient(true), gradient(gradient)
	{
	}

	CStyleValue::CStyleValue(const Gradient& gradient)
		: isGradient(true), gradient(gradient)
	{
	}
    
    CStyle::CStyle()
    {
        
    }

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
