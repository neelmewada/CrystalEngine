
#include "Drawers/FieldDrawer.h"
#include "Drawers/EnumFieldDrawer.h"

#include "System.h"

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

    void FieldDrawer::SetTargets(TypeInfo* targetType, Array<void*> instances)
    {
        if (targetType == nullptr || !targetType->IsField())
            return;

        this->fieldType = (FieldType*)targetType;
        this->targetInstance = nullptr;

        if (instances.GetSize() == 0)
        {
            multipleTargetInstances.Clear();
            return;
        }

        multipleTargetInstances.AddRange(instances);
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
        return fieldType != nullptr && (targetInstance != nullptr || multipleTargetInstances.GetSize() > 0);
    }

    ClassType* FieldDrawer::GetFieldDrawerClassFor(TypeId fieldTypeId)
    {
        auto type = GetTypeInfo(fieldTypeId);
        if (type == nullptr)
        {
            type = GetStaticType<TextureDataType>();
            return nullptr;
        }
        if (type != nullptr && type->IsEnum())
        {
            return EnumFieldDrawer::Type();
        }
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

CE_RTTI_CLASS_IMPL(EDITORCORE_API, CE::Editor, FieldDrawer)