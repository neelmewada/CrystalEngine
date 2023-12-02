#pragma once

struct DxcBuffer;

namespace CE
{
	enum HlslShaderModel
	{
		SHADER_6_0 = 0,
	};

	struct ShaderBuildConfig
	{
		String debugName = "";
		String entry = "VertMain";
		ShaderStage stage = ShaderStage::Vertex;

		HlslShaderModel shaderModel = SHADER_6_0;

		u32 maxPermutations = 1024;
		Array<String> globalDefines{};

		// Define flags to build multiple permutations of the shader
		Array<String> featurePermutationDefines{};
		// Define flags to strip from compilation
		Array<String> unusedDefines{};

		Array<IO::Path> includeSearchPaths{};
	};

    /*
    *   Low level access to DirectX Shader Compiler.
    */
    class CORESHADER_API ShaderCompiler
    {
    public:
        enum ErrorCode
        {
            ERR_Success = 0,
			ERR_InvalidBuildFormat,
            ERR_FileNotFound,
            ERR_InvalidFile,
            ERR_FailedToLoadFile,
            ERR_CompilationFailure,
			ERR_InvalidArgs,
        };

        ShaderCompiler();
        ~ShaderCompiler();

		// It allocates memory to the *outByteCode location which you will have to manually release after use.
		ErrorCode BuildSpirv(const IO::Path& hlslPath, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs);

		// It allocates memory to the *outByteCode location which you will have to manually release after use.
		ErrorCode BuildSpirv(const void* data, u32 dataSize, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs);

		// It allocates memory to the *outByteCode location which you will have to manually release after use.
		inline ErrorCode Build(ShaderBlobFormat buildFormat, const void* data, u32 dataSize, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs)
		{
			if (buildFormat == ShaderBlobFormat::Spirv)
			{
				return BuildSpirv(data, dataSize, buildConfig, outByteCode, extraArgs);
			}
			return ERR_InvalidBuildFormat;
		}

		inline ErrorCode Build(ShaderBlobFormat buildFormat, const IO::Path& hlslPath, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs)
		{
			if (buildFormat == ShaderBlobFormat::Spirv)
			{
				return BuildSpirv(hlslPath, buildConfig, outByteCode, extraArgs);
			}
			return ERR_InvalidBuildFormat;
		}

        const String& GetErrorMessage() const
        {
            return errorMessage;
        }

    protected:

		ErrorCode BuildSpirv(DxcBuffer buffer, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs);

        String errorMessage = "";

        struct Impl;
        Impl* impl = nullptr;
        
    };
    
} // namespace CE

