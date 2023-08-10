#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI/RTTIDefines.h"

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

        List(const std::vector<ElementType>& vector) : Impl(vector)
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
			Impl.insert(Impl.begin() + index, item);
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
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ElementType;
            using pointer = ElementType*;  // or also value_type*
            using reference = ElementType&;  // or also value_type&

            Iterator(pointer Ptr) : Ptr(Ptr) {}

            // De-reference ops
            reference operator*() const { return *Ptr; }
            pointer operator->() { return Ptr; }

            // Increment ops
            Iterator& operator++() { Ptr++; return *this; }
            Iterator operator++(int) { Iterator Temp = *this; ++(*this); return Temp; }
            Iterator& operator+(SIZE_T rhs) { Ptr += rhs; return *this; }

            // Decrement ops
            Iterator& operator--() { Ptr--; return *this; }
            Iterator operator--(int) { Iterator Temp = *this; --(*this); return Temp; }
            Iterator& operator-(SIZE_T rhs) { Ptr -= rhs; return *this; }

            friend bool operator== (const Iterator& A, const Iterator& B) { return A.Ptr == B.Ptr; };
            friend bool operator!= (const Iterator& A, const Iterator& B) { return A.Ptr != B.Ptr; };

        private:
            pointer Ptr;
        };

        struct ConstIterator
        {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ElementType;
            using pointer = const ElementType*;  // or also value_type*
            using reference = const ElementType&;  // or also value_type&

            ConstIterator(pointer Ptr) : Ptr(Ptr) {}

            // De-reference ops
            reference operator*() const { return *Ptr; }
            pointer operator->() { return Ptr; }

            // Increment ops
            ConstIterator& operator++() { Ptr++; return *this; }
            ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }
            ConstIterator& operator+(SIZE_T rhs) { Ptr += rhs; return *this; }
            // Decrement ops
            ConstIterator& operator--() { Ptr--; return *this; }
            ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }
            ConstIterator& operator-(SIZE_T rhs) { Ptr -= rhs; return *this; }

            friend bool operator== (const ConstIterator& A, const ConstIterator& B) { return A.Ptr == B.Ptr; };
            friend bool operator!= (const ConstIterator& A, const ConstIterator& B) { return A.Ptr != B.Ptr; };

        private:
            pointer Ptr;
        };

        Iterator begin() { return Iterator{ Impl.data() }; }
        Iterator end() { return Iterator{ Impl.data() + Impl.size() }; }

        const ConstIterator begin() const { return ConstIterator{ &Impl[0] }; }
        const ConstIterator end() const { return ConstIterator{ &Impl[0] + Impl.size() }; }

        Iterator Begin() { return begin(); }
        Iterator End() { return end(); }

    protected:
        std::vector<ElementType> Impl;

        friend class Variant;
    };

    template<typename ElementType>
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

        Array(const std::vector<ElementType>& vector) : Super(vector), ElementTypeId(GetTypeId<ElementType>())
        {

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

        struct Iterator
        {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = ElementType;
            using pointer           = ElementType*;  // or also value_type*
            using reference         = ElementType&;  // or also value_type&

            Iterator(pointer Ptr) : Ptr(Ptr) {}

            // De-reference ops
            reference operator*() const { return *Ptr; }
            pointer operator->() { return Ptr; }

            // Increment ops
            Iterator& operator++() { Ptr++; return *this; }
            Iterator operator++(int) { Iterator Temp = *this; ++(*this); return Temp; }
            Iterator& operator+(SIZE_T rhs) { Ptr += rhs; return *this; }

            // Decrement ops
            Iterator& operator--() { Ptr--; return *this; }
            Iterator operator--(int) { Iterator Temp = *this; --(*this); return Temp; }
            Iterator& operator-(SIZE_T rhs) { Ptr -= rhs; return *this; }

            friend bool operator== (const Iterator& A, const Iterator& B) { return A.Ptr == B.Ptr; };
            friend bool operator!= (const Iterator& A, const Iterator& B) { return A.Ptr != B.Ptr; };

            inline operator pointer() const { return Ptr; }

        private:
            pointer Ptr;
        };

        struct ConstIterator
        {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = ElementType;
            using pointer           = const ElementType*;  // or also value_type*
            using reference         = const ElementType&;  // or also value_type&

            ConstIterator(pointer Ptr) : Ptr(Ptr) {}

            // De-reference ops
            reference operator*() const { return *Ptr; }
            pointer operator->() { return Ptr; }

            // Increment ops
            ConstIterator& operator++() { Ptr++; return *this; }
            ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }
            ConstIterator& operator+(SIZE_T rhs) { Ptr += rhs; return *this; }
            // Decrement ops
            ConstIterator& operator--() { Ptr--; return *this; }
            ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }
            ConstIterator& operator-(SIZE_T rhs) { Ptr -= rhs; return *this; }

            friend bool operator== (const ConstIterator& A, const ConstIterator& B) { return A.Ptr == B.Ptr; };
            friend bool operator!= (const ConstIterator& A, const ConstIterator& B) { return A.Ptr != B.Ptr; };

            inline operator pointer() const { return Ptr; }

        private:
            pointer Ptr;
        };

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

