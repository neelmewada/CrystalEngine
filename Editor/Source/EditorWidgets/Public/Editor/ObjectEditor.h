#pragma once

namespace CE::Editor
{
	class FieldEditor;

	CLASS()
	class EDITORWIDGETS_API ObjectEditorSection : public CWidget
	{
		CE_CLASS(ObjectEditorSection, CWidget)
	public:

		ObjectEditorSection();
		virtual ~ObjectEditorSection();

		bool IsLayoutCalculationRoot() override final { return true; }

		bool Construct(ClassType* type, const String& category, const Array<Object*>& targets);

	protected:

		void OnAfterComputeStyle() override;

		void Construct() override;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnDrawGUI() override;

		FIELD()
		ClassType* targetType = nullptr;

		FIELD()
		String category = "General";

		FIELD()
		Array<Object*> targets{};

		Array<CLabel*> fieldNameLabels{};
		Array<FieldEditor*> fieldEditors{};

		GUI::ID tableId = 0;

		Vec4 headerPadding = {};
		GUI::GuiStyleState headerStyle{};

		Array<FieldType*> fields{};

		Array<float> columnWidths = Array<float>(2, 0);
		float totalHeight;

		friend class ObjectEditor;
	};

    CLASS(TargetType = "/Code/Core.CE::Object")
	class EDITORWIDGETS_API ObjectEditor : public CLayoutGroup
	{
		CE_CLASS(ObjectEditor, CLayoutGroup)
	public:

		ObjectEditor();
		virtual ~ObjectEditor();

		virtual SubClassType<Object> GetTargetClassType() { return GetStaticClass<Object>(); }

		bool IsContainer() override final { return true; }

		bool SetTargets(const Array<Object*>& targets);

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		static SubClassType<ObjectEditor> GetObjectEditorClass(const Name& targetObjectTypeName);

		static ObjectEditor* CreateEditorFor(const Name& targetObjectTypeName, CWidget* parent);

	editorwidgets_protected_internal:

		void Construct() override;

		void ConstructWidgets();

		void OnDrawGUI() override;

		FIELD()
		Array<Object*> targets{};

		FIELD()
		ClassType* targetType = nullptr;

		FIELD()
		b8 useCategories = true;

		FIELD()
		Array<String> categories{};

		int numFields = 0;

		FIELD(ReadOnly)
		Vec2 size = Vec2();

		static HashMap<Name, Array<ClassType*>> classTypeToEditorMap;
	};

} // namespace CE::Editor

#include "ObjectEditor.rtti.h"
