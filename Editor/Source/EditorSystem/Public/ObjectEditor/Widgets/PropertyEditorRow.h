#pragma once

namespace CE::Editor
{
	CLASS()
	class EDITORSYSTEM_API PropertyEditorRow : public CSplitView
	{
		CE_CLASS(PropertyEditorRow, CSplitView)
	public:

		PropertyEditorRow();

		virtual ~PropertyEditorRow();

	protected:

		void Construct() override;

		FIELD()
		CSplitViewContainer* left = nullptr;

		FIELD()
		CSplitViewContainer* right = nullptr;

	};

} // namespace CE::Editor

#include "PropertyEditorRow.rtti.h"