
#include "CoreMinimal.h"

namespace CE
{
    namespace Editor
    {
        class AssetDatabase;
    }

    enum class AssetType
    {
        None = 0,

        Texture,
        Shader,
        Mesh,
    };

    class SYSTEM_API Asset : public Object
    {
        CE_CLASS(Asset, Object)
    protected:
        Asset();

    public:
        virtual ~Asset();

        virtual AssetType GetAssetType() = 0;

    protected:
        String assetName{};
        String assetExtension{};
        IO::Path assetPath{};

        friend class CE::Editor::AssetDatabase;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, Asset,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(assetName)
    ),
    CE_FUNCTION_LIST()
)

