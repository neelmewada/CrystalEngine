#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API Material : public MaterialInterface
	{
		CE_CLASS(Material, MaterialInterface)
	public:

		Material();

		static Material* GetErrorMaterial();

		

	protected:

	};
    
} // namespace CE

#include "Material.rtti.h"