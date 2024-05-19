#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API PropertyDrawer : public CWidget
    {
        CE_CLASS(PropertyDrawer, CWidget)
    public:

        PropertyDrawer();

        virtual ~PropertyDrawer();

        static void RegisterPropertyDrawer(TypeId fieldDeclTypeId, SubClass<PropertyDrawer> propertyDrawer);
        static void RegisterPropertyDrawer(TypeId fieldDeclTypeId, TypeId underlyingTypeId, SubClass<PropertyDrawer> propertyDrawer);

        static void DeregisterPropertyDrawer(TypeId fieldDeclTypeId, SubClass<PropertyDrawer> propertyDrawer);
        static void DeregisterPropertyDrawer(TypeId fieldDeclTypeId, TypeId underlyingTypeId, SubClass<PropertyDrawer> propertyDrawer);

        //! @brief The targetField is used only for figuring out the type of the field.
        static PropertyDrawer* Create(FieldType* targetField, CWidget* outer, const String& name = "");

        virtual void CreateGUI(FieldType* field, void* instance);

        // - Signals -

        CE_SIGNAL(OnPropertyModified, PropertyDrawer*);

    protected:

        void Construct() override;

		FIELD()
		CWidget* left = nullptr;

		FIELD()
		CWidget* right = nullptr;

        FieldType* targetField = nullptr;

        void* targetInstance = nullptr;

    private:

        static HashMap<Pair<TypeId, TypeId>, Array<SubClass<PropertyDrawer>>> customProperyDrawers;

        friend class ObjectEditor;
    };
    
} // namespace CE::Editor

#include "PropertyDrawer.rtti.h"