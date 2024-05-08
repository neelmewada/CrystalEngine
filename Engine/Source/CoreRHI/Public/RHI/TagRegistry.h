#pragma once

namespace CE::RHI
{
	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 * Used under MIT license. https://github.com/o3de/o3de/blob/development/LICENSE_MIT.TXT
	 */

	template<typename IndexType, SIZE_T MaxTagCount>
	class TagRegistry
	{
	public:

		using TagType = RHI::Handle<IndexType>;

		/// @brief Acquires a tag from the provided tag name (Case sensitive).
		/// ReleaseTag() must be called to release the tag if this method was successful.
		TagType AcquireTag(const Name& tagName)
		{
			if (!tagName.IsValid())
			{
				return {};
			}

			TagType tag{};
			Entry* foundEmptyEntry = nullptr;

			LockGuard<SharedMutex> lock{ mutex };

			for (int i = 0; i < entriesByTag.GetSize(); i++)
			{
				Entry& entry = entriesByTag[i];

				if (entry.refCount == 0 && !foundEmptyEntry)
				{
					foundEmptyEntry = &entry;
					tag = TagType(i);
				}
				else if (entry.name == tagName)
				{
					entry.refCount++;
					return TagType(i);
				}
			}

			if (foundEmptyEntry)
			{
				foundEmptyEntry->name = tagName;
				foundEmptyEntry->refCount = 1;
				allocatedTagCount++;
			}

			return tag;
		}

		void ReleaseTag(TagType tag)
		{
			if (tag.IsValid())
			{
				LockGuard<SharedMutex> lock{ mutex };
				Entry& entry = entriesByTag[tag.Get()];
				const SIZE_T refCount = --entry.refCount;
				if (refCount == 0)
				{
					entry.name = Name();
					--allocatedTagCount;
				}
			}
		}

		TagType FindTag(const Name& tagName) const
		{
			LockGuard<SharedMutex> lock{ mutex };
			for (int i = 0; i < entriesByTag.GetSize(); i++)
			{
				if (entriesByTag[i].name == tagName)
				{
					return TagType(i);
				}
			}
			return {};
		}

		TagType FindOrAcquireTag(const Name& tagName) const
		{
			{
				LockGuard<SharedMutex> lock{ mutex };
				for (int i = 0; i < entriesByTag.GetSize(); i++)
				{
					if (entriesByTag[i].name == tagName)
					{
						return TagType(i);
					}
				}
			}

			return AcquireTag(tagName);
		}

		Name GetName(TagType tag) const
		{
			if (tag.Get() < entriesByTag.GetSize())
			{
				return entriesByTag[tag].name;
			}
			return {};
		}

		SIZE_T GetAllocatedTagCount() const
		{
			return allocatedTagCount;
		}

		template<class TagVisitor>
		void VisitTags(TagVisitor visitor)
		{
			SIZE_T validEntriesLeft = allocatedTagCount;

			for (int i = 0; i < entriesByTag.GetSize() && validEntriesLeft > 0; i++)
			{
				const Entry& entry = entriesByTag[i];
				if (entry.refCount > 0)
				{
					visitor(entry.name, entry.refCount);
					validEntriesLeft--;
				}
			}
		}
		
	private:

		struct Entry
		{
			Name name{};
			int refCount = 0;
		};

		mutable SharedMutex mutex{};
		StaticArray<Entry, MaxTagCount> entriesByTag{};
		SIZE_T allocatedTagCount = 0;

	};

} // namespace CE::RHI
