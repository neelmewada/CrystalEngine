#include "CrystalWidgets.h"

namespace CE::Widgets
{

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

} // namespace CE::Widgets
