#pragma once

#include <deque>

namespace CE
{
    template<typename ElementType>
	class Queue
	{
	public:
		Queue() {}

		inline auto begin()
		{
			return impl.begin();
		}

		inline auto end()
		{
			return impl.end();
		}

		inline auto Begin() { return begin(); }
		inline auto End() { return end(); }

		inline bool IsEmpty() const
		{
			return impl.empty();
		}

		inline u32 GetSize() const
		{
			return impl.size();
		}

		inline const ElementType& GetFront() const
		{
			return impl.front();
		}

		inline ElementType& GetFront()
		{
			return impl.front();
		}

		inline void PopFront()
		{
			impl.pop_front();
		}

		inline void PushFront(const ElementType& value)
		{
			impl.push_front(value);
		}

		inline void PushFront(ElementType&& value)
		{
			impl.push_front(value);
		}

		inline const ElementType& GetBack() const
		{
			return impl.back();
		}

		inline ElementType& GetBack()
		{
			return impl.back();
		}

		inline void PopBack()
		{
			impl.pop_back();
		}

		inline void PushBack(const ElementType& value)
		{
			impl.push_back(value);
		}

		inline void PushBack(ElementType&& value)
		{
			impl.push_back(value);
		}

	protected:

		std::deque<ElementType> impl{};
	};

} // namespace CE
