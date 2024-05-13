#pragma once

namespace CE::Widgets
{
    ENUM(Flags)
	enum class CStateFlag : u32
	{
		Default = 0,
		On = BIT(0),
		Off = BIT(1),
		Hovered = BIT(2),
		Pressed = BIT(3),
		Focused = BIT(4),
		Horizontal = BIT(5),
		Vertical = BIT(6),
		Disabled = BIT(7),
		Enabled = BIT(8),
		Window = BIT(9),
		Collapsed = BIT(10),
		Active = BIT(11),
		Inactive = BIT(12),
		Major = BIT(13),
		Minor = BIT(14)
	};
	ENUM_CLASS_FLAGS(CStateFlag);

	CRYSTALWIDGETS_API CStateFlag StateFlagsFromString(const String& string);

	ENUM()
	enum class CSubControl
	{
		None = 0,
		Icon,
		Tab,
		Selection,
		TitleBar,
		MenuBar,
		Header,
		Window,
		Frame,
		Check,
		Radio,
		Hint,
		Alternate,
		Splitter,
		ScrollBar,

		Cell,

		Any,
	};
	ENUM_CLASS_FLAGS(CSubControl);

	CRYSTALWIDGETS_API CSubControl SubControlFromString(const String& string);

	ENUM()
	enum class CStylePropertyType
	{
		None = 0,
		Opacity,
		Padding,
		Margin,
		Position,
		Display,
		Left, Top, Right, Bottom,
		ImageTint,
		Foreground, // Inherited
		Background,
        BackgroundImage,
		BackgroundSize,
		BackgroundPosition,
		BorderRadius,
		BorderWidth,
		BorderColor,
		ShadowColor,
		ShadowOffset,
		TextAlign, // Inherited
		Width,
		Height,
		MinWidth,
		MinHeight,
		MaxWidth,
		MaxHeight,
		Spacing,
		FontSize, // Inherited
		FontName, // Inherited
		Cursor, // Inherited
		WordWrap, // Inherited

		// FlexBox properties
		AlignContent,
		AlignItems,
		AlignSelf,
		JustifyContent,
		Flex,
		FlexBasis,
		FlexFlow,
		FlexWrap,
		FlexGrow,
		FlexShrink,
		FlexDirection,
		FlexOrder,
		RowGap,
		ColumnGap,
	};
	ENUM_CLASS_FLAGS(CStylePropertyType);

	CRYSTALWIDGETS_API CStylePropertyType StylePropertyTypeFromString(const String& string);
	CRYSTALWIDGETS_API EnumType* GetEnumTypeForProperty(CStylePropertyType property);

	ENUM()
	enum class CStylePropertyTypeFlags
	{
		None = 0,
		Inherited = BIT(0),
		NonInherited = BIT(1),
		All = Inherited | NonInherited,
	};
	ENUM_CLASS_FLAGS(CStylePropertyTypeFlags);

	ENUM()
	enum class CBackgroundSize : u8
	{
		Fill,
		Cover,
		Contain,
		Auto = Fill
	};
	ENUM_CLASS(CBackgroundSize);

	ENUM()
	enum class CTextAlign : u8
	{
		Inherited = 0,
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight
	};
	ENUM_CLASS(CTextAlign);

	CRYSTALWIDGETS_API CTextAlign StringToAlignment(const String& string);

	ENUM()
	enum class CCursor : u8
	{
		Inherited = 0,
		Arrow,
		Hand,
		Edit,
		SizeTL,
		SizeBR,
		SizeTR,
		SizeBL,
		SizeH,
		SizeV,
		SizeAll,
		Wait
	};
	ENUM_CLASS(CCursor);

	inline SystemCursor ToSystemCursor(CCursor cursor)
	{
		switch (cursor)
		{
		case CCursor::Inherited:
			return SystemCursor::Default;
		case CCursor::Arrow:
			return SystemCursor::Arrow;
		case CCursor::Hand:
			return SystemCursor::Hand;
		case CCursor::Edit:
			return SystemCursor::IBeam;
		case CCursor::SizeTL:
		case CCursor::SizeBR:
			return SystemCursor::SizeTopLeft;
		case CCursor::SizeTR:
		case CCursor::SizeBL:
			return SystemCursor::SizeTopRight;
		case CCursor::SizeH:
			return SystemCursor::SizeHorizontal;
		case CCursor::SizeV:
			return SystemCursor::SizeVertical;
		case CCursor::SizeAll:
			return SystemCursor::SizeAll;
		case CCursor::Wait:
			return SystemCursor::Wait;
		}
		return SystemCursor::Default;
	}

	ENUM()
	enum class CWordWrap : u8
	{
		Inherited = 0,
		Normal,
		BreakWord,
		Clip,
		Ellipsis,
	};
	ENUM_CLASS(CWordWrap);

	STRUCT()
	struct CRYSTALWIDGETS_API CStyleValue
	{
		CE_STRUCT(CStyleValue)
	public:

		enum ValueType : int
		{
			Type_None = 0,
			Type_Enum,
			Type_Single,
			Type_Vector,
			Type_Color,
			Type_String,
		};

		enum EnumValue : int
		{
			None = 0,
			Auto,
			Inherited,
			Initial,
			Percent,
			Absolute,
		};

		CStyleValue();
		void Release();

		template<typename TEnum> requires TIsEnum<TEnum>::Value
		CStyleValue(TEnum enumValue) : enumValue(Vec4i(1, 1, 1, 1)* (int)enumValue), valueType(Type_Enum)
		{

		}

		template<typename TEnum> requires TIsEnum<TEnum>::Value
		CStyleValue(TVector4<TEnum> enumValue4) : enumValue(Vec4i(enumValue4.x, enumValue4.y, enumValue4.z, enumValue4.w)), valueType(Type_Enum)
		{

		}

		template<>
		CStyleValue(EnumValue value) : enumValue(Vec4i(1, 1, 1, 1) * (int)value), valueType(Type_Enum)
		{

		}

		CStyleValue(f32 single, bool isPercent = false);
		CStyleValue(const Vec4& vector, bool isPercent = false);
		CStyleValue(const Color& color);
		CStyleValue(const String& string);

		CStyleValue(const CStyleValue& copy);

		CStyleValue& operator=(const CStyleValue& copy);

		CStyleValue(CStyleValue&& move) noexcept;

		void CopyFrom(const CStyleValue& copy);

		inline bool IsOfType(ValueType checkType) const { return valueType == checkType; }

		inline bool IsPercentValue() const { return enumValue == Vec4i(1, 1, 1, 1) * Percent; }
		inline bool IsAutoValue() const { return IsEnum() && enumValue == Vec4i(1, 1, 1, 1) * Auto; }

		inline bool IsValid() const { return valueType != Type_None; }
		inline bool IsEnum() const { return IsOfType(Type_Enum); }
		inline bool IsSingle() const { return IsOfType(Type_Single); }
		inline bool IsVector() const { return IsOfType(Type_Vector); }
		inline bool IsColor() const { return IsOfType(Type_Color); }
		inline bool IsString() const { return IsOfType(Type_String); }

		bool operator==(const CStyleValue& rhs) const
		{
			if (valueType != rhs.valueType)
				return false;
			if (IsSingle() && single != rhs.single)
				return false;
			if (IsVector() && vector != rhs.vector)
				return false;
			if (IsColor() && color.ToVec4() != rhs.color.ToVec4())
				return false;
			if (IsString() && string != rhs.string)
				return false;
			if (enumValue != rhs.enumValue)
				return false;
			return true;
		}

		bool operator!=(const CStyleValue& rhs) const
		{
			return !(*this == rhs);
		}

		// Modifiers

		CStyleValue& AsAbsolute()
		{
			enumValue = Vec4i(Absolute, Absolute, Absolute, Absolute);
			return *this;
		}

		CStyleValue& AsPercent()
		{
			enumValue = Vec4i(Percent, Percent, Percent, Percent);
			return *this;
		}

		CStyleValue& AsPercent(bool maskX, bool maskY, bool maskZ, bool maskW)
		{
			if (maskX) enumValue.x = Percent;
			if (maskY) enumValue.y = Percent;
			if (maskZ) enumValue.z = Percent;
			if (maskW) enumValue.w = Percent;
			return *this;
		}

		CStyleValue& AsAuto()
		{
			enumValue = Vec4i(Auto, Auto, Auto, Auto);
			return *this;
		}

		CStyleValue& AsAuto(bool maskX, bool maskY, bool maskZ, bool maskW)
		{
			if (maskX) enumValue.x = Auto;
			if (maskY) enumValue.y = Auto;
			if (maskZ) enumValue.z = Auto;
			if (maskW) enumValue.w = Auto;
			return *this;
		}

		CStyleValue& WithState(CStateFlag state)
		{
			this->state = state;
			return *this;
		}

		CStyleValue& WithSubControl(CSubControl subControl)
		{
			this->subControl = subControl;
			return *this;
		}

		FIELD()
		f32 single = 0;

		FIELD()
		Vec4 vector{};

		FIELD()
		Color color = Color();

		FIELD()
		String string = "";

		FIELD()
		Vec4i enumValue{};

		FIELD()
		CStateFlag state{}; // Style for a single specific state

		FIELD()
		CSubControl subControl{}; // Style for a specific subcontrol

		FIELD()
		int valueType = 0;

	};

	struct CRYSTALWIDGETS_API CStyle
	{
	public:

		static const Array<CStylePropertyType>& GetInheritedProperties();
		static bool IsInheritedProperty(CStylePropertyType property);
		static TypeInfo* GetValueTypeForProperty(CStylePropertyType property);
		static bool IsLayoutProperty(CStylePropertyType property);
		static CStylePropertyTypeFlags GetPropertyTypeFlags(CStylePropertyType property);

		void ApplyProperties(const CStyle& from);

		//! Returns true if layout properties are different, else false.
		bool CompareLayoutProperties(const CStyle& rhs);

		Vec4 GetPadding() const
		{
			if (!properties.KeyExists(CStylePropertyType::Padding))
				return Vec4();
			return properties.Get(CStylePropertyType::Padding).vector;
		}

		Name GetFontName() const
		{
			if (!properties.KeyExists(CStylePropertyType::FontName))
				return "Roboto";
			String fontName = properties.Get(CStylePropertyType::FontName).string;
			if (fontName.IsEmptyOrWhiteSpace())
				return "Roboto";
			return fontName;
		}

		f32 GetFontSize() const
		{
			if (!properties.KeyExists(CStylePropertyType::FontSize))
				return 14;
			f32 fontSize = properties.Get(CStylePropertyType::FontSize).single;
			if (fontSize < 1)
				fontSize = 14;
			return fontSize;
		}

		Color GetForegroundColor() const
		{
			if (!properties.KeyExists(CStylePropertyType::Foreground))
				return Color::White();
			return properties.Get(CStylePropertyType::Foreground).color;
		}

		Color GetBackgroundColor() const
		{
			if (!properties.KeyExists(CStylePropertyType::Background))
				return Color::Clear();
			return properties.Get(CStylePropertyType::Background).color;
		}

		Vec4 GetBorderRadius() const
		{
			if (!properties.KeyExists(CStylePropertyType::BorderRadius))
				return Vec4();
			return properties.Get(CStylePropertyType::BorderRadius).vector;
		}

		CBackgroundSize GetBackgroundSize() const
		{
			if (!properties.KeyExists(CStylePropertyType::BackgroundSize))
				return CBackgroundSize::Auto;
			return (CBackgroundSize)properties.Get(CStylePropertyType::BackgroundSize).enumValue.x;
		}

		CTextAlign GetBackgroundPosition() const
		{
			if (!properties.KeyExists(CStylePropertyType::BackgroundPosition))
				return CTextAlign::MiddleCenter;
			return (CTextAlign)properties.Get(CStylePropertyType::BackgroundPosition).enumValue.x;
		}

		HashMap<CStylePropertyType, CStyleValue> properties{};

	};

} // namespace CE::Widgets

#include "CStyle.rtti.h"