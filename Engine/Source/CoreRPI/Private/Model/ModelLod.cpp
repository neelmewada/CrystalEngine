#include "CoreRPI.h"

namespace CE::RPI
{

	void Mesh::Clear()
	{
		vertices.Clear();
		uvCoords.Clear();
		normals.Clear();
		tangents.Clear();
		vertexColors.Clear();
		indices.Clear();
	}

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
