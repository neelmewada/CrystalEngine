#include "CoreRPI.h"

namespace CE::RPI
{


	ModelLod::ModelLod()
	{

	}

	ModelLod::~ModelLod()
	{
		for (auto buffer : trackedBuffers)
		{
			RHI::gDynamicRHI->DestroyBuffer(buffer);
		}
		trackedBuffers.Clear();
	}

	void ModelLod::TrackBuffer(RHI::Buffer* buffer)
	{
		if (!trackedBuffers.Exists(buffer))
		{
			trackedBuffers.Add(buffer);
		}
	}

} // namespace CE::RPI
