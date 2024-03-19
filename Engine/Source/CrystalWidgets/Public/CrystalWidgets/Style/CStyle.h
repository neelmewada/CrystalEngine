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
		Unfocused = BIT(10),
		Active = BIT(11),
		Inactive = BIT(12),
		Collapsed = BIT(13),
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
	enum class CTextAlign : u8
	{
		Inherited = 0,
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight
	};
	ENUM_CLASS_FLAGS(CTextAlign);

	CRYSTALWIDGETS_API CTextAlign StringToAlignment(const String& string);

	ENUM()
	enum class CCursor : u8
	{
		Inherited = 0,
		Arrow = 1,
		Hand = 2,
	};
	ENUM_CLASS(CCursor);

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

	crystalwidgets_internal:

		HashMap<CStylePropertyType, CStyleValue> properties{};

	};

} // namespace CE::Widgets

#include "CStyle.rtti.h"