#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CRYSTALWIDGETS_API bool CFloatInputValidator(const String& text)
	{
		int periodCount = 0;

		for (int i = 0; i < text.GetLength(); ++i)
		{
			if (text[i] == '.')
			{
				if (periodCount > 0)
					return false;
				periodCount++;
			}
			else if (!String::IsNumeric(text[i]))
			{
				return false;
			}
		}
		return true;
	}

} // namespace CE::Widgets
