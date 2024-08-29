#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

#include "i_serializable_member.h"
#include "name.h"
#include "serializable_class.h"
#include "traits.h"

namespace Serialization
{
    template<typename SerializedType, typename NameType>
    class SerializableClassImpl;

    template<typename T, typename SerializedType, typename NameType, typename Parent>
    class SerializableMember : public ISerializableMember<SerializedType, NameType>
    {
    public:
        template<typename ActualParent>
        constexpr SerializableMember(ActualParent* parent, const Name<NameType>& name)
            : ISerializableMember<SerializedType, NameType>(parent, name.value())
        {
            static_assert(
                std::is_base_of_v<SerializableClass<SerializedType, NameType, typename ActualParent::UserSerializableClass>, ActualParent>,
                "Incorrect usage of CRPT: your serializable class (YourClassName) is not derived directly from SerializableClass<YourClassName> or other class, which is derived from SerializableClass");
        }

        /**
         * @brief Deleted, because SerializableClassImpl copy constructor is deleted
         */
        constexpr SerializableMember(const SerializableMember&) = delete;

        constexpr SerializableMember(SerializableMember&&) = default;

        constexpr SerializableMember& operator=(const SerializableMember&) = default;

        constexpr SerializableMember& operator=(SerializableMember&&) = default;

        constexpr operator T&() noexcept { return m_value; }

        constexpr T& value() noexcept { return m_value; }

        constexpr const T& value() const noexcept { return m_value; }

        constexpr T& operator*() noexcept { return m_value; }

        constexpr const T& operator*() const noexcept { return m_value; }

        constexpr T* operator->() noexcept { return &m_value; }

        constexpr const T* operator->() const noexcept { return &m_value; }

        constexpr T& operator=(const T& value)
        {
            m_value = value;
            return m_value;
        }

        constexpr T& operator=(T&& value)
        {
            m_value = std::move(value);
            return m_value;
        }

    private:
        constexpr bool optional() const noexcept override { return IsStdOptional<T> || IsSharedPtr<T>; }

        constexpr bool exists() const noexcept override
        {
            if constexpr (IsStdOptional<T>)
            {
                return m_value.has_value();
            }
            else if constexpr (IsSharedPtr<T>)
            {
                return m_value.get() != nullptr;
            }
            else
            {
                return true;
            }
        }

        void serialize(ISerializedType& result) const override;
        void deserialize(ISerializedType& src) override;

    private:
        T m_value {};
    };

} // namespace Serialization
