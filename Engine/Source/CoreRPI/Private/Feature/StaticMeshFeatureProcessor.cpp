#include "CoreRPI.h"

namespace CE::RPI
{

	StaticMeshFeatureProcessor::StaticMeshFeatureProcessor()
	{
		
	}

	StaticMeshFeatureProcessor::~StaticMeshFeatureProcessor()
	{
		auto parallelRanges = modelInstances.GetParallelRanges();

		for (const auto& range : parallelRanges)
		{
			for (auto it = range.begin; it != range.end; ++it)
			{
				it->Deinit(this);
			}
		}
	}

	void ModelDataInstance::Init(StaticMeshFeatureProcessor* fp)
	{
		if (flags.initialized)
			return;
		
		flags.initialized = true;

		for (int i = 0; i < objectBuffers.GetSize(); ++i)
		{
			RHI::BufferDescriptor bufferDescriptor{};
			bufferDescriptor.name = "ObjectBuffer";
			bufferDescriptor.bindFlags = BufferBindFlags::ConstantBuffer;
			bufferDescriptor.bufferSize = sizeof(Matrix4x4);
			bufferDescriptor.defaultHeapType = MemoryHeapType::Upload;
			bufferDescriptor.structureByteStride = sizeof(Matrix4x4);

			objectBuffers[i] = gDynamicRHI->CreateBuffer(bufferDescriptor);

			Matrix4x4 defaultValue = Matrix4x4::Identity();

			objectBuffers[i]->UploadData(&defaultValue, sizeof(defaultValue));
		}

		for (int i = 0; i < model->GetModelLodCount(); ++i)
		{
			BuildDrawPacketList(fp, i);
		}
	}

	void ModelDataInstance::Deinit(StaticMeshFeatureProcessor* fp)
	{
		// TODO: Set objectBuffer destruction to queue instead of deleting it immediately

		for (auto& objectBuffer : objectBuffers)
		{
			RPISystem::Get().EnqueueDestroy(objectBuffer);
			objectBuffer = nullptr;
		}

		for (auto& srg : objectSrgList)
		{
			delete srg; srg = nullptr;
		}

		objectSrgList.Clear();

		flags.initialized = false;
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
			CustomMaterialId materialId = CustomMaterialId(modelLodIndex, i);
			RPI::Material* material = materialMap[materialId];
			if (material == nullptr)
			{
				material = materialMap[DefaultCustomMaterialId];
				if (material == nullptr)
				{
					continue;
				}
			}

			const auto& objectSrgLayout = material->GetCurrentShader()->GetDefaultVariant()->GetSrgLayout(SRGType::PerObject);

			RHI::ShaderResourceGroup* objectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(objectSrgLayout);

			for (int j = 0; j < objectBuffers.GetSize(); ++j)
			{
				objectSrg->Bind(j, "_ObjectData", objectBuffers[j]);
			}

			objectSrg->FlushBindings();

			objectSrgList.Add(objectSrg);

			RPI::MeshDrawPacket& packet = drawPacketList.EmplaceBack(lod, modelLodIndex, objectSrg, material);
			
			packet.SetStencilRef(0);
			packet.Update(scene, false);
		}
	}

	void ModelDataInstance::UpdateDrawPackets(StaticMeshFeatureProcessor* fp, bool forceUpdate)
	{
		for (int i = 0; i < drawPacketsListByLod.GetSize(); ++i)
		{
			for (int j = 0; j < drawPacketsListByLod[i].GetSize(); ++j)
			{
				drawPacketsListByLod[i][j].Update(scene, forceUpdate);
			}
		}
	}

	void ModelDataInstance::UpdateSrgs(int imageIndex)
	{
		objectBuffers[imageIndex]->UploadData(&localToWorldTransform, sizeof(Matrix4x4));
	}

	ModelHandle StaticMeshFeatureProcessor::AcquireMesh(const ModelHandleDescriptor& modelHandleDescriptor, const CustomMaterialMap& materialMap)
	{
		ModelHandle handle = modelInstances.Insert({});
		handle->model = modelHandleDescriptor.model;
		handle->originalModel = modelHandleDescriptor.originalModel;
		handle->scene = this->scene;
		handle->materialMap = materialMap;
		
		return handle;
	}

	ModelHandle StaticMeshFeatureProcessor::AcquireMesh(const ModelHandleDescriptor& modelHandleDescriptor,
		RPI::Material* defaultMaterial)
	{
		if (defaultMaterial == nullptr)
		{
			return ModelHandle();
		}

		CustomMaterialMap materialMap{};
		materialMap[DefaultCustomMaterialId] = defaultMaterial;

		return AcquireMesh(modelHandleDescriptor, materialMap);
	}

	bool StaticMeshFeatureProcessor::ReleaseMesh(ModelHandle& handle)
	{
		if (handle.IsValid())
		{
			handle->Deinit(this);
			modelInstances.Remove(handle);
			return true;
		}

		return false;
	}

	void StaticMeshFeatureProcessor::Simulate(const SimulatePacket& packet)
	{
		Super::Simulate(packet);

		JobCompletion jobCompletion = JobCompletion();

		Array<Job*> initJobs = CreateInitJobs();

		if (initJobs.GetSize() == 1)
		{
			initJobs[0]->Process();
			initJobs[0]->Finish();
			delete initJobs[0];
			initJobs.Clear();
		}
		else if (initJobs.GetSize() > 1)
		{
			for (Job* initJob : initJobs)
			{
				initJob->SetDependent(&jobCompletion);
				initJob->Start();
			}
		}

		jobCompletion.StartAndWaitForCompletion();

		forceRebuildDrawPackets = false;
	}

	void StaticMeshFeatureProcessor::Render(const RenderPacket& packet)
	{
		Super::Render(packet);

		auto parallelRanges = modelInstances.GetParallelRanges();

		JobCompletion jobCompletion{};
		int imageIndex = packet.imageIndex;

		// - Enqueue Draw Packets to relevant views -

		for (View* view : packet.views)
		{
			for (const auto& range : parallelRanges)
			{
				Job* jobFunction = new JobFunction([range, view, imageIndex](Job* job)
					{
						for (auto it = range.begin; it != range.end; ++it)
						{
							if (it->drawPacketsListByLod.IsEmpty())
								continue;
							if (!it->flags.visible)
								continue;

							it->UpdateSrgs(imageIndex);

							for (RHI::ShaderResourceGroup* objectSrg : it->objectSrgList)
							{
								objectSrg->FlushBindings();
							}

							const auto& meshDrawPacketList = it->drawPacketsListByLod[0];
							RHI::DrawPacket* drawPacket = meshDrawPacketList[0].GetDrawPacket();
							view->AddDrawPacket(drawPacket, 0);
						}
					}, true);

				jobFunction->SetDependent(&jobCompletion);
				jobFunction->Start();
			}
		}

		jobCompletion.StartAndWaitForCompletion();
	}

	void StaticMeshFeatureProcessor::OnRenderEnd()
	{
		Super::OnRenderEnd();

		
	}

	Array<Job*> StaticMeshFeatureProcessor::CreateInitJobs()
	{
		Array<Job*> jobs{};

		auto ranges = modelInstances.GetParallelRanges();

		for (const auto& range : ranges)
		{
			const auto initJobLambda = [this, range](Job* job)
				{
					for (auto it = range.begin; it != range.end; ++it)
					{
						if (!it->flags.visible)
						{
							continue;
						}

						if (!it->flags.initialized)
						{
							it->Init(this);
						}
						
						it->UpdateDrawPackets(this, forceRebuildDrawPackets);
					}
				};

			jobs.Add(new JobFunction(initJobLambda, true));
		}

		return jobs;
	}

} // namespace CE::RPI
