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

		void RecreateFrameBuffer();

		RHI::Format GetImageFormat() const { return format; }

		void SetImageFormat(RHI::Format format)
		{
			this->format = format;
			RecreateFrameBuffer();
		}

		u8 GetSampleCount() const { return sampleCount; }

		void SetSampleCount(u8 samples)
		{
			if (samples == sampleCount)
				return;

			switch (samples)
			{
			case 1:
			case 2:
			case 4:
			case 8:
				sampleCount = samples;
				RecreateFrameBuffer();
				break;
			default:
				break;
			}
		}

	protected:

		void OnPaint(CPaintEvent* paintEvent) override;

		FIELD()
		RHI::Format format = Format::B8G8R8A8_UNORM;

		FIELD()
		u8 sampleCount = 1;

	private:

		Vec2i currentSize = Vec2i(0, 0);

		StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount> frames{};

	};

} // namespace CE::Widgets

#include "CViewport.rtti.h"