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

    private:

        FIELD()
        RPI::ModelAsset* modelAsset = nullptr;
        
#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::StaticMeshAssetImporter;
#endif
    };
    
} // namespace CE

#include "StaticMesh.rtti.h"