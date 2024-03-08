#pragma once

namespace CE::RPI
{
    class Model;

    CLASS()
    class CORERPI_API ModelAsset : public Object
    {
        CE_CLASS(ModelAsset, Object)
    public:

        inline u32 GetLodCount() const { return lods.GetSize(); }

        inline ModelLodAsset* GetModelLod(u32 index) const { return lods[index]; }

        Model* CreateModel();

    private:

        FIELD()
        Array<ModelLodAsset*> lods{};

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::StaticMeshAssetImportJob;
#endif
    };

} // namespace CE

#include "ModelAsset.rtti.h"
