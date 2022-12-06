#pragma once

#include "Types/CoreTypeDefs.h"

#include "Object/Object.h"

namespace CE
{
    
    class CORE_API Component : public Object
    {
    public:
        
        virtual void Init() = 0;
        
        virtual void Activate() = 0;
        virtual void Deactivate() = 0;
        
        virtual void Tick(f32 deltaTime) = 0;
    };

} // namespace CE


