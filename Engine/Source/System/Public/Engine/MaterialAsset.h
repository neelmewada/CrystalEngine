#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API MaterialAsset : public Asset
	{
		CE_CLASS(MaterialAsset, Asset)
	public:

		MaterialAsset();

	protected:

		FIELD()
		MaterialInterface* material = nullptr;

	};
    
} // namespace CE

#include "MaterialAsset.rtti.h"