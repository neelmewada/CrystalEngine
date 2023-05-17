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

	public:

        bool IsAsset() override;

	private:
		Name fullAssetPath;
    };
    
} // namespace CE

#include "Asset.rtti.h"
