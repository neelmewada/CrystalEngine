
#include "Drawers/DrawerBase.h"

namespace CE::Editor
{
    CE::HashMap<TypeId, ClassType*> DrawerBase::drawerClassMap{};

    DrawerBase::DrawerBase(CE::Name name)
        : Object(name)
    {
        
    }

    DrawerBase::~DrawerBase()
    {
        
    }

    void DrawerBase::RegisterDrawer(TypeId targetType, ClassType* drawerClass)
    {
        if (drawerClass == nullptr)
            return;

        if (drawerClassMap.KeyExists(targetType))
            drawerClassMap[targetType] = drawerClass;
        else
            drawerClassMap.Add({ targetType, drawerClass });
    }

    void DrawerBase::RegisterDrawer(Array<TypeId> targetTypes, ClassType* drawerClass)
    {
        if (drawerClass == nullptr)
            return;

        for (auto type : targetTypes)
        {
            RegisterDrawer(type, drawerClass);
        }
    }

    ClassType* DrawerBase::GetDrawerClassFor(TypeId targetType)
    {
        if (drawerClassMap.KeyExists(targetType))
            return drawerClassMap[targetType];

        auto targetTypeInfo = GetTypeInfo(targetType);
        if (targetTypeInfo == nullptr)
            return nullptr;

        if (!targetTypeInfo->IsStruct() && !targetTypeInfo->IsClass())
            return nullptr;

        StructType* structType = (StructType*)targetTypeInfo;

        for (int i = 0; i < structType->GetSuperTypesCount(); i++)
        {
            auto result = GetDrawerClassFor(structType->GetSuperType(i));
            if (result != nullptr)
            {
                return result;
            }
        }

        return nullptr;
    }
    
} // namespace CE::Editor

