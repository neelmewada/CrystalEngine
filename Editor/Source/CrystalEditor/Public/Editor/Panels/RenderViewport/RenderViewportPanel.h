#pragma once

namespace CE::Editor
{
	using namespace Widgets;

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

		RHI::GraphicsCommandList* gfxCommandList = nullptr;
		RHI::RenderTarget* viewportRT = nullptr;
		CTextureID guiTexture = nullptr;

		Vec2i currentResolution = Vec2i(512, 512);
	};
    
} // namespace CE::Editor

#include "RenderViewportPanel.rtti.h"
