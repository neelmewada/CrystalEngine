#pragma once

namespace CE::RPI
{
    class ModelLod;

    STRUCT()
    struct CORERPI_API ModelLodSubMesh
    {
        CE_STRUCT(ModelLodSubMesh)
    public:

        FIELD()
        u32 materialIndex = 0;

        FIELD()
        u32 numIndices = 0;

        FIELD()
        BinaryBlob indicesData{};

        FIELD()
        RHI::IndexFormat indexFormat = RHI::IndexFormat::Uint32;

    };

    CLASS()
    class CORERPI_API ModelLodAsset final : public Object
    {
        CE_CLASS(ModelLodAsset, Object)
    public:

        ModelLodAsset();
        ~ModelLodAsset();

        ModelLod* CreateModel();

        inline u32 GetNumVertices() const { return numVertices; }

    private:

        FIELD()
        u32 numVertices = 0;

        FIELD()
        Array<ModelLodSubMesh> subMeshes{};

        FIELD()
        BinaryBlob vertexData{};

        FIELD()
        BinaryBlob normalData{};

        FIELD()
        BinaryBlob tangentData{};

        FIELD()
        BinaryBlob color0Data{};

        FIELD()
        BinaryBlob uv0Data{};

        FIELD()
        BinaryBlob uv1Data{};

        FIELD()
        BinaryBlob uv2Data{};

        FIELD()
        BinaryBlob uv3Data{};

        friend class ModelLod;

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::StaticMeshAssetImporter;
#endif
    };

} // namespace CE::RPI

#include "ModelLodAsset.rtti.h"
