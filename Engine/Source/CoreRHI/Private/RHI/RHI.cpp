
#include "RHI/Resources.h"
#include "RHI/RHI.h"

namespace CE::RHI
{
	CORERHI_API DynamicRHI* gDynamicRHI = nullptr;

	CORERHI_API SIZE_T GetVertexInputTypeSize(VertexInputAttribute input)
	{
		switch (input) {
		case VertexInputAttribute::Position:
			return sizeof(Vec3);
		case VertexInputAttribute::UV:
			return sizeof(Vec2);
		case VertexInputAttribute::Normal:
			return sizeof(Vec3);
		case VertexInputAttribute::Tangent:
			return sizeof(Vec3);
		case VertexInputAttribute::Color:
			return sizeof(Vec4);
		default:
			return 0;
		}

		return 0;
	}

	ShaderSemantic ShaderSemantic::Parse(const String& string)
	{
		ShaderSemantic result{};
		result.index = 0;

		if (string.GetLength() < 2)
			return result;
		
		String name{};
		name.Reserve(string.GetLength());
		String numberString{};
		
		for (int i = 0; i < string.GetLength(); i++)
		{
			if (String::IsNumeric(string[i]))
			{
				numberString.Append(string[i]);
			}
			else if (String::IsAlphabet(string[i]) && numberString.IsEmpty())
			{
				name.Append(string[i]);
			}
		}

		String nameUpper = name.ToUpper();
		EnumType* enumType = GetStaticEnum<VertexInputAttribute>();
		if (!enumType)
			return result;

		for (int i = 0; i < enumType->GetConstantsCount(); i++)
		{
			if (enumType->GetConstant(i)->GetName().GetString().ToUpper() == nameUpper)
			{
				result.attribute = (VertexInputAttribute)enumType->GetConstant(i)->GetValue();
				break;
			}
		}

		if (!numberString.IsEmpty())
		{
			String::TryParse(numberString, result.index);
		}

		return result;
	}

	String ShaderSemantic::ToString() const
	{
		String result{};

		EnumType* enumType = GetStaticEnum<VertexInputAttribute>();
		if (!enumType)
			return result;

		EnumConstant* enumConst = enumType->FindConstantWithValue((s64)attribute);
		if (!enumConst)
			return result;

		result = enumConst->GetName().GetString();

		if (index > 0)
			result.Concatenate((s64)index);

		return result;
	}
}
