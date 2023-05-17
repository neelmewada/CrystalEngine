#pragma once

#include <algorithm>

#include "RemoveReference.h"

namespace CE
{

    template<bool TValue>
    struct TBoolConst
    {
        static constexpr bool Value = TValue;
    };
	
    struct TFalseType : TBoolConst<false> {};

    struct TTrueType : TBoolConst<true> {};

    template<typename T>
    FORCE_INLINE typename RemoveReference<T>::Type&& MoveTemp(T&& value)
    {
        return static_cast<typename RemoveReference<T>::Type&&>(value);
    }

    template<typename T>
    void Swap(T& a, T& b)
    {
        T aTemp = MoveTemp(a);
        a = MoveTemp(b);
        b = MoveTemp(aTemp);
    }

	template<typename T>
	class SharedPtr
	{
	public:
		class Counter
		{
		public:
			Counter() : refCount(0)
			{}

			Counter(const Counter&) = delete;
			Counter& operator=(const Counter&) = delete;

			// Destructor
			~Counter() {}

			void Reset()
			{
				refCount = 0;
			}

			u32 Get() const { return refCount; }

			void operator++()
			{
				refCount++;
			}

			void operator++(int)
			{
				refCount++;
			}

			void operator--()
			{
				refCount--;
			}
			void operator--(int)
			{
				refCount--;
			}

		private:
			u32 refCount = 0;
		};

		explicit SharedPtr(T* ptr = nullptr) : counter(Counter())
		{
			pointer = ptr;
			counter++;
		}

		SharedPtr(SharedPtr& copy)
		{
			pointer = copy.pointer;
			counter = copy.counter;
			counter++;
		}

		SharedPtr& operator=(SharedPtr& copy)
		{
			pointer = copy.pointer;
			counter = copy.counter;
			counter++;
		}

		SharedPtr(SharedPtr&& move)
		{
			pointer = move.pointer;
			counter = move.counter;
		}

		~SharedPtr()
		{
			counter--;
			if (counter.Get() <= 0)
			{
				delete pointer;
				pointer = nullptr;
			}
		}

		u32 GetCount() const
		{
			return counter.Get();
		}

		T* Get() const
		{
			return pointer;
		}

		T& operator*()
		{
			return *pointer;
		}

		T* operator->()
		{
			return pointer;
		}

	private:
		Counter& counter;
		T* pointer = nullptr;
	};
    
} // namespace CE
