#pragma once

#include "nlohmann/detail/serializable/serializable_class.h"
#include "nlohmann/detail/serializable/serializable_member.h"

namespace Serialization
{
    namespace JsonNlohmann
    {
        class SerializedType;

        using NameType = std::string_view;

        template<typename Derived>
        using Serializable = SerializableClass<SerializedType, NameType, Derived>;
    } // namespace Json

    template<>
    void SerializableClassImpl<JsonNlohmann::SerializedType, Serialization::JsonNlohmann::NameType>::serialize(ISerializedType& result) const;

    template<>
    void SerializableClassImpl<JsonNlohmann::SerializedType, Serialization::JsonNlohmann::NameType>::deserialize(ISerializedType& value);
} // namespace Serialization
