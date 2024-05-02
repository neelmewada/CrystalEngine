#include "System.h"

namespace CE
{

    StaticMeshComponent::StaticMeshComponent()
    {
		canTick = true;
    }

    StaticMeshComponent::~StaticMeshComponent()
    {
	    if (meshHandle.IsValid())
	    {
            meshHandle.Free();
	    }
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

        if (meshChanged)
        {
            if (meshHandle.IsValid())
            {
                fp->ReleaseMesh(meshHandle);
            }

            auto materialMap = GetRpiMaterialMap();

            RPI::ModelHandleDescriptor descriptor{};
            descriptor.model = model;
            descriptor.originalModel = staticMesh->GetModelAsset();

            meshHandle = fp->AcquireMesh(descriptor, materialMap);
        }

        if (meshHandle.IsValid())
        {
            meshHandle->localToWorldTransform = GetTransform();
        }

        meshChanged = false;
    }

} // namespace CE
