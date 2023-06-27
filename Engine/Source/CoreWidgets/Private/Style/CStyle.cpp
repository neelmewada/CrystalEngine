#include "CoreWidgets.h"

namespace CE::Widgets
{

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
			enumValue = Percent;
	}

	CStyleValue::CStyleValue(const Vec4& vector, bool isPercent)
		: vector(vector), valueType(Type_Vector)
	{
		if (isPercent)
			enumValue = Percent;
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
			CStylePropertyType::ForegroundColor,
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
		auto& styleVariants = properties[propertyType];

		for (int i = styleVariants.GetSize() - 1; i >= 0; i--)
		{
			// Remove 'duplicate-like' states, because they will be fully overriden anyways.
			if (styleVariants[i].state == value.state &&
				styleVariants[i].subControl == value.subControl &&
				styleVariants[i].valueType == value.valueType)
			{
				styleVariants.RemoveAt(i);
			}
		}

		styleVariants.Add(value);
		return styleVariants.Top();
	}

	CStyleValue& CStyle::GetProperty(CStylePropertyType propertyType, CStateFlag forState, CSubControl forSubControl)
	{
		return properties[propertyType].Get(forState, forSubControl);
	}

	bool CStyle::IsInheritedProperty(CStylePropertyType property)
	{
		return GetInheritedProperties().Exists(property);
	}

} // namespace CE::Widgets


