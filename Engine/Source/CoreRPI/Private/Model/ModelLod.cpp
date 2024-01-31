#include "CoreRPI.h"

namespace CE::RPI
{


	ModelLod::ModelLod()
	{

	}

	ModelLod::~ModelLod()
	{
		for (auto buffer : vertexBuffers)
		{
			delete buffer;
		}
		vertexBuffers.Clear();

		for (auto buffer : indexBuffers)
		{
			delete buffer;
		}
		indexBuffers.Clear();
	}

	void ModelLod::AddMesh(const Mesh& mesh)
	{
		RHI::Buffer* buffer = mesh.indexBufferView.GetBuffer();

		if (buffer == nullptr)
			return;

		if (!indexBuffers.Exists(buffer))
		{
			indexBuffers.Add(buffer);
		}

		meshes.Add(mesh);
	}

	void ModelLod::AddVertexBuffer(RHI::Buffer* buffer)
	{
		if (buffer && !vertexBuffers.Exists(buffer))
		{
			vertexBuffers.Add(buffer);
		}
	}

} // namespace CE::RPI
