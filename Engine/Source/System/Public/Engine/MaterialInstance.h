#pragma once

namespace CE
{

    CLASS(Abstract)
    class SYSTEM_API MaterialInstance : public MaterialInterface
    {
        CE_CLASS(MaterialInstance, MaterialInterface)
    public:


    private:

        CE::Material* material = nullptr;

    };
    
} // namespace CE

#include "MaterialInstance.rtti.h"