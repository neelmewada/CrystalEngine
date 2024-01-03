#pragma once

namespace CE
{

	template<typename ElementType>
	class ArrayView
	{
	public:

		struct Iterator
		{
			friend class ArrayView;
			friend class List<ElementType>;
			friend class Array<ElementType>;

			using iterator_category = std::contiguous_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = ElementType;
			using pointer = ElementType*;  // or also value_type*
			using reference = ElementType&;  // or also value_type&

			Iterator(pointer ptr = nullptr) : ptr(ptr) {}

			// De-reference ops
			reference operator*() const { return *ptr; }
			pointer operator->() { return ptr; }
			operator pointer() const { return ptr; }

			// Increment ops
			Iterator& operator++() { ptr++; return *this; }
			Iterator operator++(int) { Iterator Temp = *this; ++(*this); return Temp; }
			//Iterator operator+(difference_type rhs) const { return Iterator(ptr + rhs); }
			Iterator operator+(Iterator rhs) const { return ptr + rhs.ptr; }

			// Decrement ops
			Iterator& operator--() { ptr--; return *this; }
			Iterator operator--(int) { Iterator Temp = *this; --(*this); return Temp; }
			Iterator operator-(difference_type rhs) const { return Iterator(ptr - rhs); }
			difference_type operator-(Iterator rhs) const { return ptr - rhs.ptr; }

			//friend bool operator== (const Iterator& A, const Iterator& B) { return A.ptr == B.ptr; };
			//friend bool operator!= (const Iterator& A, const Iterator& B) { return A.ptr != B.ptr; };

		private:
			pointer ptr;
		};

		struct ConstIterator
		{
			friend class ArrayView;
			friend class List<ElementType>;
			friend class Array<ElementType>;

			using iterator_category = std::contiguous_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = ElementType;
			using pointer = const ElementType*;  // or also value_type*
			using reference = const ElementType&;  // or also value_type&

			ConstIterator(pointer ptr = nullptr) : ptr(ptr) {}

			// De-reference ops
			reference operator*() const { return *ptr; }
			pointer operator->() const { return ptr; }
			operator pointer() const { return ptr; }

			// Increment ops
			ConstIterator& operator++() { ptr++; return *this; }
			ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }
			ConstIterator operator+(difference_type rhs) const { return ConstIterator(ptr + rhs); }
			//ConstIterator operator+(SIZE_T rhs) { return ConstIterator(ptr + rhs); }
			difference_type operator+(const ConstIterator& rhs) const { return (ptr + rhs.ptr); }
			// Decrement ops
			ConstIterator& operator--() { ptr--; return *this; }
			ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }
			ConstIterator operator-(difference_type rhs) const { return ConstIterator(ptr - rhs); }
			//ConstIterator operator-(SIZE_T rhs) { return Iterator(ptr - rhs); }
			difference_type operator-(const ConstIterator& rhs) const { return (ptr - rhs.ptr); }

			//friend bool operator== (const ConstIterator& A, const ConstIterator& B) { return A.ptr == B.ptr; };
			//friend bool operator!= (const ConstIterator& A, const ConstIterator& B) { return A.ptr != B.ptr; };

		private:
			pointer ptr;
		};

		ArrayView()
		{}

		ArrayView(std::initializer_list<ElementType> list)
			: first(list.begin()), _end(list.end())
		{

		}

		ArrayView(const Array<ElementType>& array)
			: first(array.begin()), _end(array.end())
		{

		}

		ArrayView(const List<ElementType>& array)
			: first(array.begin()), _end(array.end())
		{

		}

		ArrayView(const Iterator& first, const Iterator& last)
			: first(first.ptr), _end(last.ptr + 1)
		{

		}

		ArrayView(const Iterator& first, SIZE_T count)
			: first(first.ptr), _end(first.ptr + count)
		{

		}

		ArrayView(ElementType* first, ElementType* last)
			: first(first), _end(last + 1)
		{

		}

		ArrayView(ElementType* first, SIZE_T count)
			: first(first), _end(first + count)
		{

		}

		inline const ElementType& operator[](SIZE_T index) const
		{
			return first[index];
		}

		inline bool IsValid() const
		{
			return !IsEmpty();
		}

		inline bool IsEmpty() const
		{
			return first == nullptr || _end == nullptr || ((SIZE_T)first.ptr > (SIZE_T)_end.ptr);
		}

		inline SIZE_T GetSize() const
		{
			if (IsEmpty())
				return 0;
			return ((SIZE_T)_end.ptr - (SIZE_T)first.ptr) / sizeof(ElementType);
		}

		inline ElementType* GetFirst() const
		{
			return first;
		}

		inline ElementType* GetLast() const
		{
			return _end - 1;
		}

		const ConstIterator begin() const { return first; }
		const ConstIterator end() const { return _end; }

		auto Begin() { return begin(); }
		auto End() { return end(); }

	private:

		ConstIterator first = nullptr;
		ConstIterator _end = nullptr;

	};

	template<typename ElementType>
	inline CE::List<ElementType>::List(const ArrayView<ElementType>& view)
		: Impl(view.begin(), view.end())
	{

	}

	template<typename ElementType>
	inline CE::Array<ElementType>::Array(const ArrayView<ElementType>& view) : Super(view)
	{
	}
    
} // namespace CE
