
#include "CoreMinimal.h"

#include <cstring>

namespace CE
{

	Name::Name(String name)
	{
		value = "";

		if (name.IsEmpty())
		{
			hashValue = 0;
			return;
		}

		int i = 0;
		int length = name.GetLength();

		if (length >= 2 && name[0] == ':' && name[1] == ':') // remove "::" from prefix
			i += 2;
		if (length >= 2 && name[length - 1] == ':' && name[length - 2] == ':') // remove "::" from suffix
			length -= 2;

		if (i >= length)
		{
			value = "";
			hashValue = 0;
			return;
		}

		value = name.GetSubstringView(i, length);

		String hashString = "";
		String hashStringLowercase = "";

		i = 0;
		
		while (i < length)
		{
			if (i < length - 1 && value[i] == ':' && value[i + 1] == ':')
			{
				i += 2;
				continue;
			}
			else
			{
				int startIdx = i;
				int len = 0;

				while (i < length)
				{
					i++;
					len++;

					if (i < length - 1 && value[i] == ':' && value[i + 1] == ':')
					{
						i++;
						break;
					}
				}

				if (!hashString.IsEmpty())
					hashString += "::";
				hashString += String(value.GetSubstringView(startIdx, len));
			}

			i++;
		}

		hashStringLowercase = hashString.ToLower();
		this->value = hashString;
		this->hashValue = GetHash(hashStringLowercase);
	}

	Name::Name(const char* name) : Name(String(name))
	{
		
	}

	Name::Name(const Name& copy)
	{
		this->value = copy.value;
		this->hashValue = copy.hashValue;
	}

	Name& Name::operator=(const Name& copy)
	{
		this->value = copy.value;
		this->hashValue = copy.hashValue;
		return *this;
	}

	void Name::GetComponents(CE::Array<String>& components) const
	{
		components.Clear();

		int i = 0;
		int length = value.GetLength();

		while (i < length)
		{
			if (i < length - 1 && value[i] == ':' && value[i + 1] == ':')
			{
				i += 2;
				continue;
			}
			else
			{
				int startIdx = i;
				int len = 0;

				while (i < length)
				{
					i++;
					len++;

					if (i < length - 1 && value[i] == ':' && value[i + 1] == ':')
					{
						i++;
						break;
					}
				}

				components.Add(value.GetSubstringView(startIdx, len));
			}

			i++;
		}
	}

	String Name::GetLastComponent() const
	{
		String last = "";
		int i = 0;
		int length = value.GetLength();

		while (i < length)
		{
			if (i < length - 1 && value[i] == ':' && value[i + 1] == ':')
			{
				i += 2;
				continue;
			}
			else if (value[i] == ':')
			{
				i++;
				continue;
			}
			else
			{
				int startIdx = i;
				int len = 0;

				while (value[i] != ':' && i < length)
				{
					i++;
					len++;
				}

				last = value.GetSubstringView(startIdx, len);

				i = startIdx + len;
			}

			i++;
		}

		return last;
	}

} // namespace CE

