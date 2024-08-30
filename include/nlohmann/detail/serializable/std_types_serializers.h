#pragma once

#include <optional>

#include "nlohmann/json.hpp"
#include "serializers.h"
#include "serialized_type.h"

namespace Serialization
{
    template<typename SerializedType, NonAssociativeContainer T>
    void deserialize(SerializedType& src, T& result)
    {
        if constexpr (requires { T().reserve({}); })
        {
            result.reserve(src.json.size());
        }

        std::size_t n = 0;
        for (auto& i : src.json)
        {
            JsonNlohmann::SerializedType serializedType;
            serializedType.json = i; //
            typename T::value_type val;
            deserialize<SerializedType, typename T::value_type>(serializedType, val);
            if constexpr (requires { T().push_back({}); })
            {
                result.push_back(std::move(val));
            }
            else if constexpr (IsStdArray<T>)
            {
                result[n] = std::move(val);
                n++;
            }
            // Move to trait IsSet
            else if constexpr (std::is_same_v<T, std::set<typename T::value_type, typename T::key_compare, typename T::allocator_type>>)
            {
                result.insert(std::move(val));
            }
        }

    // TODO (s.dobychin@vk.team): implement for other containers
    }

    template<typename SerializedType, NonAssociativeContainer T>
    void serialize(const T& src, SerializedType& result)
    {
        // TODO: implement
//        if constexpr (requires { T().reserve({}); })
//        {
//            result.raw().Reserve(src.size(), *result.getAllocator());
//        }

        result.json = nlohmann::json::array_t();
        for (auto& i : src)
        {
            SerializedType serialized;
            serialize<JsonNlohmann::SerializedType, typename T::value_type>(i, serialized);
            result.json.push_back(serialized.json);
            // TODO (s.dobychin@vk.team): implement for other containers
        }
    }

    template<typename SerializedType, AssociativeContainer T>
    void deserialize(SerializedType& src, T& result)
    {
        for (auto& [key, value] : src.json.items())
        {
            JsonNlohmann::SerializedType serializedType;
            serializedType.json = value; //
            typename T::mapped_type val;
            deserialize<SerializedType, typename T::mapped_type>(serializedType, val);

            result.emplace(std::move(key), std::move(val));
        }
    }

    template<typename SerializedType, AssociativeContainer T>
    void serialize(const T& src, SerializedType& result)
    {
        result.json = nlohmann::json::object_t();
        for (auto& [key, value] : src)
        {
            result.json[key] = value;
        }
    }

    template<typename SerializedType, IsStdOptional SerializableType>
    void deserialize([[maybe_unused]] SerializedType& src,[[maybe_unused]]  SerializableType& result)
    {
        if (!src.json.empty())
        {
            typename SerializableType::value_type resultValue;
            deserialize<SerializedType, typename SerializableType::value_type>(src, resultValue);
            result = resultValue;
        }
    }

    template<typename SerializedType, IsStdOptional SerializableType>
    void serialize([[maybe_unused]] const SerializableType& src,[[maybe_unused]] SerializedType& result)
    {
        if (src)
        {
            serialize<SerializedType, typename SerializableType::value_type>(*src, result);
        }
    }
} // namespace Serialization
