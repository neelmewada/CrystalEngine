#include "CoreWidgets.h"

namespace CE::Widgets
{

	void CStyleSheet::OnSubobjectAttached(Object* subObject)
	{
		if (subObject == nullptr || !subObject->GetClass()->IsSubclassOf<CStyleSheet>())
			return;

		CStyleSheet* subStylesheet = (CStyleSheet*)subObject;
		subStylesheet->SetParentStyleSheet(this);
	}

	void CStyleSheet::SetParentStyleSheet(CStyleSheet* parent)
	{
		this->parent = parent;
	}

} // namespace CE::Widgets
