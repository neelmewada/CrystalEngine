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
    
    CStyle::CStyle()
    {
        
    }

    CStyle::~CStyle()
    {
        
    }

    GUI::StyleVar CStyle::GetStyleVar(CStyleVarType variable)
    {
        static HashMap<CStyleVarType, GUI::StyleVar> map{
            { CStyleVarType::Alpha, GUI::StyleVar_Alpha }
        };
        
        if (!map.KeyExists(variable))
            return GUI::StyleVar_COUNT;
        
        return map[variable];
    }

    GUI::StyleCol CStyle::GetStyleColorVar(CStyleVarType variable)
    {
        static HashMap<CStyleVarType, GUI::StyleCol> map{
            
        };
        
        if (!map.KeyExists(variable))
            return GUI::StyleCol_COUNT;
        
        return map[variable];
    }

    void CStyle::AddStyleVar(CStyleVarType variableType, const CStyleValue& styleVar)
    {
        styles[variableType] = styleVar;
    }

    void CStyle::RemoveStyleVar(CStyleVarType variableType)
    {
        styles.Remove(variableType);
    }

} // namespace CE::Widgets
