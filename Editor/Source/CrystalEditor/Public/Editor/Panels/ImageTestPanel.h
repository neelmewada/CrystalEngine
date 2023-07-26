#pragma once

namespace CE::Editor
{
    CLASS()
	class CRYSTALEDITOR_API ImageTestPanel : public EditorPanel
	{
		CE_CLASS(ImageTestPanel, EditorPanel)
	public:

		ImageTestPanel();
		virtual ~ImageTestPanel();

		void Construct() override;

	protected:

		void OnDrawGUI() override;

		CTexture icon{};
	};

} // namespace CE::Editor

#include "ImageTestPanel.rtti.h"
