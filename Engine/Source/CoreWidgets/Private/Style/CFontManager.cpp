#include "CoreWidgets.h"

namespace CE::Widgets
{
	void CFontManager::Initialize(const Array<RHI::FontDesc>& fonts, const Array<void*>& fontHandles)
	{
		this->fonts.Clear();

		int i = 0;

		for (const auto& font : fonts)
		{
			this->fonts.Add({ font.fontName, font.pointSize, fontHandles[i] });
			i++;
		}
	}

	void CFontManager::SetFont(u32 pointSize, const Name& fontName)
	{
		for (const auto& font : fonts)
		{
			if (font.pointSize == pointSize && (!fontName.IsValid() || fontName == font.fontName))
			{
				GUI::SetCurrentFont(font.fontHandle);
				return;
			}
		}
	}

	void CFontManager::PushFont(u32 pointSize, const Name& fontName)
	{
		if (pointSize > 0)
		{
			for (const auto& font : fonts)
			{
				if (font.pointSize == pointSize && (!fontName.IsValid() || fontName == font.fontName))
				{
					GUI::PushFont(font.fontHandle);
					pushedFonts.Push(1);
					return;
				}
			}
		}

		pushedFonts.Push(0);
	}

	void CFontManager::PopFont()
	{
		if (pushedFonts.NonEmpty())
		{
			if (pushedFonts.Top() > 0)
				GUI::PopFont();
			pushedFonts.Pop();
		}
	}

	CFont CFontManager::GetCurrentFont()
	{
		for (const auto& font : fonts)
		{
			if (font.fontHandle == GUI::GetCurrentFont())
			{
				return font;
			}
		}
		return CFont();
	}

} // namespace CE::Widgets
