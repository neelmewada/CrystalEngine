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

	CRYSTALWIDGETS_API bool CIntegerInputValidator(const String& text)
	{
		for (int i = 0; i < text.GetLength(); ++i)
		{
			if (i == 0 && (text[0] == '-' || text[0] == '+'))
				continue;

			if (!String::IsNumeric(text[i]))
				return false;
		}
		return true;
	}

	CRYSTALWIDGETS_API bool CSignedIntegerInputValidator(const String& text)
	{
		for (int i = 0; i < text.GetLength(); ++i)
		{
			if (!String::IsNumeric(text[i]))
				return false;
		}
		return true;
	}

	CRYSTALWIDGETS_API bool CObjectNameInputValidator(const String& text)
	{
		for (int i = 0; i < text.GetLength(); ++i)
		{
			char c = text[i];
			if (!String::IsAlphabet(c) && !String::IsNumeric(c) && c != '_')
			{
				return false;
			}
		}

		return true;
	}

	CRYSTALWIDGETS_API bool CPackageNameInputValidator(const String& text)
	{
		for (int i = 0; i < text.GetLength(); ++i)
		{
			char c = text[i];

			if (!String::IsAlphabet(c) && !String::IsNumeric(c) && c != '_' && c != '/' && c != '.')
			{
				return false;
			}
		}

		return true;
	}

	CRYSTALWIDGETS_API bool CTypeNameInputValidator(const String& text)
	{
		for (int i = 0; i < text.GetLength(); ++i)
		{
			char c = text[i];

			if (c == ':' && i < text.GetLength() - 1 && text[i + 1] == ':') // Found Scope operator '::'
			{
				i++;
				continue;
			}

			if (!String::IsAlphabet(c) && !String::IsNumeric(c) && c != '_' && c != '/' && c != '.')
			{
				return false;
			}
		}

		return true;
	}

} // namespace CE::Widgets
