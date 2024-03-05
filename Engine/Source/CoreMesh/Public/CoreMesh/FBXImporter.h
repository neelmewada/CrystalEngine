#pragma once

namespace CE
{
    enum class FbxLoadFlags : u16
    {
		NONE = 0,
		IGNORE_GEOMETRY = 1 << 1,
		IGNORE_BLEND_SHAPES = 1 << 2,
		IGNORE_CAMERAS = 1 << 3,
		IGNORE_LIGHTS = 1 << 4,
		IGNORE_TEXTURES = 1 << 5,
		IGNORE_SKIN = 1 << 6,
		IGNORE_BONES = 1 << 7,
		IGNORE_PIVOTS = 1 << 8,
		IGNORE_ANIMATIONS = 1 << 9,
		IGNORE_MATERIALS = 1 << 10,
		IGNORE_POSES = 1 << 11,
		IGNORE_VIDEOS = 1 << 12,
		IGNORE_LIMBS = 1 << 13,
		IGNORE_MESHES = 1 << 14,
		IGNORE_MODELS = 1 << 15,

    };
    ENUM_CLASS_FLAGS(FbxLoadFlags);

	struct MeshImportConfig
	{
		FbxLoadFlags loadFlags = FbxLoadFlags::NONE;
	};

    class COREMESH_API FbxImporter final
    {
    public:

        FbxImporter();
        ~FbxImporter();

		CMScene* LoadScene(const u8* fbxData, int dataSize, const MeshImportConfig& config = {});

		inline bool Succeeded() const { return success; }
		inline const String& GetErrorMessage() const { return errorMessage; }

    private:

		String errorMessage = "";
		bool success = false;
    };
    
} // namespace CE
