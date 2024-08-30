#pragma once

#include "serializable_member.h"
#include "traits.h"

namespace Serialization
{
    // TODO: тут везде SerializedType заменить на nlohmann::json

    template<typename SerializedType, typename SerializableType>
    void serialize(const SerializableType& src, SerializedType& result);

    template<typename SerializedType, typename SerializableType>
    void deserialize(SerializedType& src, SerializableType& result);

    template<typename SerializedType, NonAssociativeContainer T>
    void deserialize(SerializedType& src, T& result);

    template<typename SerializedType, NonAssociativeContainer T>
    void serialize(const T& src, SerializedType& result);

    template<typename SerializedType, AssociativeContainer T>
    void deserialize(SerializedType& src, T& result);

    template<typename SerializedType, AssociativeContainer T>
    void serialize(const T& src, SerializedType& result);

    template<typename SerializedType, IsStdOptional SerializableType>
    void deserialize(SerializedType& src, SerializableType& result);

    template<typename SerializedType, IsStdOptional SerializableType>
    void serialize(const SerializableType& src, SerializedType& result);

    template<typename SerializedType, IsSharedPtr SerializableType>
    void deserialize(SerializedType& src, SerializableType& result);

    template<typename SerializedType, IsSharedPtr SerializableType>
    void serialize(const SerializableType& src, SerializedType& result);

    template<typename SerializedType, IsSerializableClass SerializableType>
    void deserialize(SerializedType& src, SerializableType& result)
    {
        result.deserialize(src);
    }

    template<typename SerializedType, IsSerializableClass SerializableType>
    void serialize(const SerializableType& src, SerializedType& result)
    {
        src.serialize(result);
    }

    template<typename SerializedType, IsSharedPtr SerializableType>
    void deserialize(SerializedType& src, SerializableType& result)
    {
        result = std::make_shared<typename SerializableType::element_type>();
        Serialization::deserialize<SerializedType, typename SerializableType::element_type>(static_cast<SerializedType&>(src), *result);
    }

    template<typename SerializedType, IsSharedPtr SerializableType>
    void serialize(const SerializableType& src, SerializedType& result)
    {
        if (src)
        {
            auto& typedResult = static_cast<SerializedType&>(result);
            Serialization::serialize<SerializedType, typename SerializableType::element_type>(*src, typedResult);
        }
    }

    template<typename SerializedType>
    void deserialize(SerializedType& src, nlohmann::json& result)
    {
        result = src.json;
    }

    template<typename SerializedType>
    void serialize(const nlohmann::json& src, SerializedType& result)
    {
        result.json = src;
    }

    template<typename T, typename SerializedType, typename NameType, typename Parent>
    void SerializableMember<T, SerializedType, NameType, Parent>::serialize(ISerializedType& result) const
    {
        auto& typedResult = static_cast<SerializedType&>(result);
        Serialization::serialize<SerializedType, T>(m_value, typedResult);
    }

    template<typename T, typename SerializedType, typename NameType, typename Parent>
    void SerializableMember<T, SerializedType, NameType, Parent>::deserialize(ISerializedType& src)
    {
        Serialization::deserialize<SerializedType, T>(static_cast<SerializedType&>(src), m_value);
    }
} // namespace Serialization
