#include "CrystalWidgets.h"

namespace CE::Widgets
{
	Array<CStyleSheet*> CStyleSheet::styleSheets{};

	CStyleSheet::CStyleSheet()
	{
		styleSheets.Add(this);
	}

	CStyleSheet::~CStyleSheet()
	{
		styleSheets.Remove(this);
	}

	CStyleSheet* CStyleSheet::Load(const IO::Path& path, Object* parent)
	{
		FileStream fileStream = FileStream(path, Stream::Permissions::ReadOnly);

		CSSParser parser = CSSParser(&fileStream);

		Object* owner = parent;
		if (!owner)
			owner = CApplication::Get();

		return parser.ParseStyleSheet(owner);
	}

	void CStyleSheet::OnSubobjectAttached(Object* subObject)
	{
		Super::OnSubobjectAttached(subObject);

		if (subObject == nullptr || !subObject->IsOfType<CStyleSheet>())
			return;
		
		CStyleSheet* subStylesheet = (CStyleSheet*)subObject;
		subStylesheet->parent = this;
	}

	void CStyleSheet::OnSubobjectDetached(Object* subObject)
	{
		Super::OnSubobjectDetached(subObject);

		if (subObject == nullptr || !subObject->IsOfType<CStyleSheet>())
			return;

		CStyleSheet* subStylesheet = (CStyleSheet*)subObject;
		if (subStylesheet->parent == this)
		{
			subStylesheet->parent = nullptr;
		}
	}

	void CStyleSheet::MarkAllDirty()
	{
		for (auto styleSheet : styleSheets)
		{
			styleSheet->MarkDirty();
		}

		CApplication::Get()->OnStyleSheetsReloaded();
	}

} // namespace CE::Widgets
