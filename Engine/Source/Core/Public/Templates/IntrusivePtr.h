#pragma once

namespace CE
{

	//
	//  intrusive_ptr.hpp
	//
	//  Copyright (c) 2001, 2002 Peter Dimov
	//
	// Distributed under the Boost Software License, Version 1.0. (See
	// accompanying file LICENSE_1_0.txt or copy at
	// http://www.boost.org/LICENSE_1_0.txt)
	//
	//  See http://www.boost.org/libs/smart_ptr/intrusive_ptr.html for documentation.
	//
    
	struct IntrusiveDefaultDeleter
	{
		template<typename U>
		void operator()(U* p) const
		{
			delete p;
		}
	};

	template<typename T>
	struct IntrusivePtrCountPolicy
	{
		static inline void AddRef(T* p) { p->_AddRef(); }
		static inline void Release(T* p) { p->_Release(); }
	};

	template<typename RefCntType, typename Deleter = IntrusiveDefaultDeleter>
	class IntrusiveBaseRefCnt
	{
	public:

		inline u32 GetCount() const
		{
			return refCount;
		}

	protected:

		IntrusiveBaseRefCnt() = default;
		IntrusiveBaseRefCnt(Deleter deleter) : deleter(deleter)
		{}

		IntrusiveBaseRefCnt(const IntrusiveBaseRefCnt&) = delete;
		IntrusiveBaseRefCnt(IntrusiveBaseRefCnt&&) = delete;

		virtual ~IntrusiveBaseRefCnt()
		{
			
		}

	private:

		void _AddRef() const
		{
			++refCount;
		}

		void _Release() const
		{
			const int refCnt = static_cast<int>(--this->refCount);
			if (refCnt == 0)
			{
				deleter(this);
			}
		}

		mutable RefCntType refCount = {};
		Deleter deleter = {};

		template<typename T>
		friend struct IntrusivePtrCountPolicy;
	};

	template<class T>
	class IntrusivePtr final
	{
	private:
		template<typename U>
		friend class IntrusivePtr;

		typedef IntrusivePtr ThisType;
		typedef IntrusivePtrCountPolicy<T> CountPolicy;

	public:

		IntrusivePtr() : ptr(0)
		{

		}

		IntrusivePtr(T* p) : ptr(p)
		{
			if (ptr != 0)
			{
				CountPolicy::AddRef(ptr);
			}
		}

		template<class U> requires std::is_convertible<U*, T*>::value
		IntrusivePtr(const IntrusivePtr<U>& rhs) : ptr(rhs.Get())
		{
			if (ptr != 0)
			{
				CountPolicy::AddRef(ptr);
			}
		}

		IntrusivePtr(const IntrusivePtr& rhs) : ptr(rhs.ptr)
		{
			if (ptr != 0)
			{
				CountPolicy::AddRef(ptr);
			}
		}

		~IntrusivePtr()
		{
			if (ptr != 0)
			{
				CountPolicy::Release(ptr);
			}
		}

		inline void Reset()
		{
			ThisType().Swap(*this);
		}

		inline void Reset(T* rhs)
		{
			ThisType(rhs).Swap(*this);
		}

		inline T* Get() const
		{
			return ptr;
		}

		inline T& operator*() const
		{
			return *ptr;
		}

		inline T* operator->() const
		{
			return *ptr;
		}

		inline bool operator!() const 
		{ 
			return ptr == 0; 
		}

		inline operator bool() const
		{
			return ptr != 0;
		}

		inline void Swap(IntrusivePtr& rhs)
		{
			T* temp = ptr;
			ptr = rhs.ptr;
			rhs.ptr = temp;
		}

	private:
		T* ptr = nullptr;
	};

	using IntrusiveBase = IntrusiveBaseRefCnt<std::atomic_uint>;

} // namespace CE
