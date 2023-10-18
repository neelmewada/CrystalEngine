#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI/RTTIDefines.h"

#include "fast_vector.h"

#include <vector>
#include <functional>

namespace CE
{
    class Variant;

    template<typename ElementType>
    using InitializerList = std::initializer_list<ElementType>;

    template<typename ElementType>
    class Array;

    template<typename ElementType>
    class List
    {
    public:
        List() : Impl()
        {

        }

        List(SIZE_T count) : Impl(count)
        {

        }

        List(SIZE_T count, const ElementType& defaultValue) : Impl(count, defaultValue)
        {

        }

        List(std::initializer_list<ElementType> list) : Impl(list)
        {

        }

		List(std::vector<ElementType> list) : Impl(list)
		{

		}

        ElementType& operator[](SIZE_T index)
        {
            return (ElementType&)Impl[index];
        }

        const ElementType& operator[](SIZE_T index) const
        {
            return Impl[index];
        }

        ElementType& GetFirst()
        {
            return Impl[0];
        }

        const ElementType& GetFirst() const
        {
            return Impl[0];
        }

        ElementType& GetLast()
        {
            return Impl[GetSize() - 1];
        }

        const ElementType& GetLast() const
        {
            return Impl[GetSize() - 1];
        }

        CE_INLINE bool IsEmpty() const
        {
            return Impl.empty();
        }

        CE_INLINE ElementType* GetData()
        {
            return Impl.data();
        }

        CE_INLINE const ElementType* GetData() const
        {
            return Impl.data();
        }

        CE_INLINE SIZE_T GetSize() const
        {
            return Impl.size();
        }

        CE_INLINE void Add(const ElementType& item)
        {
            Impl.push_back(item);
        }

		CE_INLINE void InsertAt(int index, const ElementType& item)
		{
			if (Impl.empty())
			{
				Impl.push_back(item);
				return;
			}
			Impl.insert(Impl.begin() + index, item);
		}

		CE_INLINE void InsertRange(int index, std::initializer_list<ElementType> elements)
		{
			Impl.insert(Impl.begin() + index, elements);
		}

		CE_INLINE void InsertRange(int index, const List<ElementType>& elements)
		{
			Impl.insert(Impl.begin() + index, elements.begin().ptr, elements.end().ptr);
		}

        s32 IndexOf(const ElementType& item) const
        {
            for (int i = 0; i < Impl.size(); i++)
            {
                if (Impl[i] == item)
                {
                    return i;
                }
            }
            return -1;
        }

        s32 IndexOf(std::function<bool(const ElementType&)> pred) const
        {
            for (int i = 0; i < Impl.size(); i++)
            {
                if (pred(Impl[i]))
                {
                    return i;
                }
            }
            return -1;
        }

        template<typename... Args>
        CE_INLINE void EmplaceBack(Args... args)
        {
            Impl.emplace_back(args...);
        }

        CE_INLINE void AddRange(std::initializer_list<ElementType> elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Impl.push_back(*it);
            }
        }

        CE_INLINE void AddRange(CE::List<ElementType> elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Impl.push_back(*it);
            }
        }

        void Remove(const ElementType& item)
        {
            for (int i = 0; i < Impl.size(); i++)
            {
                if (Impl[i] == item)
                {
                    Impl.erase(Impl.begin() + i);
                    return;
                }
            }
        }

        void RemoveAll(std::function<bool(const ElementType& item)> pred)
        {
            List<u32> indicesToRemove{};
            for (int i = Impl.size() - 1; i >= 0; i--)
            {
                if (pred(Impl[i]))
                {
                    indicesToRemove.Add(i);
                }
            }

            for (auto idx : indicesToRemove)
            {
                if (idx >= 0 && idx < Impl.size() - 1)
                    RemoveAt(idx);
            }
        }

        void RemoveFirst(std::function<bool(const ElementType& item)> pred)
        {
            List<u32> indicesToRemove{};
            for (int i = 0; i < Impl.size(); i++)
            {
                if (pred(Impl[i]))
                {
                    RemoveAt(i);
                    break;
                }
            }
        }

        CE_INLINE void RemoveAt(SIZE_T index)
        {
            Impl.erase(Impl.begin() + index);
        }

        CE_INLINE void Clear()
        {
            Impl.clear();
        }

        CE_INLINE void Resize(u32 newSize)
        {
            Impl.resize(newSize);
        }

        bool Exists(const ElementType& item) const
        {
            for (int i = 0; i < Impl.size(); i++)
            {
                if (Impl[i] == item)
                {
                    return true;
                }
            }
            return false;
        }

        bool Exists(std::function<bool(const ElementType&)> func) const
        {
            for (int i = 0; i < Impl.size(); i++)
            {
                if (func(Impl[i]))
                {
                    return true;
                }
            }
            return false;
        }

        /*
         *  Iterators
         */

        struct Iterator
        {
            friend class List;
            friend class Array<ElementType>;
            
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type	= std::ptrdiff_t;
            using value_type		= ElementType;
            using pointer			= ElementType*;  // or also value_type*
            using reference			= ElementType&;  // or also value_type&

            Iterator(pointer ptr) : ptr(ptr) {}

            // De-reference ops
            reference operator*() const { return *ptr; }
            pointer operator->() { return ptr; }
			operator pointer() { return ptr; }

            // Increment ops
            Iterator& operator++() { ptr++; return *this; }
            Iterator operator++(int) { Iterator Temp = *this; ++(*this); return Temp; }
			Iterator operator+(difference_type rhs) const { return Iterator(ptr + rhs); }
			Iterator operator+(Iterator rhs) const { return ptr + rhs.ptr; }

            // Decrement ops
            Iterator& operator--() { ptr--; return *this; }
            Iterator operator--(int) { Iterator Temp = *this; --(*this); return Temp; }
			Iterator operator-(difference_type rhs) const { return Iterator(ptr - rhs); }
			difference_type operator-(Iterator rhs) const { return ptr - rhs.ptr; }

            friend bool operator== (const Iterator& A, const Iterator& B) { return A.ptr == B.ptr; };
            friend bool operator!= (const Iterator& A, const Iterator& B) { return A.ptr != B.ptr; };

        private:
            pointer ptr;
        };

        struct ConstIterator
        {
            friend class List;
            friend class Array<ElementType>;
            
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ElementType;
            using pointer = const ElementType*;  // or also value_type*
            using reference = const ElementType&;  // or also value_type&

            ConstIterator(pointer ptr) : ptr(ptr) {}

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
			ConstIterator operator-(SIZE_T rhs) { return Iterator(ptr - rhs); }
			difference_type operator-(const ConstIterator& rhs) const { return (ptr - rhs.ptr); }

            friend bool operator== (const ConstIterator& A, const ConstIterator& B) { return A.ptr == B.ptr; };
            friend bool operator!= (const ConstIterator& A, const ConstIterator& B) { return A.ptr != B.ptr; };

        private:
            pointer ptr;
        };

        Iterator begin() { return Iterator{ Impl.data() }; }
        Iterator end() { return Iterator{ Impl.data() + Impl.size() }; }

        const ConstIterator begin() const { return ConstIterator{ &Impl[0] }; }
        const ConstIterator end() const { return ConstIterator{ &Impl[0] + Impl.size() }; }

        Iterator Begin() { return begin(); }
        Iterator End() { return end(); }

		template<class TPred>
		inline void Sort(TPred pred)
		{
			std::sort(Impl.begin(), Impl.end(), pred);
		}

    protected:
        std::vector<ElementType> Impl;

        friend class Variant;
    };

    template<typename ElementType = u8>
    class Array : public List<ElementType>
    {
    public:
        using Type = ElementType;
        using Super = List<ElementType>;
        
        Array() : Super(), ElementTypeId(GetTypeId<ElementType>())
        {
            
        }

        Array(SIZE_T count) : Super(count), ElementTypeId(GetTypeId<ElementType>())
        {
            
        }

        Array(SIZE_T count, const ElementType& defaultValue) : Super(count, defaultValue), ElementTypeId(GetTypeId<ElementType>())
        {

        }

        Array(std::initializer_list<ElementType> list) : Super(list), ElementTypeId(GetTypeId<ElementType>())
        {
            
        }

		Array(std::vector<ElementType> list) : Super(list), ElementTypeId(GetTypeId<ElementType>())
		{
			
		}

		template<SIZE_T Size>
		Array(const ElementType inArray[Size]) : Super(Size), ElementTypeId(GetTypeId<ElementType>())
		{
			for (int i = 0; i < Size; i++)
			{
				Super::Impl[i] = inArray[i];
			}
		}

        Array(const Array& copy)
        {
            Super::Impl = copy.Super::Impl;
            ElementTypeId = copy.ElementTypeId;
        }

        Array& operator=(const Array& copy)
        {
            Super::Impl = copy.Super::Impl;
            ElementTypeId = copy.ElementTypeId;
            return *this;
        }

        Array(Array&& move) noexcept
        {
            Super::Impl = std::move(move.Super::Impl);
            ElementTypeId = move.ElementTypeId;
        }

        CE_INLINE TypeId GetElementTypeId() const
        {
            return ElementTypeId;
        }

        ElementType& operator[](SIZE_T index)
        {
            return (ElementType&)Super::Impl[index];
        }

        const ElementType& operator[](SIZE_T index) const
        {
            return Super::Impl[index];
        }
        
        const ElementType& At(SIZE_T index) const
        {
            return Super::Impl[index];
        }
        
        ElementType& At(SIZE_T index)
        {
            return Super::Impl[index];
        }

        ElementType& GetFirst()
        {
            return Super::Impl[0];
        }

        const ElementType& GetFirst() const
        {
            return Super::Impl[0];
        }

        ElementType& GetLast()
        {
            return Super::Impl[GetSize() - 1];
        }

        const ElementType& GetLast() const
        {
            return Super::Impl[GetSize() - 1];
        }

		ElementType& Top()
		{
			return GetLast();
		}

        const ElementType& Top() const
        {
            return GetLast();
        }

        CE_INLINE bool IsEmpty() const
        {
            return Super::Impl.empty();
        }

        INLINE bool NonEmpty() const
        {
            return !IsEmpty();
        }

        CE_INLINE ElementType* GetData()
        {
            return Super::Impl.data();
        }

        CE_INLINE const ElementType* GetData() const
        {
            return Super::Impl.data();
        }

        CE_INLINE SIZE_T GetSize() const
        {
            return Super::Impl.size();
        }

        CE_INLINE void Add(const ElementType& item)
        {
			Super::Add(item);
        }

		CE_INLINE void InsertAt(SIZE_T index, const ElementType& item)
		{
			Super::InsertAt(index, item);
		}

        void Push(const ElementType& item)
        {
            Super::Impl.push_back(item);
        }

        void Pop()
        {
            Super::Impl.pop_back();
        }

        s32 IndexOf(const ElementType& item) const
        {
            for (int i = 0; i < Super::Impl.size(); i++)
            {
                if (Super::Impl[i] == item)
                {
                    return i;
                }
            }
            return -1;
        }

        s32 IndexOf(std::function<bool(const ElementType&)> pred) const
        {
            for (int i = 0; i < Super::Impl.size(); i++)
            {
                if (pred(Super::Impl[i]))
                {
                    return i;
                }
            }
            return -1;
        }

		template<typename T>
		Array<T> Transform(std::function<T(ElementType&)> selector)
		{
			Array<T> result{};
			for (int i = 0; i < Super::Impl.size(); i++)
			{
				result.Add(selector(Super::Impl[i]));
			}
			return result;
		}

        template<typename... Args>
        CE_INLINE void EmplaceBack(Args... args)
        {
            Super::Impl.emplace_back(args...);
        }

        CE_INLINE void AddRange(std::initializer_list<ElementType> elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Super::Impl.push_back(*it);
            }
        }

        CE_INLINE void AddRange(const CE::Array<ElementType>& elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Super::Impl.push_back(*it);
            }
        }

		CE_INLINE void InsertRange(int index, std::initializer_list<ElementType> elements)
		{
			Super::InsertRange(index, elements);
		}

		CE_INLINE void InsertRange(int index, const Array<ElementType>& elements)
		{
			Super::InsertRange(index, (const List<ElementType>&)elements);
		}

        bool Remove(const ElementType& item)
        {
            for (int i = 0; i < GetSize(); i++)
            {
                if (Super::Impl[i] == item)
                {
                    Super::Impl.erase(Super::Impl.begin() + i);
                    return true;
                }
            }

			return false;
        }

        void RemoveAll(std::function<bool(const ElementType& item)> pred)
        {
            Array<u32> indicesToRemove{};
            for (int i = GetSize() - 1; i >= 0; i--)
            {
                if (pred(Super::Impl[i]))
                {
                    indicesToRemove.Add(i);
                }
            }

            for (int i = 0; i < indicesToRemove.GetSize(); i++)
            {
                u32 idx = indicesToRemove[i];
                if (idx >= 0 && idx < GetSize())
                    RemoveAt(idx);
            }
        }

        void RemoveFirst(std::function<bool(const ElementType& item)> pred)
        {
            Array<u32> indicesToRemove{};
            for (int i = 0; i < Super::Impl.size(); i++)
            {
                if (pred(Super::Impl[i]))
                {
                    RemoveAt(i);
                    break;
                }
            }
        }

        CE_INLINE void RemoveAt(SIZE_T index)
        {
            Super::Impl.erase(Super::Impl.begin() + index);
        }
        
        CE_INLINE void Clear()
        {
            List<ElementType>::Impl.clear();
        }

        CE_INLINE void Resize(u32 newSize)
        {
            List<ElementType>::Impl.resize(newSize);
        }

		CE_INLINE void Resize(u32 newSize, const ElementType& defaultValue)
		{
			List<ElementType>::Impl.resize(newSize, defaultValue);
		}

        bool Exists(const ElementType& item) const
        {
            for (int i = 0; i < Super::Impl.size(); i++)
            {
                if (List<ElementType>::Impl[i] == item)
                {
                    return true;
                }
            }
            return false;
        }

        bool Exists(std::function<bool(const ElementType&)> func) const
        {
            for (int i = 0; i < Super::Impl.size(); i++)
            {
                if (func(Super::Impl[i]))
                {
                    return true;
                }
            }
            return false;
        }

        /*
         *  Iterators
         */

        /*struct Iterator
        {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = ElementType;
            using pointer           = ElementType*;  // or also value_type*
            using reference         = ElementType&;  // or also value_type&

            Iterator(pointer ptr) : ptr(ptr) {}

            // De-reference ops
            reference operator*() const { return *ptr; }
            pointer operator->() { return ptr; }

            // Increment ops
            Iterator& operator++() { ptr++; return *this; }
            Iterator operator++(int) { Iterator Temp = *this; ++(*this); return Temp; }
			Iterator operator+(difference_type rhs) { return Iterator(ptr + rhs); }

            // Decrement ops
            Iterator& operator--() { ptr--; return *this; }
            Iterator operator--(int) { Iterator Temp = *this; --(*this); return Temp; }
			Iterator operator-(difference_type rhs) { return Iterator(ptr - rhs); }

            friend bool operator== (const Iterator& A, const Iterator& B) { return A.ptr == B.ptr; };
            friend bool operator!= (const Iterator& A, const Iterator& B) { return A.ptr != B.ptr; };

            inline operator pointer() const { return ptr; }

        private:
            pointer ptr;
        };

        struct ConstIterator
        {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = ElementType;
            using pointer           = const ElementType*;  // or also value_type*
            using reference         = const ElementType&;  // or also value_type&

            ConstIterator(pointer ptr) : ptr(ptr) {}

            // De-reference ops
            reference operator*() const { return *ptr; }
            pointer operator->() { return ptr; }

            // Increment ops
            ConstIterator& operator++() { ptr++; return *this; }
            ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }
            ConstIterator& operator+(SIZE_T rhs) { ptr += rhs; return *this; }
            // Decrement ops
            ConstIterator& operator--() { ptr--; return *this; }
            ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }
            ConstIterator& operator-(SIZE_T rhs) { ptr -= rhs; return *this; }

            friend bool operator== (const ConstIterator& A, const ConstIterator& B) { return A.ptr == B.ptr; };
            friend bool operator!= (const ConstIterator& A, const ConstIterator& B) { return A.ptr != B.ptr; };

            inline operator pointer() const { return ptr; }

        private:
            pointer ptr;
        };*/

        using Iterator = typename Super::Iterator;
        using ConstIterator = typename Super::ConstIterator;

        auto begin() { return Iterator{ List<ElementType>::Impl.data() }; }
        auto end() { return Iterator{ List<ElementType>::Impl.data() + List<ElementType>::Impl.size() }; }

        const ConstIterator begin() const { return ConstIterator{ List<ElementType>::Impl.data() }; }
        const ConstIterator end() const { return ConstIterator{ List<ElementType>::Impl.data() + List<ElementType>::Impl.size() }; }

		Iterator Begin() { return begin(); }
		Iterator End() { return end(); }

    private:
        TypeId ElementTypeId;
        //std::vector<ElementType> Impl;

        friend class Variant;
    };
    
} // namespace CE

