#pragma once

namespace CE
{

    CLASS(Abstract)
    class SYSTEM_API Asset : public Object
    {
        CE_CLASS(Asset, CE::Object)
    protected:

        Asset();
        virtual ~Asset();

        bool IsAsset() override;

    };
    
} // namespace CE

#include "Asset.rtti.h"
