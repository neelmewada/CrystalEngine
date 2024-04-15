#pragma once

namespace CE::Editor
{

	CLASS()
	class CRYSTALEDITOR_API EditorToolBar : public CWidget
	{
		CE_CLASS(EditorToolBar, CWidget)
	public:

		EditorToolBar();
		virtual ~EditorToolBar();

	protected:

		void Construct() override;

	};

} // namespace CE::Editor

#include "EditorToolBar.rtti.h"