#pragma once

namespace CE::RHI
{
	CORERHI_API u32 GetTheadLocalContextCounter();

	template<typename Storage>
	class ThreadLocalContext
	{
	public:
		using InitFunction = Delegate<void(Storage&)>;
		using Visitor = Delegate<void(Storage&)>;
		using ConstVisitor = Delegate<void(const Storage&)>;

		static void DefaultInitializer(Storage&)
		{

		}

		ThreadLocalContext(InitFunction initFunction = &DefaultInitializer);

		ThreadLocalContext(const ThreadLocalContext&) = delete;
		ThreadLocalContext(ThreadLocalContext&&) = delete;

		Storage& GetStorage();

		void ForEach(Visitor visitor);

		// Clear all storages
		void Clear();

	private:

		static u32 MakeId();

		u32 id = 0;
		InitFunction initFunction;
		Array<ThreadId> threadIdList{};
		Array<UniquePtr<Storage>> storageList{};
		mutable SharedMutex mutex{};
	};

	template<typename Storage>
	ThreadLocalContext<Storage>::ThreadLocalContext(InitFunction initFunction)
		: id(MakeId())
		, initFunction(initFunction)
	{
	}

	template<typename Storage>
	inline u32 ThreadLocalContext<Storage>::MakeId()
	{
		return GetTheadLocalContextCounter();
	}

	template<typename Storage>
	Storage& ThreadLocalContext<Storage>::GetStorage()
	{
		// We cache the storage pointer & container ID using thread_local, which makes
		// subsequent fetches completely instant. We need to cache the containerID too,
		// because there can be multiple ThreadLocalContext<> containers in the process.
		thread_local u32 containerId = u32(-1);
		thread_local Storage* storagePtr = nullptr;

		// If the cached containerId & storagePtr is from a different ThreadLocalContext<> container
		if (id != containerId)
		{
			ThreadId curThreadId = Thread::GetCurrentThreadId();

			// Check if storage already exists
			{
				LockGuard<SharedMutex> lock{ mutex };

				SIZE_T index = 0;
				for (; index < threadIdList.GetSize(); index++)
				{
					if (threadIdList[index] == curThreadId)
					{
						break;
					}
				}

				if (index != threadIdList.GetSize())
				{
					containerId = id;
					storagePtr = storageList[index];
					return storagePtr;
				}
			}

			// Storage doesn't exist => create it
			{
				LockGuard<SharedMutex> lock{ mutex };
				threadIdList.EmplaceBack(curThreadId);
				storageList.EmplaceBack(UniquePtr<Storage>(new Storage()));

				Storage& storage = *storageList.Top();
				initFunction(storage);
				containerId = id;
				storagePtr = &storage;
				return storage;
			}
		}

		return *storagePtr;
	}

	template<typename Storage>
	void ThreadLocalContext<Storage>::ForEach(Visitor visitor)
	{
		LockGuard<SharedMutex> lock{ mutex };

		for (Storage* storage : storageList)
		{
			visitor(*storage);
		}
	}

	template<typename Storage>
	void ThreadLocalContext<Storage>::Clear()
	{
		LockGuard<SharedMutex> lock{ mutex };

		threadIdList.Clear();
		storageList.Clear();
		id = MakeId();
	}

} // namespace CE::RHI
