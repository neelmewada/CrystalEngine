#include "CoreWidgets.h"

namespace CE::Widgets
{
	struct CStylePropertyMeta
	{
		CStylePropertyType property{};
		TypeInfo* valueType = nullptr;
		b8 affectsLayout = false;
	};

	static Array<CStylePropertyMeta> propertyMetas = {
		{ CStylePropertyType::Opacity, TYPE(f32), false },
		{ CStylePropertyType::Padding, TYPE(f32), true },
		{ CStylePropertyType::Margin, TYPE(f32), true },
		{ CStylePropertyType::Position, TYPE(CPosition), true },
		{ CStylePropertyType::Display, TYPE(CDisplay), true },
		{ CStylePropertyType::Left, TYPE(f32), true },
		{ CStylePropertyType::Top, TYPE(f32), true },
		{ CStylePropertyType::Right, TYPE(f32), true },
		{ CStylePropertyType::Bottom, TYPE(f32), true },
		{ CStylePropertyType::ImageTint, TYPE(Color), false },
		{ CStylePropertyType::Foreground, TYPE(Color), false },
		{ CStylePropertyType::Background, TYPE(Color), false },
        { CStylePropertyType::BackgroundImage, TYPE(String), false },
		{ CStylePropertyType::BackgroundSize, TYPE(Vec2), false },
		{ CStylePropertyType::BorderRadius, TYPE(Vec4), false },
		{ CStylePropertyType::BorderWidth, TYPE(Vec4), false },
		{ CStylePropertyType::BorderColor, TYPE(Color), false },
		{ CStylePropertyType::ShadowColor, TYPE(Color), false },
		{ CStylePropertyType::ShadowOffset, TYPE(Vec2), false },
		{ CStylePropertyType::TextAlign, TYPE(CTextAlign), false },
		{ CStylePropertyType::Width, TYPE(f32), true },
		{ CStylePropertyType::Height, TYPE(f32), true },
		{ CStylePropertyType::MinWidth, TYPE(f32), true },
		{ CStylePropertyType::MinHeight, TYPE(f32), true },
		{ CStylePropertyType::MaxWidth, TYPE(f32), true },
		{ CStylePropertyType::MaxHeight, TYPE(f32), true },
		{ CStylePropertyType::Spacing, TYPE(f32), true },
		{ CStylePropertyType::FontSize, TYPE(f32), true },
		{ CStylePropertyType::FontName, TYPE(String), true },
		{ CStylePropertyType::AlignContent, TYPE(CAlign), true },
		{ CStylePropertyType::AlignItems, TYPE(CAlign), true },
		{ CStylePropertyType::AlignSelf, TYPE(CAlign), true },
		{ CStylePropertyType::JustifyContent, TYPE(CJustify), true },
		{ CStylePropertyType::Flex, TYPE(f32), true },
		{ CStylePropertyType::FlexBasis, TYPE(f32), true },
		{ CStylePropertyType::FlexFlow, TYPE(f32), true },
		{ CStylePropertyType::FlexWrap, TYPE(CFlexWrap), true },
		{ CStylePropertyType::FlexGrow, TYPE(f32), true },
		{ CStylePropertyType::FlexShrink, TYPE(f32), true },
		{ CStylePropertyType::FlexDirection, TYPE(CFlexDirection), true },
		{ CStylePropertyType::FlexOrder, TYPE(f32), true },
		{ CStylePropertyType::RowGap, TYPE(f32), true },
		{ CStylePropertyType::ColumnGap, TYPE(f32), true },

	};

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

	const Array<CStylePropertyType>& CStyle::GetInheritedProperties()
	{
		static Array<CStylePropertyType> inheritedProperties{
			CStylePropertyType::Foreground,
			CStylePropertyType::TextAlign,
			CStylePropertyType::FontSize,
			CStylePropertyType::FontName,
			CStylePropertyType::Cursor,
			CStylePropertyType::WordWrap,
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

	TypeInfo* CStyle::GetPropertyValueType(CStylePropertyType property)
	{
		return nullptr;
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

	bool CStyle::IsValidProperty(CStylePropertyType property, CStyleValue::ValueType type)
	{
		return properties.KeyExists(property) && properties[property].IsValid() && (type == CStyleValue::Type_None || type == properties[property].valueType);
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

	TypeInfo* CStyle::GetValueTypeForProperty(CStylePropertyType property)
	{
		int index = propertyMetas.IndexOf([=](const CStylePropertyMeta& meta) -> bool { return meta.property == property; });
		if (index >= 0)
		{
			return propertyMetas[index].valueType;
		}
		return nullptr;
	}

    bool CStyle::IsLayoutProperty(CStylePropertyType property)
    {
		return propertyMetas.Exists([](const CStylePropertyMeta& meta) -> bool { return meta.affectsLayout; });
    }

	static HashMap<String, CStylePropertyType> stringToStylePropertyTypeMap{
		{ "opacity", CStylePropertyType::Opacity },
		{ "padding", CStylePropertyType::Padding },
		{ "margin", CStylePropertyType::Margin },
		{ "padding", CStylePropertyType::Padding },
		{ "display", CStylePropertyType::Display },
		{ "color", CStylePropertyType::Foreground },
		{ "foreground", CStylePropertyType::Foreground },
		{ "foreground-color", CStylePropertyType::Foreground },
		{ "background", CStylePropertyType::Background },
		{ "background-color", CStylePropertyType::Background },
        { "background-image", CStylePropertyType::BackgroundImage },
		{ "border-radius", CStylePropertyType::BorderRadius },
		{ "border-width", CStylePropertyType::BorderWidth },
		{ "border-color", CStylePropertyType::BorderColor },
		{ "image-tint", CStylePropertyType::ImageTint }, { "tint", CStylePropertyType::ImageTint },
		{ "shadow", CStylePropertyType::ShadowColor }, { "box-shadow", CStylePropertyType::ShadowColor }, { "shadow-color", CStylePropertyType::ShadowColor },
		{ "background-size", CStylePropertyType::BackgroundSize },
		{ "shadow-offset", CStylePropertyType::ShadowOffset },
		{ "text-align", CStylePropertyType::TextAlign },
		{ "word-wrap", CStylePropertyType::WordWrap },
		{ "width", CStylePropertyType::Width },
		{ "height", CStylePropertyType::Height },
		{ "min-width", CStylePropertyType::MinWidth },
		{ "min-height", CStylePropertyType::MinHeight },
		{ "max-width", CStylePropertyType::MaxWidth },
		{ "max-height", CStylePropertyType::MaxHeight },
		{ "font-size", CStylePropertyType::FontSize },
		{ "font-name", CStylePropertyType::FontName},
		{ "font", CStylePropertyType::FontName},
		{ "cursor", CStylePropertyType::Cursor },
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
		{ CStylePropertyType::TextAlign, GetStaticEnum<CTextAlign>() },
		{ CStylePropertyType::AlignContent, GetStaticEnum<CAlign>() },
		{ CStylePropertyType::AlignItems, GetStaticEnum<CAlign>() },
		{ CStylePropertyType::AlignSelf, GetStaticEnum<CAlign>() },
		{ CStylePropertyType::JustifyContent, GetStaticEnum<CJustify>() },
		{ CStylePropertyType::FlexWrap, GetStaticEnum<CFlexWrap>() },
		{ CStylePropertyType::FlexDirection, GetStaticEnum<CFlexDirection>() },
		{ CStylePropertyType::Display, GetStaticEnum<CDisplay>() },
		{ CStylePropertyType::Cursor, GetStaticEnum<CCursor>() },
		{ CStylePropertyType::WordWrap, GetStaticEnum<CWordWrap>() }
	};

	COREWIDGETS_API EnumType* GetEnumTypeForProperty(CStylePropertyType property)
	{
		return propertyTypeToEnumMap[property];
	}

	static HashMap<String, CTextAlign> stringToAlignmentMap{
		{ "auto", CTextAlign::Inherited },
		{ "inherited", CTextAlign::Inherited },
		{ "top-left", CTextAlign::TopLeft },
		{ "top-center", CTextAlign::TopCenter },
		{ "top-right", CTextAlign::TopRight },
		{ "middle-left", CTextAlign::MiddleLeft },
		{ "middle-center", CTextAlign::MiddleCenter },
		{ "middle-right", CTextAlign::MiddleRight },
		{ "bottom-left", CTextAlign::BottomLeft },
		{ "bottom-center", CTextAlign::BottomCenter },
		{ "bottom-right", CTextAlign::BottomRight },
	};

	COREWIDGETS_API CTextAlign StringToAlignment(const String& string)
	{
		return stringToAlignmentMap[string];
	}

} // namespace CE::Widgets


