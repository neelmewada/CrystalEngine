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
        String entry = "vert";
        CE::RHI::ShaderStage stage = CE::RHI::ShaderStage::Vertex;

        u32 maxPermutations = 1024;
		Array<String> globalDefines{};

        // Define flags to build multiple permutations of the shader
        Array<String> featurePermutationDefines{};
        // Define flags to strip from compilation
        Array<String> unusedDefines{};

		Array<IO::Path> includeSearchPaths{};
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
            ERR_CompilationFailure,
			ERR_InvalidArgs,
        };

        ShaderCompiler();
        ~ShaderCompiler();

		// It allocates memory to the *outByteCode location which you will have to manually release after use
		ErrorCode Build(const IO::Path& hlslPath, const ShaderBuildConfig& buildConfig, void** outByteCode, u32* outByteCodeSize, Array<std::wstring>& extraArgs);

		// Fully custom compilation
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

