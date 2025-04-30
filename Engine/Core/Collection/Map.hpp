//
// Created by Echo on 2025/3/20.
//
#pragma once

#include <unordered_map>

#include "Core/Serialization/Archive.hpp"
#include "Core/TypeAlias.hpp"

template <typename TKey, typename TValue>
class Map
{
public:
    using key_type = typename std::unordered_map<TKey, TValue>::key_type;
    using mapped_type = typename std::unordered_map<TKey, TValue>::mapped_type;
    using value_type = typename std::unordered_map<TKey, TValue>::value_type;
    using iterator = typename std::unordered_map<TKey, TValue>::iterator;

    Map() = default;
    Map(const Map&) = default;
    Map(Map&&) = default;
    Map& operator=(const Map&) = default;
    Map& operator=(Map&&) = default;

    bool Add(const TKey& key, const TValue& value)
    {
        if (!mData.contains(key))
        {
            mData[key] = value;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Remove(const TKey& key)
    {
        if (mData.contains(key))
        {
            mData.erase(key);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool RemoveByValue(const TValue& value)
    {
        for (auto it = mData.begin(); it != mData.end();)
        {
            if (it->second == value)
            {
                mData.erase(it);
                return true;
            }
        }
        return false;
    }

    TValue Get(const TKey& key, const TValue& default_value = TValue())
    {
        if (mData.contains(key))
        {
            return mData[key];
        }
        else
        {
            return default_value;
        }
    }

    bool TryGet(const TKey& key, TValue& value)
    {
        if (mData.contains(key))
        {
            value = mData[key];
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Contains(const TKey& key) const
    {
        return mData.contains(key);
    }

    void Clear()
    {
        mData.clear();
    }

    UInt64 Count() const
    {
        return mData.size();
    }

    TValue& operator[](const TKey& key)
    {
        return mData[key];
    }

    TValue operator[](const TKey& key) const
    {
        return mData.at(key);
    }

    bool Empty()
    {
        return mData.empty();
    }

    auto Find(const TKey& key)
    {
        return mData.find(key);
    }

    template <typename Func>
        requires std::is_invocable_v<Func, Pair<const TKey, TValue>&>
    bool RemoveIf(Func&& func, const bool remove_all = false)
    {
        bool removed = false;
        for (auto it = mData.begin(); it != mData.end();)
        {
            if (func(*it))
            {
                it = mData.erase(it);
                removed = true;
                if (!remove_all)
                {
                    return true;
                }
            }
            else
            {
                ++it;
            }
        }
        return removed;
    }

    auto begin()
    {
        return mData.begin();
    }
    auto end()
    {
        return mData.end();
    }
    auto begin() const
    {
        return mData.begin();
    }
    auto end() const
    {
        return mData.end();
    }

    template <typename T>
    auto Erase(T&& key_or_iter)
    {
        return mData.erase(Forward<T>(key_or_iter));
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Int64 MapSize;
        Archive.ReadMapSize(MapSize);
        for (Int32 i = 0; i < Count(); ++i)
        {
            TKey Key;
            TValue Value;
            Archive.ReadKeyValue(Key, Value);
            mData[Key] = Value;
        }
    }

    void Serialization_Save(OutputArchive& Archive) const
    {
        Archive.WriteMapSize(Count());
        for (auto& item : mData)
        {
            Archive.WriteKeyValue(item.first, item.second);
        }
    }

private:
    std::unordered_map<TKey, TValue> mData;
};
