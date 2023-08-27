#include "CoreShader.h"

namespace CE
{

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
