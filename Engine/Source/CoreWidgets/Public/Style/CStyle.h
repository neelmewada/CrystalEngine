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
	enum class Alignment
	{
		Inherited,
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight
	};

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
			PointSize,
			PercentSize
		};

		CStyleValue();

		CStyleValue(EnumValue enumValue);
		

		CStyleValue(const CStyleValue& copy);

		// Called upon destruction
		void Release();

		bool IsValid() const { return valueType != Type_None; }
		

		FIELD()
		CStateFlag states{}; // Style for specific states (multiple states possible)

		FIELD()
		CSubControl subControl{}; // Style for a specific subcontrol

		FIELD()
		int enumValue{}; // enum EnumValue

		FIELD()
		int valueType{}; // enum ValueType

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

		FIELD()
		HashMap<CStylePropertyType, Array<CStyleValue>> styleMap{};

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
