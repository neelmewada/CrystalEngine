
#include "Types/Name.h"

#include "Containers/Array.h"

#include <cstring>

namespace CE
{

	Name::Name(String name)
	{
		Value = "";

		int i = 0;
		int length = name.GetLength();

		if (length >= 2 && name[0] == ':' && name[1] == ':') // remove "::" from prefix
			i += 2;
		if (length >= 2 && name[length - 1] == ':' && name[length - 2] == ':') // remove "::" from suffix
			length -= 2;

		Value = name.GetSubstringView(i, length);
		HashValue = GetHash(Value);

		i = 0;
		
		while (i < length)
		{
			if (i < length - 1 && Value[i] == ':' && Value[i + 1] == ':')
			{
				i += 2;
				continue;
			}
			else if (Value[i] == ':')
			{
				i++;
				continue;
			}
			else
			{
				int startIdx = i;
				int len = 0;

				while (Value[i] != ':' && i < length)
				{
					i++;
					len++;
				}

				Components.Add(Value.GetSubstringView(startIdx, len));

				i = startIdx + len;
			}

			i++;
		}
	}

	Name::Name(const char* name) : Name(String(name))
	{
		
	}

} // namespace CE

