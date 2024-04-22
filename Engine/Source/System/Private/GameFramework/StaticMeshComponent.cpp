#include "System.h"

namespace CE
{

    StaticMeshComponent::StaticMeshComponent()
    {
		canTick = true;
    }

    StaticMeshComponent::~StaticMeshComponent()
    {
	    
    }

    u32 StaticMeshComponent::GetLodCount() const
    {
        if (staticMesh)
        {
            return staticMesh->GetLodCount();
        }
	    return Super::GetLodCount();
    }

    void StaticMeshComponent::SetStaticMesh(StaticMesh* staticMesh)
    {
        this->staticMesh = staticMesh;
        meshChanged = true;
    }

    void StaticMeshComponent::Tick(f32 delta)
    {
	    Super::Tick(delta);

        CE::Scene* scene = GetScene();
        if (!scene || !staticMesh)
            return;

        RPI::Scene* rpiScene = scene->GetRpiScene();
        StaticMeshFeatureProcessor* fp = rpiScene->GetFeatureProcessor<RPI::StaticMeshFeatureProcessor>();
        if (!fp)
            return;

        RPI::Model* model = staticMesh->GetModelAsset()->GetModel();
        if (!model)
            return;

        ModelDataInstance modelDataInstance;
        modelDataInstance.model = model;
        modelDataInstance.originalModel = staticMesh->GetModelAsset();
        modelDataInstance.worldTransform = GetTransform();

        if (meshChanged)
        {
            if (meshHandle >= 0)
            {
                fp->ReleaseMesh(meshHandle);
                meshHandle = -1;
            }

        }

        meshChanged = false;
    }

} // namespace CE
