
#include "CoreMinimal.h"

namespace CE
{
	extern Package* gTransientPackage;

	CORE_API Package* GetTransientPackage()
	{
		return gTransientPackage;
	}

	Package::~Package()
	{

	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Package)
