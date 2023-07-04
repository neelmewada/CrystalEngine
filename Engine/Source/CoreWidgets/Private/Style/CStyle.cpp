#include "CoreWidgets.h"

namespace CE::Widgets
{

	COREWIDGETS_API CStateFlag StateFlagsFromString(const String& string)
	{
		String pascalCase = string.ToPascalCase();

		return GetStaticEnum<CStateFlag>()->GetConstantValue<CStateFlag>(pascalCase);
	}

	COREWIDGETS_API CSubControl SubControlFromString(const String& string)
	{
		String pascalCase = string.ToPascalCase();

		return GetStaticEnum<CSubControl>()->GetConstantValue<CSubControl>(pascalCase);
	}

	CStyleValue::CStyleValue()
	{

	}

	void CStyleValue::Release()
	{
		if (IsGradient())
		{
			gradient.~Gradient();
		}
		else if (IsString())
		{
			string.~String();
		}

		memset(this, 0, sizeof(Self));
	}

	CStyleValue::CStyleValue(f32 single, bool isPercent)
		: single(single), valueType(Type_Single)
	{
		if (isPercent)
			enumValue = Vec4i(1, 1, 1, 1) * Percent;
	}

	CStyleValue::CStyleValue(const Vec4& vector, bool isPercent)
		: vector(vector), valueType(Type_Vector)
	{
		if (isPercent)
			enumValue = Vec4i(1, 1, 1, 1) * Percent;
	}

	CStyleValue::CStyleValue(const Color& color)
		: color(color), valueType(Type_Color)
	{

	}

	CStyleValue::CStyleValue(const Gradient& gradient)
		: gradient(gradient), valueType(Type_Gradient)
	{
	}

	CStyleValue::CStyleValue(const String& string)
		: string(string), valueType(Type_String)
	{
		
	}

	CStyleValue::CStyleValue(const CStyleValue& copy)
	{
		CopyFrom(copy);
	}

	CStyleValue& CStyleValue::operator=(const CStyleValue& copy)
	{
		CopyFrom(copy);
		return *this;
	}

    CStyleValue::CStyleValue(CStyleValue&& move)
    {
		CopyFrom(move);
		memset(&move, 0, sizeof(Self));
    }

	void CStyleValue::CopyFrom(const CStyleValue& copy)
	{
		if (valueType != copy.valueType)
			Release();

		state = copy.state;
		subControl = copy.subControl;
		valueType = copy.valueType;
		enumValue = copy.enumValue;
		
		if (IsSingle())
		{
			single = copy.single;
		}
		else if (IsVector())
		{
			vector = copy.vector;
		}
		else if (IsColor())
		{
			color = copy.color;
		}
		else if (IsGradient())
		{
			gradient = copy.gradient;
		}
		else if (IsString())
		{
			string = copy.string;
		}
	}

	CStyle::CStyle()
	{

	}

	void CStyle::Release()
	{
		
	}

	Array<CStylePropertyType> CStyle::GetInheritedProperties()
	{
		static Array<CStylePropertyType> inheritedProperties{
			CStylePropertyType::Foreground,
			CStylePropertyType::TextAlign,
			CStylePropertyType::FontSize,
			CStylePropertyType::FontName
		};

		return inheritedProperties;
	}

	CStylePropertyTypeFlags CStyle::GetPropertyTypeFlags(CStylePropertyType type)
	{
		static Array<CStylePropertyType> inheritedProperties = GetInheritedProperties();
		CStylePropertyTypeFlags flags{};

		if (inheritedProperties.Exists(type))
			flags |= CStylePropertyTypeFlags::Inherited;
		else
			flags |= CStylePropertyTypeFlags::NonInherited;

		return flags;
	}

	CStyleValue& CStyle::AddProperty(CStylePropertyType propertyType, const CStyleValue& value)
	{
		properties[propertyType] = value;

		return properties[propertyType];
	}

	CStyleValue& CStyle::GetProperty(CStylePropertyType propertyType)
	{
		return properties[propertyType];
	}

    void CStyle::ApplyProperties(const CStyle& from)
    {
		for (const auto& [property, value] : from.properties)
		{
			properties[property] = value;
		}
    }

	bool CStyle::IsInheritedProperty(CStylePropertyType property)
	{
		return GetInheritedProperties().Exists(property);
	}

	static HashMap<String, CStylePropertyType> stringToStylePropertyTypeMap{
		{ "opacity", CStylePropertyType::Opacity },
		{ "padding", CStylePropertyType::Padding },
		{ "margin", CStylePropertyType::Margin },
		{ "padding", CStylePropertyType::Padding },
		{ "foreground", CStylePropertyType::Foreground },
		{ "foreground-color", CStylePropertyType::Foreground },
		{ "background", CStylePropertyType::Background },
		{ "background-color", CStylePropertyType::Background },
		{ "border-radius", CStylePropertyType::BorderRadius },
		{ "border-width", CStylePropertyType::BorderWidth },
		{ "border-color", CStylePropertyType::BorderColor },
		{ "text-align", CStylePropertyType::TextAlign },
		{ "width", CStylePropertyType::Width },
		{ "height", CStylePropertyType::Height },
		{ "min-width", CStylePropertyType::MinWidth },
		{ "min-height", CStylePropertyType::MinHeight },
		{ "max-width", CStylePropertyType::MaxWidth },
		{ "max-height", CStylePropertyType::MaxHeight },
		{ "font-size", CStylePropertyType::FontSize },
		{ "font-name", CStylePropertyType::FontName},

		{ "align-content", CStylePropertyType::AlignContent },
		{ "align-items", CStylePropertyType::AlignItems },
		{ "align-self", CStylePropertyType::AlignSelf },
		{ "justify-content", CStylePropertyType::JustifyContent },
		{ "flex", CStylePropertyType::Flex },
		{ "flex-basis", CStylePropertyType::FlexBasis },
		{ "flex-wrap", CStylePropertyType::FlexWrap },
		{ "flex-grow", CStylePropertyType::FlexGrow },
		{ "flex-shrink", CStylePropertyType::FlexShrink },
		{ "flex-direction", CStylePropertyType::FlexDirection },
		{ "flex-order", CStylePropertyType::FlexOrder },
		{ "row-gap", CStylePropertyType::RowGap },
		{ "col-gap", CStylePropertyType::ColumnGap }, { "column-gap", CStylePropertyType::ColumnGap },
	};

	COREWIDGETS_API CStylePropertyType StylePropertyTypeFromString(const String& in)
    {
		if (!stringToStylePropertyTypeMap.KeyExists(in))
		{
			String pascalCase = in.ToPascalCase();
			return GetStaticEnum<CStylePropertyType>()->GetConstantValue<CStylePropertyType>(pascalCase);
		}
		return stringToStylePropertyTypeMap[in];
	}

	static HashMap<CStylePropertyType, EnumType*> propertyTypeToEnumMap{
		{ CStylePropertyType::TextAlign, GetStaticEnum<Alignment>() },
		{ CStylePropertyType::AlignContent, GetStaticEnum<CAlign>() },
		{ CStylePropertyType::AlignItems, GetStaticEnum<CAlign>() },
		{ CStylePropertyType::AlignSelf, GetStaticEnum<CAlign>() },
		{ CStylePropertyType::JustifyContent, GetStaticEnum<CJustify>() },
		{ CStylePropertyType::FlexWrap, GetStaticEnum<CFlexWrap>() },
		{ CStylePropertyType::FlexDirection, GetStaticEnum<CFlexDirection>() },
	};

	COREWIDGETS_API EnumType* GetEnumTypeForProperty(CStylePropertyType property)
	{
		return propertyTypeToEnumMap[property];
	}

	static HashMap<String, Alignment> stringToAlignmentMap{
		{ "inherited", Alignment::Inherited },
		{ "top-left", Alignment::TopLeft },
		{ "top-center", Alignment::TopCenter },
		{ "top-right", Alignment::TopRight },
		{ "middle-left", Alignment::MiddleLeft },
		{ "middle-center", Alignment::MiddleCenter },
		{ "middle-right", Alignment::MiddleRight },
		{ "bottom-left", Alignment::BottomLeft },
		{ "bottom-center", Alignment::BottomCenter },
		{ "bottom-right", Alignment::BottomRight },
	};

	COREWIDGETS_API Alignment StringToAlignment(const String& string)
	{
		return stringToAlignmentMap[string];
	}

} // namespace CE::Widgets


