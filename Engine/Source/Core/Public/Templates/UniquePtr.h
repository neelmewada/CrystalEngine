#pragma once

#include <memory>

namespace CE
{

	template<typename T>
	class UniquePtr
	{
	public:

		UniquePtr(T* ptr = nullptr) : impl(ptr)
		{

		}

		inline void Reset(T* ptr)
		{
			impl.reset(ptr);
		}

		inline T* Release()
		{
			return impl.release();
		}

		inline T* Get()
		{
			return impl.get();
		}

		inline operator T*()
		{
			return impl.get();
		}

		inline T* operator->()
		{
			return impl.get();
		}

	private:
		std::unique_ptr<T> impl = nullptr;
	};
    
} // namespace CE
