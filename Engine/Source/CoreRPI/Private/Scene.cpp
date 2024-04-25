#include "CoreRPI.h"

namespace CE::RPI
{
    
	Scene::Scene()
	{
		RPISystem::Get().scenes.Add(this);
	}

	Scene::~Scene()
	{
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

	void Scene::AddView(PipelineViewTag viewTag, ViewPtr view)
	{
		PipelineViews& pipelineViews = viewsByTag[viewTag];
		pipelineViews.viewType = PipelineViewType::Persistent;
		pipelineViews.viewTag = viewTag;
		pipelineViews.views.Add(view);
	}

	void Scene::RemoveView(PipelineViewTag viewTag, ViewPtr view)
	{
		PipelineViews& pipelineViews = viewsByTag[viewTag];
		pipelineViews.views.Remove(view);
	}

	void Scene::AddRenderPipeline(RenderPipeline* renderPipeline)
	{
		if (!renderPipeline)
			return;
		renderPipelines.Add(renderPipeline);
	}

	RHI::DrawListMask& Scene::GetDrawListMask(PipelineViewTag viewTag)
	{
		return viewsByTag[viewTag].drawListMask;
	}

	void Scene::Simulate(f32 currentTime)
	{
		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->Simulate({});
		}
	}

	void Scene::PrepareRender(f32 currentTime, u32 imageIndex)
	{
		CollectDrawPackets();
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

} // namespace CE::RPI
