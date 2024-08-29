#pragma once

#include <optional>

#include "nlohmann/json.hpp"
#include "serializers.h"
#include "serialized_type.h"

namespace Serialization
{
    template<typename SerializedType, NonAssociativeContainer T>
    void deserialize([[maybe_unused]] SerializedType& src,[[maybe_unused]]  T& result)
    {
//        if constexpr (requires { T().reserve({}); })
//        {
//            result.reserve(src.raw().Size());
//        }
//
//        for (auto& i : src.raw().GetArray())
//        {
//            JsonNlohmann::SerializedType serializedType { src.getAllocator(), &i };
//            typename T::value_type val;
//            deserialize<SerializedType, typename T::value_type>(serializedType, val);
//            result.push_back(val);
//        }

        // TODO (s.dobychin@vk.team): implement for other containers
    }

    template<typename SerializedType, NonAssociativeContainer T>
    void serialize([[maybe_unused]] const T& src,[[maybe_unused]] SerializedType& result)
    {
//        result.raw().SetArray();
//
//        if constexpr (requires { T().reserve({}); })
//        {
//            result.raw().Reserve(src.size(), *result.getAllocator());
//        }
//
//        for (auto& i : src)
//        {
//            SerializedType serialized { result.getAllocator() };
//            serialize<JsonNlohmann::SerializedType, typename T::value_type>(i, serialized);
//            result.raw().PushBack(serialized.raw(), *result.getAllocator());
//            // TODO (s.dobychin@vk.team): implement for other containers
//        }
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
