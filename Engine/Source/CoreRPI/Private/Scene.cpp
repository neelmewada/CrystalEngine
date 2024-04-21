#include "CoreRPI.h"

namespace CE::RPI
{
    
	Scene::Scene()
	{
		RPISystem::Get().scenes.Add(this);
	}

	Scene::~Scene()
	{
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
		return fp;
	}

	void Scene::Simulate(f32 currentTime)
	{
		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->Simulate({});
		}
	}

	void Scene::PrepareRender(f32 currentTime)
	{
		CollectDrawPackets();
	}

	void Scene::CollectDrawPackets()
	{
		for (FeatureProcessor* fp : featureProcessors)
		{
			fp->Render(renderPacket);
		}
	}

} // namespace CE::RPI
