
#include "Drawers/FieldDrawer.h"

namespace CE::Editor
{

    CE::HashMap<TypeId, ClassType*> FieldDrawer::fieldDrawerClassMap{};
    

    FieldDrawer::FieldDrawer(Name name) : DrawerBase(name)
    {

    }

    FieldDrawer::~FieldDrawer()
    {

    }

    void FieldDrawer::ClearGUI(QLayout* container)
    {
        QLayoutItem* item;
        while ((item = container->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }

    void FieldDrawer::SetTarget(TypeInfo* targetType, void* instance)
    {
        if (targetType == nullptr || instance == nullptr || !targetType->IsField())
            return;

        this->fieldType = (FieldType*)targetType;
        this->targetInstance = instance;
    }

    TypeInfo* FieldDrawer::GetTargetType()
    {
        return fieldType;
    }

    void* FieldDrawer::GetTargetInstance()
    {
        return targetInstance;
    }

    bool FieldDrawer::IsValid()
    {
        return fieldType != nullptr && targetInstance != nullptr;
    }

    ClassType* FieldDrawer::GetFieldDrawerClassFor(TypeId fieldTypeId)
    {
        return fieldDrawerClassMap[fieldTypeId];
    }

    void FieldDrawer::RegisterFieldDrawer(TypeId fieldTypeId, ClassType* fieldDrawerClass)
    {
        if (fieldDrawerClass == nullptr || fieldDrawerClass == Self::Type())
            return;

        if (fieldDrawerClassMap.KeyExists(fieldTypeId))
            fieldDrawerClassMap[fieldTypeId] = fieldDrawerClass;
        else
            fieldDrawerClassMap.Add({ fieldTypeId, fieldDrawerClass });
    }

    void FieldDrawer::RegisterFieldDrawer(Array<TypeId> fieldTypeIds, ClassType* fieldDrawerClass)
    {
        for (auto fieldTypeId : fieldTypeIds)
        {
            RegisterFieldDrawer(fieldTypeId, fieldDrawerClass);
        }
    }

} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, FieldDrawer)