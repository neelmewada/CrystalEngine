#pragma once


namespace CE::Editor
{

    class EDITORCORE_API ShaderCompiler
    {
    public:
        enum ErrorCode
        {
            ERR_Success = 0,
            ERR_FileNotFound,
            ERR_InvalidFile,

        };


        ShaderCompiler();
        ~ShaderCompiler();

        ErrorCode Compile(IO::Path filePath, Array<std::wstring> args);

    private:

        struct Impl;
        Impl* impl = nullptr;
        
    };
    
} // namespace CE::Editor

