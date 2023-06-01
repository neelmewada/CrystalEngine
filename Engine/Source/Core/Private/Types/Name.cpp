
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

		while (i < length && (name[i] == ':' || name[i] == '.')) // Remove all ':' or '.' from prefix
		{
			i++;
		}

		while (length > i && (name[length - 1] == ':' || name[length - 1] == '.')) // Remove all ':' or '.' from suffix
		{
			length--;
		}

		if (i >= length)
		{
			value = "";
			hashValue = 0;
			return;
		}

		value = name.GetSubstringView(i, length);

		String hashString = "";

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

		this->value = hashString;
		this->hashValue = GetHash(hashString);
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

	Name::Name(Name&& move)
		: value(move.value)
		, hashValue(move.hashValue)
	{
		move.value.Clear();
		move.hashValue = 0;
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

