#pragma once

namespace CE::Widgets
{
	class CWidget;

	ENUM(Flags)
	enum class CStateFlag
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
	};
	ENUM_CLASS_FLAGS(CStateFlag);

	ENUM()
	enum class CSubControl
	{
		None = 0,
		Icon,
		Tab,
	};
	ENUM_CLASS_FLAGS(CSubControl);

	ENUM()
	enum class CStylePropertyType
	{
		None = 0,
		Opacity,
		Padding,
		ForegroundColor, // Inherited
		Background,
		BorderRadius,
		BorderWidth,
		BorderColor,
		Width,
		Height,
		TextAlign, // Inherited
	};
	ENUM_CLASS_FLAGS(CStylePropertyType);

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
		Inherited,
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight
	};
	ENUM_CLASS_FLAGS(Alignment);

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
			Type_Asset
		};

		enum EnumValue : int
		{
			Auto = 0,
			Inherited,
			Initial,
		};

		CStyleValue();

		template<typename TEnum> requires TIsEnum<TEnum>::Value
		CStyleValue(TEnum enumValue) : enumValue(enumValue), valueType(Type_Enum)
		{

		}

		template<>
		CStyleValue(EnumValue value) : enumValue(value), valueType(Type_Enum)
		{

		}

		CStyleValue(f32 single, bool isPercent = false);
		CStyleValue(const Vec4& vector, bool isPercent = false);
		CStyleValue(const Color& color);
		CStyleValue(const Gradient& gradient);
		CStyleValue(const Name& assetPath);

		CStyleValue(const CStyleValue& copy);

		CStyleValue& operator=(const CStyleValue& copy);

		// Called upon destruction
		void Release();

		bool IsValid() const { return valueType != Type_None; }
		

		FIELD()
		CStateFlag state{}; // Style for a specific state

		FIELD()
		CSubControl subControl{}; // Style for a specific subcontrol

		FIELD()
		int enumValue{}; // Any enum value

		FIELD()
		int valueType{}; // enum ValueType

		FIELD()
		b8 isPercent = false;

		union
		{
			f32 single;
			Vec4 vector{};
			Color color;
			Gradient gradient;
			Name assetPath;
		};


	};

	STRUCT()
	struct COREWIDGETS_API CStyle
	{
		CE_STRUCT(CStyle)
	public:

		static Array<CStylePropertyType> GetInheritedProperties();
		static bool IsInheritedProperty(CStylePropertyType property);
		static CStylePropertyTypeFlags GetPropertyTypeFlags(CStylePropertyType property);

		void AddProperty(CStylePropertyType property, const CStyleValue& value, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None);
		Array<CStyleValue>& GetProperties(CStylePropertyType property);

		CStyleValue& GetProperty(CStylePropertyType property, CStateFlag forState = CStateFlag::Default, CSubControl forSubControl = CSubControl::None);

		void Push(CStylePropertyTypeFlags flags = CStylePropertyTypeFlags::All);
		void Pop();
		
		HashMap<CStylePropertyType, Array<CStyleValue>> styleMap{};

	private:

		struct PushedData
		{
			u32 pushedVars = 0;
			u32 pushedColors = 0;
		};

		Array<PushedData> pushedStack{};
	};

} // namespace CE::Widgets
/*
namespace CE
{
	template<>
	inline SIZE_T GetHash<CE::Widgets::CStyleSelector>(const CE::Widgets::CStyleSelector& value)
	{
		return GetHash<String>(value.selectorString);
	}
}*/

#include "CStyle.rtti.h"
