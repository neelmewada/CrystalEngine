
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

	StringView StringView::GetSubstringView(int startIndex, int length) const
	{
		if (length < 0)
			length = Impl.size() - startIndex;
		return StringView(Impl.substr(startIndex, length));
	}

} // namespace CE
