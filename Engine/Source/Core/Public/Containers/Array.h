#pragma once

#include "Types/CoreTypeDefs.h"

#include "Object/RTTIDefines.h"

#include <vector>
#include <functional>

namespace CE
{

    template<typename ElementType>
    class Array
    {
    public:
        Array() : Impl(), ElementTypeId(GetTypeId<ElementType>())
        {
            
        }

        Array(SIZE_T count) : Impl(count), ElementTypeId(GetTypeId<ElementType>())
        {
            
        }

        Array(SIZE_T count, const ElementType& defaultValue) : Impl(count, defaultValue), ElementTypeId(GetTypeId<ElementType>())
        {

        }

        Array(std::initializer_list<ElementType> list) : Impl(list), ElementTypeId(GetTypeId<ElementType>())
        {
            
        }

        Array(std::vector<ElementType> vector) : Impl(vector), ElementTypeId(GetTypeId<ElementType>())
        {

        }

        CE_INLINE TypeId GetElementTypeId() const
        {
            return ElementTypeId;
        }

        ElementType& operator[](SIZE_T index)
        {
            return (ElementType&)Impl[index];
        }

        const ElementType& operator[](SIZE_T index) const
        {
            return Impl[index];
        }

        CE_INLINE SIZE_T GetSize() const
        {
            return Impl.size();
        }

        CE_INLINE void Add(const ElementType& item)
        {
            Impl.push_back(item);
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

        CE_INLINE void AddRange(CE::Array<ElementType> elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Impl.push_back(*it);
            }
        }

        CE_INLINE void Remove(const ElementType& item)
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

        CE_INLINE void RemoveAt(SIZE_T index)
        {
            Impl.erase(Impl.begin() + index);
        }
        
        CE_INLINE void Clear()
        {
            Impl.clear();
        }

        CE_INLINE bool Exists(const ElementType& item) const
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

        CE_INLINE bool Exists(std::function<bool(const ElementType&)> func) const
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

        private:
            pointer Ptr;
        };

        Iterator begin() { return Iterator{ Impl.data() }; }
        Iterator end() { return Iterator{ Impl.data() + Impl.size() }; }

        const ConstIterator begin() const { return ConstIterator{ &Impl[0]}; }
        const ConstIterator end() const { return ConstIterator{ &Impl[0] + Impl.size() }; }

        Iterator Begin() { return begin(); }
        Iterator End() { return end(); }

    private:
        TypeId ElementTypeId;
        std::vector<ElementType> Impl;
    };
    
} // namespace CE

