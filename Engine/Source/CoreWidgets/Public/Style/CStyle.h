#pragma once

namespace CE::Widgets
{
	class CWidget;

	ENUM()
    enum class CStyleProperty
    {
        None = 0,
        Opacity,
		DisabledOpacity,
        Padding,
        ForegroundColor, // Inherited
		ForegroundColor_Disabled, // Inherited
        BackgroundColor,
		BackgroundColor_Hovered,
		BackgroundColor_Pressed,
        BorderRadius,
		TextAlignment, // Inherited
		Size,
    };

	ENUM()
	enum class Alignment
	{
		Inherited,
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight
	};

	ENUM(Flags)
	enum class CStylePropertyFlags
	{
		None = 0,
		Inherited = BIT(0),
		NonInherited = BIT(1),
		All = Inherited | NonInherited
	};
	ENUM_CLASS_FLAGS(CStylePropertyFlags);
    
    STRUCT()
    struct COREWIDGETS_API CStyleValue
    {
        CE_STRUCT(CStyleValue)
    public:
        CStyleValue()
        {}

		template<typename T> requires TIsEnum<T>::Value
		CStyleValue(T enumValue)
		{
			this->enumValue = (s64)enumValue;
			isEnum = true;
		}
        
        CStyleValue(f32 single);

		CStyleValue(const Vec2& vector);
        
        CStyleValue(const Vec4& vector);
        
        CStyleValue(const Color& color);

		CStyleValue(std::initializer_list<Gradient::Key> gradient);

		CStyleValue(const Gradient& gradient);
        
        union
        {
			s64 enumValue;

            f32 single;
            
            FIELD()
            Vec4 vector{};
            
            Color color;
        };

		FIELD()
		Gradient gradient{};

		FIELD()
		b8 isEnum = false;
        
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
        
		static CStylePropertyFlags GetStylePropertyFlags(CStyleProperty styleVar);

        static Array<GUI::StyleVar> GetStyleVar(CStyleProperty variable);
        static Array<GUI::StyleCol> GetStyleColorVar(CStyleProperty variable);
        
        void AddProperty(CStyleProperty property, const CStyleValue& styleVar);
        void RemoveProperty(CStyleProperty property);
		CStyleValue& GetProperty(CStyleProperty property);
        
		void ApplyStyle(const CStyle& style);

	public: // Fields

        FIELD()
        HashMap<CStyleProperty, CStyleValue> styleMap{};

    };

	struct COREWIDGETS_API CStyleSelector
	{
	public:

		String selectorString{};

	private:

	};

	CLASS()
	class COREWIDGETS_API CStyleManager : public Object
	{
		CE_CLASS(CStyleManager, CE::Object)
	public:

		CStyleManager();
		virtual ~CStyleManager();

		void PushGlobal();

		void PopGlobal();

	private:

		CStyle globalStyle{};

		u32 pushedColors = 0;
		u32 pushedVars = 0;
	};

} // namespace CE::Widgets

namespace CE
{
	template<>
	inline SIZE_T GetHash<CE::Widgets::CStyleSelector>(const CE::Widgets::CStyleSelector& value)
	{
		return GetHash<String>(value.selectorString);
	}
}

#include "CStyle.rtti.h"
