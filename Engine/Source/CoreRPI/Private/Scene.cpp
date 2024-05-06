#include "CoreRPI.h"

namespace CE::RPI
{
    
	Scene::Scene()
	{
		if (RPISystem::Get().isInitialized && gDynamicRHI != nullptr)
		{
			RPISystem::Get().scenes.Add(this);
		}
	}

	Scene::~Scene()
	{
		RHI::FrameScheduler::Get()->WaitUntilIdle();

		if (shaderResourceGroup)
		{
			delete shaderResourceGroup; shaderResourceGroup = nullptr;
		}

		delete directionalLightBuffer; directionalLightBuffer = nullptr;
		delete lightConstantsBuffer; lightConstantsBuffer = nullptr;

		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->Destroy();
		}
		featureProcessors.Clear();

		RPISystem::Get().scenes.Remove(this);
	}

	FeatureProcessor* Scene::AddFeatureProcessor(SubClass<FeatureProcessor> classType)
	{
		if (classType == nullptr)
			return nullptr;

		for (FeatureProcessor* fp : featureProcessors)
		{
			if (fp && fp->IsOfType(classType))
				return fp;
		}

		FeatureProcessor* fp = CreateObject<FeatureProcessor>(GetTransientPackage(MODULE_NAME),
			classType->GetName().GetLastComponent(), OF_Transient, classType);
		featureProcessors.Add(fp);
		fp->scene = this;
		return fp;
	}

	FeatureProcessor* Scene::GetFeatureProcessor(SubClass<FeatureProcessor> classType)
	{
		if (classType == nullptr)
			return nullptr;

		for (FeatureProcessor* fp : featureProcessors)
		{
			if (fp && fp->IsOfType(classType))
				return fp;
		}

		return nullptr;
	}

	void Scene::AddView(SceneViewTag viewTag, ViewPtr view)
	{
		SceneViews& sceneViews = viewsByTag[viewTag];
		sceneViews.viewTag = viewTag;
		sceneViews.drawListMask = view->GetDrawListMask();
		sceneViews.views.Add(view);

		needsLookupTableRebuild = true;
	}

	void Scene::RemoveView(SceneViewTag viewTag, ViewPtr view)
	{
		SceneViews& sceneViews = viewsByTag[viewTag];
		sceneViews.views.Remove(view);

		needsLookupTableRebuild = true;
	}

	void RPI::Scene::AddRenderPipeline(RenderPipeline* renderPipeline)
	{
		if (!renderPipeline)
			return;
		renderPipeline->scene = this;
		renderPipelines.Add(renderPipeline);

		needsLookupTableRebuild = true;
	}

	void RPI::Scene::RemoveRenderPipeline(RenderPipeline* renderPipeline)
	{
		if (!renderPipeline)
			return;
		renderPipeline->scene = nullptr;
		renderPipelines.Remove(renderPipeline);

		needsLookupTableRebuild = true;
	}

	RHI::DrawListMask& Scene::GetDrawListMask(SceneViewTag viewTag)
	{
		return viewsByTag[viewTag].drawListMask;
	}
	
	ArrayView<ViewPtr> Scene::GetViews(const SceneViewTag& viewTag)
	{
		if (!viewsByTag.KeyExists(viewTag))
			return {};

		return viewsByTag[viewTag].views;
	}

	void Scene::Simulate(f32 currentTime)
	{
		if (shaderResourceGroup == nullptr)
		{
			const auto& srgLayout = RPISystem::Get().sceneSrgLayout;
			shaderResourceGroup = gDynamicRHI->CreateShaderResourceGroup(srgLayout);

			RHI::BufferDescriptor lightConstantsDesc{};
			lightConstantsDesc.name = "LightConstants";
			lightConstantsDesc.bindFlags = BufferBindFlags::ConstantBuffer;
			lightConstantsDesc.defaultHeapType = MemoryHeapType::Upload;
			lightConstantsDesc.bufferSize = sizeof(lightConstants);

			lightConstantsBuffer = new RPI::Buffer(lightConstantsDesc);

			lightConstantsBuffer->UploadData(sizeof(lightConstants), 0, &lightConstants);

			for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; ++i)
			{
				shaderResourceGroup->Bind(i, "_LightData", lightConstantsBuffer->GetBuffer(i));
			}

			RHI::BufferDescriptor directionalLightDesc{};
			directionalLightDesc.name = "DirectionalLightConstants";
			directionalLightDesc.bindFlags = BufferBindFlags::ConstantBuffer;
			directionalLightDesc.defaultHeapType = MemoryHeapType::Upload;
			directionalLightDesc.bufferSize = sizeof(directionalLightConstants) * RPI::Limits::MaxDirectionalLightsCount;
			directionalLightDesc.structureByteStride = sizeof(directionalLightConstants);

			directionalLightBuffer = new RPI::Buffer(directionalLightDesc);

			for (int i = 0; i < RHI::Limits::MaxSwapChainImageCount; ++i)
			{
				shaderResourceGroup->Bind(i, "_DirectionalLightsArray", directionalLightBuffer->GetBuffer(i));
			}

			RHI::SamplerDescriptor samplerDesc{};
			samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SamplerAddressMode::ClampToEdge;
			samplerDesc.enableAnisotropy = true;
			samplerDesc.maxAnisotropy = 8;
			samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;

			shaderResourceGroup->Bind("_DefaultSampler", RPISystem::Get().FindOrCreateSampler(samplerDesc));

			shaderResourceGroup->Bind("_BrdfLut", RPISystem::Get().GetBrdfLutTexture()->GetRhiTexture());

			RHI::SamplerDescriptor skyboxSamplerDesc{};
			skyboxSamplerDesc.borderColor = RHI::SamplerBorderColor::FloatOpaqueWhite;
			skyboxSamplerDesc.addressModeU = skyboxSamplerDesc.addressModeV = skyboxSamplerDesc.addressModeW = RHI::SamplerAddressMode::Repeat;
			skyboxSamplerDesc.enableAnisotropy = false;
			skyboxSamplerDesc.maxAnisotropy = 0;
			skyboxSamplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
			RHI::Sampler* skyboxSampler = RPISystem::Get().FindOrCreateSampler(skyboxSamplerDesc);

			if (skyboxCubeMap)
			{
				shaderResourceGroup->Bind("_Skybox", skyboxCubeMap->GetRhiTexture());
				shaderResourceGroup->Bind("_SkyboxSampler", skyboxSampler);
			}

			if (skyboxIrradiance)
			{
				shaderResourceGroup->Bind("_SkyboxIrradiance", skyboxIrradiance->GetRhiTexture());
			}
		}

		shaderResourceGroup->FlushBindings();

		if (needsLookupTableRebuild)
		{
			RebuildPipelineLookupTable();
		}

		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->Simulate({});
		}
	}

	void Scene::PrepareRender(f32 currentTime, u32 imageIndex)
	{
		// - Rebuild render packet -
		renderPacket.drawListMask.Reset();
		renderPacket.views.Clear();
		renderPacket.imageIndex = imageIndex;

		HashMap<View*, DrawListMask> uniqueViews{};
		for (const auto& [name, sceneViews] : viewsByTag)
		{
			for (View* view : sceneViews.views)
			{
				uniqueViews[view] |= sceneViews.drawListMask;
			}
		}

		for (const auto& [view, drawListMask] : uniqueViews)
		{
			renderPacket.drawListMask |= drawListMask;
			renderPacket.views.Add(view);

			view->Reset();
			view->Init(drawListMask);

			view->UpdateSrg(imageIndex);
		}

		shaderResourceGroup->FlushBindings();

		for (RenderPipeline* renderPipeline : renderPipelines)
		{
			View* targetView = renderPipeline->view;

			renderPipeline->GetPassTree()->IterateRecursively([&](Pass* pass)
				{
					if (!pass || pass->IsParentPass())
						return;

					if (pass->IsOfType<GpuPass>())
					{
						GpuPass* gpuPass = static_cast<GpuPass*>(pass);
						if (targetView != nullptr)
						{
							gpuPass->SetViewSrg(targetView->GetShaderResourceGroup());
							gpuPass->SetSceneSrg(shaderResourceGroup);
						}
					}
				});
		}

		// Enqueue draw packets to views

		CollectDrawPackets();
	}

	void Scene::SubmitDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{

	}

	void Scene::OnRenderEnd()
	{
		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->OnRenderEnd();
		}
	}

	void Scene::CollectDrawPackets()
	{
		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->Render(renderPacket);
		}
	}

	void Scene::GetPipelineMultiSampleState(RHI::DrawListTag drawListTag, RHI::MultisampleState& multisampleState)
	{
		if (!pipelineLookupMap.KeyExists(drawListTag))
			return;
		if (pipelineLookupMap[drawListTag].IsEmpty())
			return;

		multisampleState = pipelineLookupMap[drawListTag][0].multisampleState;
	}

	void Scene::RebuildPipelineLookupTable()
	{
		if (!needsLookupTableRebuild)
			return;

		needsLookupTableRebuild = false;

		pipelineLookupMap.Clear();

		for (auto& [name, sceneViews] : viewsByTag)
		{
			sceneViews.drawListMask.Reset();
		}

		for (RenderPipeline* renderPipeline : renderPipelines)
		{
			if (!renderPipeline)
				continue;
			
			renderPipeline->passTree->IterateRecursively([&](Pass* pass)
				{
					if (pass->IsParentPass())
						return;

					const auto& inputBindings = pass->GetInputBindings();
					const auto& inputOutputBindings = pass->GetInputOutputBindings();
					const auto& outputBindings = pass->GetOutputBindings();

					DrawListTag drawListTag = pass->GetDrawListTag();
					SceneViewTag viewTag = pass->GetViewTag();
					if (!drawListTag.IsValid())
						return;

					PipelineStateList& entry = pipelineLookupMap[drawListTag];
					PipelineStateData* pipelineStateData = nullptr;

					for (int i = 0; i < entry.GetSize(); i++)
					{
						if (entry[i].viewTag == viewTag)
						{
							pipelineStateData = &entry[i];
							break;
						}
					}

					if (pipelineStateData == nullptr)
					{
						entry.Add({});
						pipelineStateData = &entry.Top();
					}

					pipelineStateData->viewTag = viewTag;

					viewsByTag[viewTag].drawListMask.Set(drawListTag);

					auto appendEntry = [&](const PassAttachmentBinding& binding)
						{
							Ptr<PassAttachment> attachment = binding.GetOriginalAttachment();
							if (attachment == nullptr)
								return;
							if (attachment->attachmentDescriptor.type == AttachmentType::Image &&
								(binding.attachmentUsage == ScopeAttachmentUsage::Color || binding.attachmentUsage == ScopeAttachmentUsage::DepthStencil))
							{
								const RPI::ImageDescriptor& attachmentDesc = attachment->attachmentDescriptor.imageDesc;

								pipelineStateData->multisampleState.sampleCount = attachmentDesc.sampleCount;
							}
						};
					
					for (const PassAttachmentBinding& inputBinding : inputBindings)
					{
						appendEntry(inputBinding);
					}

					for (const PassAttachmentBinding& inputOutputBinding : inputOutputBindings)
					{
						appendEntry(inputOutputBinding);
					}

					for (const PassAttachmentBinding& outputBinding : outputBindings)
					{
						appendEntry(outputBinding);
					}
				});
		}
	}

	void Scene::SetSkyboxCubeMap(RPI::Texture* skyboxCubeMap, RPI::Texture* skyboxIrradiance)
	{
		this->skyboxCubeMap = skyboxCubeMap;
		this->skyboxIrradiance = skyboxIrradiance;
	}

} // namespace CE::RPI
