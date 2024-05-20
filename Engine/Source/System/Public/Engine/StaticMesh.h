#pragma once

namespace CE
{

    CLASS()
    class SYSTEM_API StaticMesh : public Asset
    {
        CE_CLASS(StaticMesh, Asset)
    public:

        StaticMesh();
        ~StaticMesh();

        RPI::ModelAsset* GetModelAsset() const
        {
	        return modelAsset;
        }

        void SetModelAsset(RPI::ModelAsset* modelAsset)
        {
            this->modelAsset = modelAsset;
        }

        u32 GetLodCount() const
        {
            if (!modelAsset)
                return 0;
	        return modelAsset->GetLodCount();
        }

        u32 GetLodSubMeshCount(u32 lodIndex) const
        {
            if (!modelAsset || lodIndex >= modelAsset->GetLodCount())
                return 0;
            ModelLodAsset* lod = modelAsset->GetModelLod(lodIndex);
            if (!lod)
                return 0;
            return lod->GetSubMeshCount();
        }

    private:

        FIELD()
        RPI::ModelAsset* modelAsset = nullptr;
        
#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::StaticMeshAssetImportJob;
#endif
    };
    
} // namespace CE

#include "StaticMesh.rtti.h"