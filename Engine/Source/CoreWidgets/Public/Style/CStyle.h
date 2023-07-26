#pragma once

#include "FlexEnums.h"

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

	COREWIDGETS_API CStateFlag StateFlagsFromString(const String& string);

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
		Hint,
		Alternate,

		Cell,

		Any,
	};
	ENUM_CLASS_FLAGS(CSubControl);

	COREWIDGETS_API CSubControl SubControlFromString(const String& string);

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
		Foreground, // Inherited
		Background,
        BackgroundImage,
		BorderRadius,
		BorderWidth,
		BorderColor,
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

	COREWIDGETS_API CStylePropertyType StylePropertyTypeFromString(const String& string);
	COREWIDGETS_API EnumType* GetEnumTypeForProperty(CStylePropertyType property);

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
	enum class Alignment : int
	{
		Inherited = 0,
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight
	};
	ENUM_CLASS_FLAGS(Alignment);

	COREWIDGETS_API Alignment StringToAlignment(const String& string);

	STRUCT()
	struct COREWIDGETS_API CStyleValue
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
			Type_Gradient,
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
			CStyleValue(TEnum enumValue) : enumValue(Vec4i(1, 1, 1, 1) * (int)enumValue), valueType(Type_Enum)
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
		CStyleValue(const Gradient& gradient);
		CStyleValue(const String& string);

		CStyleValue(const CStyleValue& copy);

		CStyleValue& operator=(const CStyleValue& copy);

		CStyleValue(CStyleValue&& move);

		void CopyFrom(const CStyleValue& copy);

		inline bool IsOfType(ValueType checkType) const { return valueType == checkType; }

		inline bool IsPercentValue() const { return enumValue == Vec4i(1, 1, 1, 1) * Percent; }
		inline bool IsAutoValue() const { return IsEnum() && enumValue == Vec4i(1, 1, 1, 1) * Auto; }

		inline bool IsValid() const { return valueType != Type_None; }
		inline bool IsEnum() const { return IsOfType(Type_Enum); }
		inline bool IsSingle() const { return IsOfType(Type_Single); }
		inline bool IsVector() const { return IsOfType(Type_Vector); }
		inline bool IsColor() const { return IsOfType(Type_Color); }
		inline bool IsGradient() const { return IsOfType(Type_Gradient); }
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

		// Fields

		union
		{
			f32 single;
			Vec4 vector{};
			Color color;
			Gradient gradient;
			String string;
		};

		FIELD()
		CStateFlag state{}; // Style for a single specific state

		FIELD()
		CSubControl subControl{}; // Style for a specific subcontrol

		FIELD()
		int valueType = 0;

		FIELD()
		Vec4i enumValue{};

	};

	class CStyleValueVariants : public Array<CStyleValue>
	{
	public:

		CStyleValue& Get(CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None)
		{
			static CStyleValue invalid{};

			for (int i = this->GetSize() - 1; i >= 0; i--)
			{
				auto& item = At(i);
				if (item.state == state && (item.subControl == CSubControl::Any || item.subControl == subControl))
					return item;
			}

			return invalid;
		}

		const CStyleValue& Get(CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) const
		{
			static CStyleValue invalid{};

			for (int i = this->GetSize() - 1; i >= 0; i--)
			{
				auto& item = At(i);
				if (item.state == state && (item.subControl == CSubControl::Any || item.subControl == subControl))
					return item;
			}

			return invalid;
		}

	};

	STRUCT()
	struct COREWIDGETS_API CStyle
	{
		CE_STRUCT(CStyle)
	public:

		CStyle();
		void Release();
		
		static const Array<CStylePropertyType>& GetInheritedProperties();
		static bool IsInheritedProperty(CStylePropertyType property);
		static CStylePropertyTypeFlags GetPropertyTypeFlags(CStylePropertyType property);

		CStyleValue& AddProperty(CStylePropertyType property, const CStyleValue& value);

		CStyleValue& GetProperty(CStylePropertyType property);

		inline bool IsDirty() const { return isDirty; }
		inline void SetDirty(bool dirty = true) { isDirty = dirty; }

		void ApplyProperties(const CStyle& from);
		
		// Fields

		HashMap<CStylePropertyType, CStyleValue> properties{};

	private:

		b8 isDirty = true;
	};
    
} // namespace CE::Widgets


#include "CStyle.rtti.h"
