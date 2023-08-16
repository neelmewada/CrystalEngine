#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API RenderViewportPanel : public EditorPanel
	{
		CE_CLASS(RenderViewportPanel, EditorPanel)
	public:

		RenderViewportPanel();
		virtual ~RenderViewportPanel();

	protected:

		void Construct() override;

		void OnDrawGUI() override;

		RHI::CommandList* gfxCommandList = nullptr;
		RHI::RenderTarget* viewportRT = nullptr;
	};
    
} // namespace CE::Editor

#include "RenderViewportPanel.rtti.h"
