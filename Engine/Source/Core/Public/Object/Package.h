#pragma once

#include "Object.h"
#include "Package/SavePackage.h"

#define __FILE_ID__ Core_Package_h

#define GENERATED_BODY()

namespace CE
{
	class SavePackage;

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package();
		virtual ~Package();
        
        /// Saves the object itself and all other objects it references from the outer package, in a cpak file.
        static SavePackageResult SavePackage(Package* outer,
                                             Object* object,
                                             const String& fileName,
                                             const SavePackageArgs& args);
        
    private:
        bool isFullyLoaded = false;
	};

} // namespace CE

#undef __FILE_ID__

CE_RTTI_CLASS(CORE_API, CE, Package,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(
        CE_FIELD(isFullyLoaded, NonSerialized)
    ),
	CE_FUNCTION_LIST()
)
