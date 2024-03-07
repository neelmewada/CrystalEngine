#include "EditorCore.h"

namespace CE::Editor
{

    const Array<String>& StaticMeshAssetDefinition::GetSourceExtensions()
    {
        static const Array<String> extensions{
            ".fbx"
        };

        return extensions;
    }

    SubClass<AssetImporter> StaticMeshAssetDefinition::GetAssetImporterClass()
    {
        return GetStaticClass<StaticMeshAssetImporter>();
    }

} // namespace CE::Editor
