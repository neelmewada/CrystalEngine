#pragma once

#include "Types/CoreTypeDefs.h"

#include "Object/RTTIDefines.h"

#include <vector>
#include <functional>

namespace CE
{
    class Variant;

    template<typename ElementType>
    using InitializerList = std::initializer_list<ElementType>;

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

        Array(const std::vector<ElementType>& vector) : Impl(vector), ElementTypeId(GetTypeId<ElementType>())
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
        
        const ElementType& At(SIZE_T index) const
        {
            return Impl[index];
        }
        
        ElementType& At(SIZE_T index)
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

        CE_INLINE void AddRange(const CE::Array<ElementType>& elements)
        {
            for (auto it = elements.begin(); it != elements.end(); ++it)
            {
                Impl.push_back(*it);
            }
        }

        void Remove(const ElementType& item)
        {
            for (int i = 0; i < GetSize(); i++)
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
            Array<u32> indicesToRemove{};
            for (int i = GetSize() - 1; i >= 0; i--)
            {
                if (pred(Impl[i]))
                {
                    indicesToRemove.Add(i);
                }
            }

            for (auto idx : indicesToRemove)
            {
                if (idx >= 0 && idx < GetSize())
                    RemoveAt(idx);
            }
        }

        void RemoveFirst(std::function<bool(const ElementType& item)> pred)
        {
            Array<u32> indicesToRemove{};
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

        Iterator begin() { return Iterator{ Impl.data() }; }
        Iterator end() { return Iterator{ Impl.data() + Impl.size() }; }

        const ConstIterator begin() const { return ConstIterator{ Impl.data() }; }
        const ConstIterator end() const { return ConstIterator{ Impl.data() + Impl.size() }; }

        Iterator Begin() { return begin(); }
        Iterator End() { return end(); }

    private:
        TypeId ElementTypeId;
        std::vector<ElementType> Impl;

        friend class Variant;
    };

    template<typename ElementType>
    class Vector
    {
    public:
        Vector() : Impl()
        {

        }

        Vector(SIZE_T count) : Impl(count)
        {

        }

        Vector(SIZE_T count, const ElementType& defaultValue) : Impl(count, defaultValue)
        {

        }

        Vector(std::initializer_list<ElementType> list) : Impl(list)
        {

        }

        Vector(const std::vector<ElementType>& vector) : Impl(vector)
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

        CE_INLINE void AddRange(CE::Vector<ElementType> elements)
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
            Array<u32> indicesToRemove{};
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
            Array<u32> indicesToRemove{};
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

    private:
        std::vector<ElementType> Impl;

        friend class Variant;
    };
    
} // namespace CE

