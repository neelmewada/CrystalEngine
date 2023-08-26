
#include "CoreShader.h"

#if PLATFORM_DESKTOP
#include "spirv_cross/spirv_reflect.hpp"
#endif

namespace CE
{   
    
    ShaderReflector::ShaderReflector()
    {
		
    }

    ShaderReflector::~ShaderReflector()
    {

    }

#if PLATFORM_DESKTOP

	ShaderReflector::ErrorCode ShaderReflector::ReflectSpirv(const void* byteCode, u32 byteSize, ShaderReflection& outReflection)
	{
		spirv_cross::CompilerReflection reflection{ (const uint32_t*)byteCode, byteSize / 4 };

		outReflection = ShaderReflection();
		
		auto resources = reflection.get_shader_resources();
		
		int numUniformBuffers = resources.uniform_buffers.size();
		for (int i = 0; i < numUniformBuffers; i++)
		{
			auto uniformBuffer = resources.uniform_buffers[i];
			
			String name = uniformBuffer.name;
			auto id = uniformBuffer.id;

		}

		return ERR_Success;
	}
#endif

} // namespace CE

