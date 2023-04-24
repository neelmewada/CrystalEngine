#pragma once

#include "CoreMinimal.h"

#include "DrawerBase.h"

#define CE_REGISTER_FIELD_DRAWER(drawerClass, targetType)\
CE::RegisterTypes<drawerClass>(); CE::Editor::FieldDrawer::RegisterFieldDrawer(TYPEID(targetType), drawerClass::Type())

#define CE_REGISTER_FIELD_DRAWERS(drawerClass, ...)\
CE::RegisterTypes<drawerClass>(); CE::Editor::FieldDrawer::RegisterFieldDrawers<__VA_ARGS__>(drawerClass::Type())

namespace CE::Editor
{

    class EDITORCORE_API FieldDrawer : public DrawerBase
    {
        CE_CLASS(FieldDrawer, DrawerBase)
    protected:
        FieldDrawer(CE::Name name);
    public:
        virtual ~FieldDrawer();

        virtual void OnValuesUpdated() = 0;

        //virtual void CreateGUI(QLayout* container);
        virtual void ClearGUI(QLayout* container);

        virtual void SetTarget(TypeInfo* targetType, void* instance) override;
        virtual void SetTargets(TypeInfo* targetType, Array<void*> instances) override;
        virtual TypeInfo* GetTargetType();
        virtual void* GetTargetInstance();

        virtual bool IsValid();

    public:
        static ClassType* GetFieldDrawerClassFor(TypeId fieldTypeId);

        static void RegisterFieldDrawer(TypeId fieldTypeId, ClassType* fieldDrawerClass);
        static void RegisterFieldDrawer(Array<TypeId> fieldTypeIds, ClassType* fieldDrawerClass);

        template<typename... Args>
        static void RegisterFieldDrawers(ClassType* fieldDrawerClass)
        {
            RegisterFieldDrawer({ TYPEID(Args)... }, fieldDrawerClass);
        }
        
    protected:
        static CE::HashMap<TypeId, ClassType*> fieldDrawerClassMap;

    protected:
        FieldType* fieldType = nullptr;
        void* targetInstance = nullptr;
        Array<void*> multipleTargetInstances{};
    };

}

CE_RTTI_CLASS(EDITORCORE_API, CE::Editor, FieldDrawer,
    CE_SUPER(CE::Editor::DrawerBase),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
