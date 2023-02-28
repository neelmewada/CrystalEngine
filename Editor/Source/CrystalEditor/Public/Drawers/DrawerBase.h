#pragma once

#include "CoreMinimal.h"

#include <QWidget>
#include <QLayout>

namespace CE::Editor
{
    
    class CRYSTALEDITOR_API DrawerBase : public CE::Object
    {
        CE_CLASS(DrawerBase, CE::Object)
    protected:
        DrawerBase(CE::Name name, TypeInfo* targetType);
    
    public:
        virtual ~DrawerBase();
        
    public:
        
        
    protected:
        TypeInfo* targetType = nullptr;
    };

} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, DrawerBase,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
