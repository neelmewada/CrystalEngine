#pragma once

namespace CE::Editor
{
    class PropertyDrawer;

    CLASS()
    class EDITORSYSTEM_API ObjectEditor : public Object
    {
        CE_CLASS(ObjectEditor, Object)
    public:

        ObjectEditor();

        virtual ~ObjectEditor();

        static ObjectEditor* Create(Class* targetClass, Object* outer = nullptr, const String& name = "");

        static ObjectEditor* Create(Object* target, Object* outer = nullptr, const String& name = "");

        void SetTarget(Object* target);
        void SetTargets(const Array<Object*>& targets);

        virtual bool CreateGUI(CWidget* parent);

        const auto& GetTargets() const { return targets; }

        Class* GetTargetClass() const { return targetClass; }

    protected:

        FIELD(ReadOnly)
        Class* targetClass = nullptr;

        FIELD(ReadOnly)
        Array<Object*> targets{};

        FIELD(ReadOnly)
        Array<PropertyDrawer*> propertyDrawers{};

        static HashMap<ClassType*, SubClass<ObjectEditor>> customObjectEditors;
    };
    
} // namespace CE::Editor

#include "ObjectEditor.rtti.h"