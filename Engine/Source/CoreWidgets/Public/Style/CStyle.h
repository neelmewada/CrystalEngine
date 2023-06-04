#pragma once

namespace CE::Widgets
{

    enum class CStyleVariable
    {
        None = 0,
        Alpha,
        Padding,
        ForegroundColor,
        BackgroundColor,
        BorderRadius,
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

		CStyleValue(std::initializer_list<Gradient::Key> gradient);

		CStyleValue(const Gradient& gradient);
        
        union
        {
            f32 single;
            
            FIELD()
            Vec4 vector{};
            
            Color color;
        };

		FIELD()
		Gradient gradient{};
        
        FIELD()
        b8 isSingle = false;
        
        FIELD()
        b8 isVector = false;
        
        FIELD()
        b8 isColor = false;

		FIELD()
		b8 isGradient = false;
        
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
        
        static Array<GUI::StyleVar> GetStyleVar(CStyleVariable variable);
        static Array<GUI::StyleCol> GetStyleColorVar(CStyleVariable variable);
        
        void AddStyleVar(CStyleVariable variableType, const CStyleValue& styleVar);
        void RemoveStyleVar(CStyleVariable variableType);
        
        FIELD()
        HashMap<CStyleVariable, CStyleValue> styles{};


    };

} // namespace CE::Widgets

#include "CStyle.rtti.h"
