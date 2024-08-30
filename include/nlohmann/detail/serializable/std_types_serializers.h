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

        for (auto& i : src.json)
        {
            JsonNlohmann::SerializedType serializedType;
            serializedType.json = i; //
            typename T::value_type val;
            deserialize<SerializedType, typename T::value_type>(serializedType, val);
            result.push_back(val);
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

    template<typename SerializedType, IsStdOptional SerializableType>
    void deserialize([[maybe_unused]] SerializedType& src,[[maybe_unused]]  SerializableType& result)
    {
//        if (!src.raw().IsNull())
//        {
//            typename SerializableType::value_type resultValue;
//            deserialize<SerializedType, typename SerializableType::value_type>(src, resultValue);
//            result = resultValue;
//        }
    }

    template<typename SerializedType, IsStdOptional SerializableType>
    void serialize([[maybe_unused]] const SerializableType& src,[[maybe_unused]] SerializedType& result)
    {
//        if (src)
//        {
//            serialize<SerializedType, typename SerializableType::value_type>(*src, result);
//        }
    }
} // namespace Serialization
