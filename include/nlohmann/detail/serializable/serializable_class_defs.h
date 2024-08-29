#pragma once

#include "serializable_class.h"

namespace Serialization
{
    template<typename SerializedType, typename NameType, typename Derived>
    void SerializableClass<SerializedType, NameType, Derived>::serialize(ISerializedType& result) const
    {
        [[maybe_unused]] static const auto check = Base::template throwIfHasDublicates<Derived>();

        SerializableClassImpl<SerializedType, NameType>::serialize(result);
    }

    template<typename SerializedType, typename NameType, typename Derived>
    void SerializableClass<SerializedType, NameType, Derived>::deserialize(ISerializedType& value)
    {
        [[maybe_unused]] static const auto check = Base::template throwIfHasDublicates<Derived>();

        SerializableClassImpl<SerializedType, NameType>::deserialize(value);
    }
} // namespace Serialization
