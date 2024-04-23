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
