#pragma once

namespace CE::Widgets
{

    enum class CStyleVarType
    {
        None = 0,
        Alpha,
        Padding,
    };
    
    STRUCT()
    struct COREWIDGETS_API CStyleValue
    {
        CE_STRUCT(CStyleValue)
    public:
        CStyleValue()
        {}
        
        CStyleValue(f32 single);
        
        CStyleValue(const Vec4& vector);
        
        CStyleValue(const Color& color);
        
        union
        {
            f32 single;
            
            FIELD()
            Vec4 vector{};
            
            Color color;
        };
        
        FIELD()
        b8 isSingle = false;
        
        FIELD()
        b8 isVector = false;
        
        FIELD()
        b8 isColor = false;
        
    private:
        GUI::StyleVar varName = GUI::StyleVar_COUNT;
        GUI::StyleCol colorName = GUI::StyleCol_COUNT;
    };
    
    STRUCT()
    struct COREWIDGETS_API CStyle
    {
        CE_STRUCT(CStyle)
    public:
        CStyle();
        virtual ~CStyle();
        
        static GUI::StyleVar GetStyleVar(CStyleVarType variable);
        static GUI::StyleCol GetStyleColorVar(CStyleVarType variable);
        
        void AddStyleVar(CStyleVarType variableType, const CStyleValue& styleVar);
        void RemoveStyleVar(CStyleVarType variableType);
        
        FIELD()
        HashMap<CStyleVarType, CStyleValue> styles{};
    };

} // namespace CE::Widgets

#include "CStyle.rtti.h"
