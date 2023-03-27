
#include "System.h"

namespace CE
{

	Asset::Asset(CE::Name name) : Object(name)
	{

	}

	Asset::~Asset()
	{

	}

    CE::Name Asset::GetAssetType()
    {
        return Type()->GetName();
    }

} // namespace CE



