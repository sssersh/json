#pragma once

#include "i_serializable.h"

namespace Serialization
{
    template<typename SerializedType, typename NameType>
    class SerializableClassImpl;

    template<typename SerializedType, typename NameType>
    class ISerializableMember : public ISerializable<NameType>
    {
    protected:
        friend class SerializableClassImpl<SerializedType, NameType>;

        constexpr ISerializableMember(SerializableClassImpl<SerializedType, NameType>* parent, const NameType& name)
            : ISerializable<NameType>(name)
            , m_parent(parent)
        {
            m_parent->registerMember(this);
        }

        /**
         * @brief Deleted, because SerializableClassImpl copy constructor is deleted
         */
        constexpr ISerializableMember(const ISerializableMember& rhs) = delete;

        constexpr ISerializableMember(ISerializableMember&& rhs)
            : ISerializable<NameType>(rhs.m_name)
            , m_parent(rhs.m_parent->m_lhs)
        {
            m_parent->registerMember(this);
        }

        /**
         * @warning Do not declare this operator as default. Default implementation will copy parent from rhs.
         */
        constexpr ISerializableMember& operator=([[maybe_unused]] const ISerializableMember& rhs) { return *this; }

        /**
         * @warning Do not declare this operator as default. Default implementation will copy parent from rhs.
         */
        constexpr ISerializableMember& operator=([[maybe_unused]] ISerializableMember&& rhs) { return *this; }

        virtual bool optional() const noexcept = 0;
        virtual bool exists() const noexcept = 0;

    protected:
        SerializableClassImpl<SerializedType, NameType>* m_parent {};
    };

} // namespace Serialization
