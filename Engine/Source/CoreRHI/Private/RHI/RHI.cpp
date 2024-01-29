
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
		case VertexInputAttribute::UV0:
		case VertexInputAttribute::UV1:
		case VertexInputAttribute::UV2:
		case VertexInputAttribute::UV3:
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
}
