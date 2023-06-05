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

    CStyle::~CStyle()
    {
        
    }

    Array<GUI::StyleVar> CStyle::GetStyleVar(CStyleVariable variable)
    {
        static HashMap<CStyleVariable, Array<GUI::StyleVar>> map{
            { CStyleVariable::Opacity, { GUI::StyleVar_Alpha } }
        };
        
        if (!map.KeyExists(variable))
            return {};
        
        return map[variable];
    }

    Array<GUI::StyleCol> CStyle::GetStyleColorVar(CStyleVariable variable)
    {
        static HashMap<CStyleVariable, Array<GUI::StyleCol>> map{
            { CStyleVariable::ForegroundColor, { GUI::StyleCol_Text } },
            { CStyleVariable::BackgroundColor,
                { GUI::StyleCol_FrameBg, GUI::StyleCol_WindowBg, GUI::StyleCol_MenuBarBg, GUI::StyleCol_ChildBg } },
        };
        
        if (!map.KeyExists(variable))
            return {};
        
        return map[variable];
    }

    void CStyle::AddStyleVar(CStyleVariable variableType, const CStyleValue& styleVar)
    {
        styles[variableType] = styleVar;
    }

    void CStyle::RemoveStyleVar(CStyleVariable variableType)
    {
        styles.Remove(variableType);
    }

} // namespace CE::Widgets
