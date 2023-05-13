#pragma once


namespace CE::Editor
{

    enum class ShaderLang
    {
        HLSL
    };

    struct ShaderBuildConfig
    {
        String debugName = "";
        String vertEntry = "vert";
        String fragEntry = "frag";
        ShaderStage stages = ShaderStage::Vertex | ShaderStage::Fragment;

        u32 maxPermutations = 1024;
        // Define flags to build multiple permutations of the shader
        Array<String> featurePermutationDefines{};
        // Flags to strip from compilation
        Array<String> unusedDefines{};
    };

    /*
    *   Low level access to DirectX Shader Compiler. Always use ShaderProcessor class instead.
    */
    class EDITORCORE_API ShaderCompiler
    {
    public:
        enum ErrorCode
        {
            ERR_Success = 0,
            ERR_FileNotFound,
            ERR_InvalidFile,
            ERR_FailedToLoadFile,
            ERR_CompilationFailure
        };

        ShaderCompiler();
        ~ShaderCompiler();

        ErrorCode Compile(const IO::Path& filePath, Array<std::wstring>& args, void** outByteCode, u32* outByteCodeSize);

        const String& GetErrorMessage() const
        {
            return errorMessage;
        }

    protected:

        String errorMessage = "";

        struct Impl;
        Impl* impl = nullptr;
        
    };
    
} // namespace CE::Editor

