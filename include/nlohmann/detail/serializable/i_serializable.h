#pragma once

#include <vector>

#include "i_serialized_type.h"
#include "name.h"

namespace Serialization
{
    template<typename NameType_>
    class ISerializable
    {
    public:
        using NameType = NameType_;

    protected:
        constexpr ISerializable() = default;

        constexpr virtual ~ISerializable() = default;
        virtual void serialize(ISerializedType& result) const = 0;
        virtual void deserialize(ISerializedType& value) = 0;

        constexpr ISerializable(const NameType& name)
            : m_name(name)
        {}

        /**
         * @brief Deleted, because SerializableClassImpl copy constructor is deleted
         */
        constexpr ISerializable(const ISerializable&) = delete;
        constexpr ISerializable(ISerializable&&) = default;
        constexpr ISerializable& operator=(const ISerializable&) = default;
        constexpr ISerializable& operator=(ISerializable&&) = default;

        constexpr NameType name() const { return m_name; }

        NameType m_name;
    };

} // namespace Serialization
