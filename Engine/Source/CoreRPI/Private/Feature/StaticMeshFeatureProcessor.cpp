#include "CoreRPI.h"

namespace CE::RPI
{

	StaticMeshFeatureProcessor::StaticMeshFeatureProcessor()
	{
		
	}

	StaticMeshFeatureProcessor::~StaticMeshFeatureProcessor()
	{
		
	}

	void ModelDataInstance::Init(StaticMeshFeatureProcessor* fp)
	{
		if (flags.isInitialized)
			return;
		
		flags.isInitialized = true;

		for (int i = 0; i < model->GetModelLodCount(); ++i)
		{
			BuildDrawPacketList(fp, i);
		}
	}

	void ModelDataInstance::Deinit(StaticMeshFeatureProcessor* fp)
	{
		

		for (RHI::ShaderResourceGroup* srg : objectSrgList)
		{
			delete srg;
		}

		objectSrgList.Clear();

		flags.isInitialized = false;
	}

	void ModelDataInstance::BuildDrawPacketList(StaticMeshFeatureProcessor* fp, u32 modelLodIndex)
	{
		ModelLod* lod = model->GetModelLod(modelLodIndex);
		SIZE_T meshCount = lod->GetMeshCount();

		if (drawPacketsListByLod.GetSize() != model->GetModelLodCount())
		{
			drawPacketsListByLod.Resize(model->GetModelLodCount());
		}

		MeshDrawPacketList& drawPacketList = drawPacketsListByLod[modelLodIndex];
		drawPacketList.Clear();

		for (RHI::ShaderResourceGroup* srg : objectSrgList)
		{
			delete srg;
		}

		objectSrgList.Clear();

		for (int i = 0; i < meshCount; i++)
		{
			const auto& objectSrgLayout = material->GetCurrentOpaqueShader()->GetSrgLayout(SRGType::PerObject);

			RHI::ShaderResourceGroup* objectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(objectSrgLayout);
			objectSrgList.Add(objectSrg);

			RPI::MeshDrawPacket& packet = drawPacketList.EmplaceBack(lod, modelLodIndex, objectSrg, material);

			
		}
	}

	void ModelDataInstance::UpdateDrawPackets(StaticMeshFeatureProcessor* fp)
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
