#include "CoreWidgets.h"

namespace CE::Widgets
{

    CStyleValue::CStyleValue(f32 single) : isSingle(true), single(single)
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

	CStylePropertyFlags CStyle::GetStylePropertyFlags(CStyleProperty property)
	{
		static std::unordered_set<CStyleProperty> inheritedProperties{
			CStyleProperty::ForegroundColor,
			CStyleProperty::ForegroundColor_Disabled,
		};

		return inheritedProperties.contains(property) ? CStylePropertyFlags::Inherited : CStylePropertyFlags::NonInherited;
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

    void CStyle::AddStyleProperty(CStyleProperty variableType, const CStyleValue& styleVar)
    {
        styleMap[variableType] = styleVar;
    }

    void CStyle::RemoveStyleProperty(CStyleProperty variableType)
    {
        styleMap.Remove(variableType);
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

} // namespace CE::Widgets
