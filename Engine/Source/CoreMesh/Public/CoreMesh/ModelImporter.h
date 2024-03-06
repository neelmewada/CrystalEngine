#pragma once

namespace CE
{
    enum class ModelPostProcessFlags
    {
        None = 0,
        CalcTangentSpace = 0x1,
        GenNormals = 0x20,
        GenSmoothNormals = 0x40,
        SplitLargeMeshes = 0x80,
        OptimizeMeshes = 0x200000,
        OptimizeGraph = 0x400000,
    };
    ENUM_CLASS_FLAGS(ModelPostProcessFlags);

    enum class ModelFileFormat
    {
        Unknown = 0,
        FBX,
        GLTF,
        OBJ
    };

    struct ModelLoadConfig
    {
        ModelPostProcessFlags postProcessFlags = ModelPostProcessFlags::CalcTangentSpace;
        ModelFileFormat fileFormat = ModelFileFormat::FBX;
    };
    
    class COREMESH_API ModelImporter
    {
    public:

        enum ErrorCode
        {
            ERR_Success = 0,
            ERR_AssImpFailed,
        };

        ModelImporter();
        ~ModelImporter();

        CMScene* ImportScene(u8* data, u64 dataSize, const ModelLoadConfig& config = {});

        inline const String& GetErrorMessage() const { return errorMessage; }

        inline bool Succeeded() const { return error == ERR_Success; }

        inline ErrorCode GetErrorCode() const { return error; }

    private:

        ErrorCode error = ERR_Success;
        String errorMessage = "";

    };

} // namespace CE
