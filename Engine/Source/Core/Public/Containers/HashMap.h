#pragma once

#include "Algorithm/Hash.h"

#include <unordered_map>

#include "unordered_dense.h"

namespace CE
{
    class Name;

    template<typename KeyType, typename ValueType>
    class Pair
    {
    public:

        Pair() : first({}), second({})
        {

        }

        Pair(KeyType key, ValueType value) : first(key), second(value)
        {

        }

        KeyType first;
        ValueType second;
    };

    
    template<typename KeyType = CE::Name, typename ValueType = CE::Variant>
    class HashMap
    {
    public:
        HashMap() : Impl({})
        {
            
        }
        
        HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list) : Impl(list)
        {
            
        }

        ~HashMap()
        {

        }

        inline SIZE_T GetSize() const
        {
            return Impl.size();
        }

		inline bool IsEmpty() const
		{
			return Impl.empty();
		}

        inline SIZE_T GetCount(const KeyType& key) const
        {
            return Impl.count(key);
        }

        inline bool KeyExists(const KeyType& key) const
        {
            return Impl.contains(key);
        }

        inline ValueType& operator[](const KeyType& key)
        {
            return Impl[key];
        }
        
        inline ValueType& Get(const KeyType& key)
        {
            return Impl.at(key);
        }
        
        inline const ValueType& Get(const KeyType& key) const
        {
            return Impl.at(key);
        }

		inline void Add(const KeyType& key, const ValueType& value)
		{
			Impl.insert({ key, value });
		}

        inline void Add(const Pair<KeyType, ValueType>& pair)
        {
            Impl.insert({ pair.first, pair.second });
        }

		inline void AddRange(const HashMap<KeyType, ValueType>& from)
		{
			for (const auto& [key, value] : from)
			{
				Add({ key, value });
			}
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
        
        inline void Clear()
        {
            Impl.clear();
        }

        inline auto begin() { return Impl.begin(); }
        inline auto end() { return Impl.end(); }

        inline const auto begin() const { return Impl.begin(); }
        inline const auto end() const { return Impl.end(); }

        inline auto Begin() { return begin(); }
        inline auto End() { return end(); }

    private:
        ankerl::unordered_dense::map<KeyType, ValueType, HashFunc<KeyType>> Impl;
    };

} // namespace CE

