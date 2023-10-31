#include "System.h"

namespace CE
{

    MeshComponent::MeshComponent()
    {
		canTick = true;
    }

    MeshComponent::~MeshComponent()
    {
		if (vertexBuffer != nullptr)
		{
			RHI::gDynamicRHI->DestroyBuffer(vertexBuffer);
			vertexBuffer = nullptr;
		}
    }

    void MeshComponent::SetMaterialAt(u32 index, MaterialInterface* material)
    {
        if (index >= materials.GetSize())
            return;
        
        materials[index] = material;
    }

} // namespace CE
