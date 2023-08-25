#pragma once

namespace CE::Editor
{
    
    class EDITORCORE_API ShaderReflector
    {
    public:
        ShaderReflector();
        ~ShaderReflector();

		enum ErrorCode
		{
			ERR_Success = 0,
			ERR_InvalidFormat,
			ERR_FileNotFound,
			ERR_InvalidFile,
			ERR_FailedToLoadFile,
			ERR_FailedToReflect,
			ERR_InvalidArgs,
		};

		ErrorCode ReflectSpirv(const void* byteCode, u32 byteSize, ShaderReflection& outReflection);

    private:

    };

} // namespace CE::Editor
