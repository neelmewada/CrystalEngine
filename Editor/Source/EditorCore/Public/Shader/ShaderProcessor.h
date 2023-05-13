#pragma once

namespace CE::Editor
{

    class EDITORCORE_API ShaderProcessor
    {
    public:
        enum ErrorCode
        {
            ERR_Success,
            ERR_VertexFail,
            ERR_FragmentFail,
        };

        ShaderProcessor();
        ~ShaderProcessor();

        void AddIncludeSearchPath(const IO::Path& includePath);
        void RemoveIncludeSearchPath(const IO::Path& includePath);
        void ClearIncludeSearchPaths();

        ErrorCode ProcessHLSL(const IO::Path& filePath, const IO::Path& outPath, const ShaderBuildConfig& buildConfig);

        const String& GetErrorMessage() const
        {
            return errorMessage;
        }

    private:
        String errorMessage = "";

        Array<IO::Path> includeSearchPaths{};
    };
    
} // namespace CE::Editor
