#include "CoreRPI.h"

namespace CE::RPI
{
	void SubMesh::Clear()
	{
		materialSlotIndex = 0;
		indices.Clear();
	}

	void SubMesh::Release()
	{
		Clear();
	}

	void Mesh::Clear()
	{
		vertices.Clear();
		uvCoords.Clear();
		normals.Clear();
		tangents.Clear();
		subMeshes.Clear();
		vertexColors.Clear();
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
