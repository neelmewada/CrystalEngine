#pragma once

namespace CE::Editor
{
    CLASS()
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

	protected:

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
	};

} // namespace CE::Editor

#include "ObjectEditor.rtti.h"
