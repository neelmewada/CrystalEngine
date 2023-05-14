
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

    void ShaderReflector::Test()
    {
        spirv_cross::CompilerReflection reflection{ std::vector<u32>{} };
        auto id = reflection.get_shader_resources().uniform_buffers[0].id;
        reflection.get_decoration(id, spv::DecorationDescriptorSet);


    }

} // namespace CE::Editor

