#pragma once

#include <memory>

namespace CE
{

	template<typename T>
	class SharedPtr
	{
	public:

		SharedPtr(T* ptr = nullptr) : impl(ptr)
		{
			
		}

		inline void Reset()
		{
			impl.reset();
		}

		inline T* Get()
		{
			return impl.get();
		}

		inline operator T*()
		{
			return impl.get();
		}

		inline long Count()
		{
			return impl.use_count();
		}

		inline T* operator->()
		{
			return impl.get();
		}
		
	private:
		
		std::shared_ptr<T> impl = nullptr;
	};
    
} // namespace CE
