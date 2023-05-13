#pragma once

namespace CE
{
    ENUM(Flags)
    enum class ShaderStage : u32
    {
        None = 0,
        Vertex = BIT(0),
        Fragment = BIT(1),
    };
    ENUM_CLASS_FLAGS(ShaderStage);

    CLASS()
    class SYSTEM_API Shader : public Asset
    {
        CE_CLASS(Shader, CE::Asset)
    public:

    };
    
} // namespace CE

#include "Shader.rtti.h"
