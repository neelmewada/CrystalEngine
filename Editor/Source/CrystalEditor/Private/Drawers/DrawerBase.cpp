
#include "Drawers/DrawerBase.h"

namespace CE::Editor
{

    DrawerBase::DrawerBase(CE::Name name, TypeInfo* targetType)
        : Object(name), targetType(targetType)
    {
        
    }

    DrawerBase::~DrawerBase()
    {
        
    }
    
} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, DrawerBase)
