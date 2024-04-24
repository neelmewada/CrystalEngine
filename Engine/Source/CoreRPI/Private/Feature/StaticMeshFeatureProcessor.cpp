#include "CoreRPI.h"

namespace CE::RPI
{

	StaticMeshFeatureProcessor::StaticMeshFeatureProcessor()
	{
		
	}

	StaticMeshFeatureProcessor::~StaticMeshFeatureProcessor()
	{
		
	}

	ModelHandle StaticMeshFeatureProcessor::AcquireMesh(const ModelHandleDescriptor& modelHandleDescriptor)
	{
		ModelHandle handle = modelInstances.Insert({});
		handle->model = modelHandleDescriptor.model;
		handle->originalModel = modelHandleDescriptor.originalModel;
		handle->scene = this->scene;
		
		return handle;
	}

	bool StaticMeshFeatureProcessor::ReleaseMesh(ModelHandle& handle)
	{
		if (handle.IsValid())
		{
			modelInstances.Remove(handle);
			return true;
		}

		return false;
	}

	void StaticMeshFeatureProcessor::Simulate(const SimulatePacket& packet)
	{
		Super::Simulate(packet);

		auto parallelRanges = modelInstances.GetParallelRanges();
		for (const auto& parallelRange : parallelRanges) // TODO: Implement multi-threaded processing
		{
			for (auto it = parallelRange.begin; it != parallelRange.end; ++it)
			{
				for (int i = 0; i < it->drawPacketsListByLod.GetSize(); ++i)
				{
					for (int j = 0; j < it->drawPacketsListByLod[i].GetSize(); ++j)
					{
						it->drawPacketsListByLod[i][j].Update(scene);
					}
				}
			}
		}
	}

	void StaticMeshFeatureProcessor::Render(const RenderPacket& packet)
	{
		Super::Render(packet);

		
	}

	void StaticMeshFeatureProcessor::OnRenderEnd()
	{
		Super::OnRenderEnd();

		
	}

} // namespace CE::RPI
