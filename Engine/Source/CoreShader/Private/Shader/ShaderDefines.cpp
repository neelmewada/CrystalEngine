#include "CoreShader.h"

namespace CE
{
	CORESHADER_API SIZE_T GetVertexInputTypeSize(VertexInputAttribute input)
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

    SRGEntry& ShaderReflection::FindOrAdd(u32 frequencyId)
    {
		for (auto& entry : resourceGroups)
		{
			if (entry.GetFrequencyId() == frequencyId)
				return entry;
		}

		SRGEntry entry{};
		entry.frequencyId = frequencyId;
		resourceGroups.Add(entry);

		return resourceGroups.Top();
    }

} // namespace CE
