#pragma once

#include "Object.h"
#include "Package/SavePackage.h"

namespace CE
{
	class SavePackage;

	class CORE_API Package : public Object
	{
		CE_CLASS(Package, CE::Object)
	public:
		Package();
		virtual ~Package();
        
        static SavePackageResult SavePackage(Package* outer,
                                             Object* object,
                                             const String& fileName,
                                             const SavePackageArgs& args);
        
    private:
        
	};

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Package,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(),
	CE_FUNCTION_LIST()
)
