
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
		if (string.GetLength() < 2)
			return result;
		
		String name{};
		name.Reserve(string.GetLength());
		String numberString{};
		
		for (int i = 0; i < string.GetLength(); i++)
		{
			if (String::IsNumeric(string[i]))
			{

			}
			else if (String::IsAlphabet(string[i]))
			{
				name.Append(string[i]);
			}
		}

		return result;
	}
}
