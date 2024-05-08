#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CViewport : public CWindow
	{
		CE_CLASS(CViewport, CWindow)
	public:

		CViewport();
		virtual ~CViewport();

	protected:

		void OnPaint(CPaintEvent* paintEvent) override;

		StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount> frameBuffers{};
	};

} // namespace CE::Widgets

#include "CViewport.rtti.h"