#pragma once

namespace CE
{
    
    class CORESHADER_API ShaderReflector
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

#if PLATFORM_DESKTOP

		ErrorCode ReflectSpirv(const void* byteCode, u32 byteSize, ShaderReflection& outReflection);

#endif

    private:

    };

} // namespace CE
