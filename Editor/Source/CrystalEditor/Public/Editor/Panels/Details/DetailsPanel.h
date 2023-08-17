#pragma once

namespace CE
{
    
	CLASS()
	class CRYSTALEDITOR_API DetailsPanel : public EditorPanel
	{
		CE_CLASS(DetailsPanel, EditorPanel)
	public:

		DetailsPanel();
		virtual ~DetailsPanel();

	protected:

		void Construct() override;

	};

} // namespace CE

#include "DetailsPanel.rtti.h"
