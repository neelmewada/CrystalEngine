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
            CE::Scene* scene = GetScene();
            RPI::Scene* rpiScene = scene->GetRpiScene();
            StaticMeshFeatureProcessor* fp = rpiScene->GetFeatureProcessor<RPI::StaticMeshFeatureProcessor>();

            fp->ReleaseMesh(meshHandle);
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

    u32 StaticMeshComponent::GetLodSubMeshCount(u32 lodIndex)
    {
        if (staticMesh)
        {
            return staticMesh->GetLodSubMeshCount(lodIndex);
        }
	    return Super::GetLodSubMeshCount(lodIndex);
    }

    void StaticMeshComponent::SetStaticMesh(StaticMesh* staticMesh)
    {
        this->staticMesh = staticMesh;
        meshChanged = true;
    }

    void StaticMeshComponent::OnEnabled()
    {
	    Super::OnEnabled();

        if (meshHandle.IsValid())
        {
            meshHandle->flags.visible = true;
        }
    }

    void StaticMeshComponent::OnDisabled()
    {
        Super::OnDisabled();

        if (meshHandle.IsValid())
        {
            meshHandle->flags.visible = false;
        }
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
