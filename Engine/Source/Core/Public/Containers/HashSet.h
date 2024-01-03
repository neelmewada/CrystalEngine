#pragma once

#include "Algorithm/Hash.h"

#include <unordered_set>

namespace CE
{

    template<typename T>
    class HashSet
    {
    public:
        HashSet() : impl({})
        {}
        
        HashSet(std::initializer_list<T> elements) : impl(elements)
        {}
        
        inline void Add(T item)
        {
            impl.insert(item);
        }
        
		inline void Remove(T item)
        {
            impl.erase(item);
        }
        
		inline bool Exists(T item) const
        {
            return impl.contains(item);
        }
        
        bool Exists(std::function<bool(const T&)> pred) const
        {
            for (auto it = impl.begin(); it != impl.end(); ++it)
            {
                if (pred(*it))
                {
                    return true;
                }
            }
            
            return false;
        }
        
		inline void Clear()
        {
            impl.clear();
        }

		inline SIZE_T GetSize() const { return impl.size(); }
        
        CE_INLINE auto Begin() { return impl.begin(); }
        CE_INLINE auto End() { return impl.end(); }
        
        CE_INLINE auto begin() { return Begin(); }
        CE_INLINE auto end() { return End(); }
        
    private:
        std::unordered_set<T, HashFunc<T>> impl{};
    };
    
} // namespace CE
