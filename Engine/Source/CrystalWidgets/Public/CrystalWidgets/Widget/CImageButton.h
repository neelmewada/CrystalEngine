#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CImageButton : public CWidget
	{
		CE_CLASS(CImageButton, CWidget)
	public:

		CImageButton();

		// - Button API -

		void SetAlternateStyle(bool set) { subControl = (set ? CSubControl::Alternate : CSubControl::None); }

		void SetText(const String& text);

		const String& GetText() const { return label->GetText(); }

		void SetImage(Name imagePath);

	protected:

		void Construct() override;

		FIELD()
		CImage* image = nullptr;

		FIELD()
		CLabel* label = nullptr;

	};

} // namespace CE::Widgets

#include "CImageButton.rtti.h"