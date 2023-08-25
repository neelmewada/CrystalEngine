
#include "EditorCore.h"

#include "spirv_cross/spirv_reflect.hpp"

namespace CE::Editor
{   
    
    ShaderReflector::ShaderReflector()
    {
		
    }

    ShaderReflector::~ShaderReflector()
    {

    }

	ShaderReflector::ErrorCode ShaderReflector::ReflectSpirv(const void* byteCode, u32 byteSize, ShaderReflection& outReflection)
	{


		return ERR_Success;
	}
    

} // namespace CE::Editor

