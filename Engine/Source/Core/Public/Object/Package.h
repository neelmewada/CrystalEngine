#pragma once

#include "Object.h"

namespace CE
{

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package()
		{}
		virtual ~Package();

	};
	
	/// Transient Package: Same lifetime as Core Module.
	///	Used to store temporary objects that are not saved to disk.
	CORE_API Package* GetTransientPackage();

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Package,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(),
	CE_FUNCTION_LIST()
)
