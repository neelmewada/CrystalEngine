#pragma once

#include "Types/CoreTypeDefs.h"

#include "Object/RTTI.h"

#include <vector>
#include <functional>

namespace CE
{

    template<typename ElementType>
    class Array
    {
    public:
        Array() : Impl()
        {
            
        }

        Array(SIZE_T count) : Impl(count)
        {
            
        }

        Array(SIZE_T count, const ElementType& defaultValue) : Impl(count, defaultValue)
        {

        }

        Array(std::initializer_list<ElementType> list) : Impl(list)
        {
            
        }

        Array(std::vector<ElementType> vector) : Impl(vector)
        {

        }

        inline static TypeId ElementTypeId()
        {
            return GetTypeId<ElementType>();
        }

        ElementType& operator[](SIZE_T index)
        {
            return (ElementType&)Impl[index];
        }

        const ElementType& operator[](SIZE_T index) const
        {
            return Impl[index];
        }

        inline SIZE_T GetSize() const
        {
            return Impl.size();
        }

        inline void Add(const ElementType& item)
        {
            Impl.push_back(item);
        }

        template<typename... Args>
        inline void EmplaceBack(Args... args)
        {
            Impl.emplace_back(args...);
        }

        inline void AddRange(std::initializer_list<const ElementType&> elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Impl.push_back(*it);
            }
        }

        inline void Remove(const ElementType& item)
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

        inline void RemoveAt(SIZE_T index)
        {
            Impl.erase(Impl.begin() + index);
        }

        inline bool Exists(const ElementType& item)
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

        inline bool Exists(std::function<bool(const ElementType&)> func)
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
        std::vector<ElementType> Impl;
    };
    
} // namespace CE

