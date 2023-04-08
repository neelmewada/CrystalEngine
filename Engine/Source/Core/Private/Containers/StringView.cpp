
#include "CoreTypes.h"

namespace CE
{

	StringView::StringView(const String& string) 
		: Impl(string.GetCString(), string.GetLength())
	{}

	bool StringView::operator==(const String& rhs)
	{
		return Impl == rhs.GetCString();
	}

	bool StringView::operator!=(const String& rhs)
	{
		return Impl != rhs.GetCString();
	}

	bool StringView::StartsWith(StringView str)
	{
		return Impl.starts_with(str.Impl);
	}

} // namespace CE
