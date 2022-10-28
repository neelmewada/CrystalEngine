#pragma once

#include "Algorithm/Hash.h"

#include <unordered_map>

namespace CE
{
    template<typename KeyType, typename ValueType>
    class Pair
    {
    public:

        Pair() : First({}), Second({})
        {

        }

        Pair(KeyType key, ValueType value) : First(key), Second(value)
        {

        }

        KeyType First;
        ValueType Second;
    };

    
    template<typename KeyType, typename ValueType>
    class HashMap
    {
    public:
        HashMap() : Impl({})
        {

        }

        inline SIZE_T GetSize() const
        {
            return Impl.size();
        }

        inline SIZE_T GetCount(const KeyType& key) const
        {
            return Impl.count(key);
        }

        inline bool KeyExists(const KeyType& key) const
        {
            return Impl.count(key) > 0;
        }

        inline ValueType& operator[](const KeyType& key)
        {
            return Impl[key];
        }

        inline void Add(const Pair<KeyType, ValueType>& pair)
        {
            Impl.insert({ pair.First, pair.Second });
        }

        template<typename... Args>
        inline void Emplace(Args... args)
        {
            Impl.emplace(args...);
        }

        inline void Remove(const KeyType& key)
        {
            Impl.erase(key);
        }

        inline auto begin() { return Impl.begin(); }
        inline auto end() { return Impl.end(); }

        inline auto Begin() { return begin(); }
        inline auto End() { return end(); }

    private:
        std::unordered_map<KeyType, ValueType, HashFunc<KeyType>> Impl;
    };

} // namespace CE

