#pragma once

namespace CE::Widgets
{

	struct CFont
	{
		Name fontName{};
		u32 pointSize = 0;
		void* fontHandle = nullptr;
	};

	class COREWIDGETS_API CFontManager
	{
	private:
		CFontManager() = default;

	public:

		static CFontManager& Get()
		{
			static CFontManager instance{};
			return instance;
		}

		void Initialize(const Array<RHI::FontDesc>& fonts, const Array<void*>& fontHandles);

		void SetFont(u32 pointSize, const Name& fontName = {});

		void PushFont(u32 pointSize, const Name& fontName = {});
		void PopFont();
		
	private:
		Array<int> pushedFonts{};

		Array<CFont> fonts{};
	};
    
} // namespace CE::Widgets

