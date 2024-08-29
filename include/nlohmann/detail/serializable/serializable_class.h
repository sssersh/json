#pragma once

#include "serializable_class_impl.h"

namespace Serialization
{
    template<typename SerializedType, typename NameType, typename Derived>
    class SerializableClass : public SerializableClassImpl<SerializedType, NameType>
    {
    public:
        using Base = SerializableClassImpl<SerializedType, NameType>;
        using UserSerializableClass = Derived;

        constexpr SerializableClass()
            : Base(fieldsNum)
        {}

        /**
         * @brief Deleted, because SerializableClassImpl copy constructor is deleted
         */
        constexpr SerializableClass(const SerializableClass&) = delete;

        constexpr SerializableClass(SerializableClass&&) = default;

        constexpr SerializableClass& operator=(const SerializableClass&) = default;

        constexpr SerializableClass& operator=(SerializableClass&&) = default;

        void serialize(ISerializedType& result) const override;

        void deserialize(ISerializedType& value) override;

    protected:
        // For using in derived classes
        template<typename T>
        using Serializable = SerializableMember<T, SerializedType, NameType, Derived>;

        // For using in derived classes
        template<typename T>
        using SerializableOptional = Serializable<std::optional<T>>;

        // Short name for using in derived classes
        template<typename T>
        using _ = Serializable<T>;

    private:
        // Use separate static member, not direct call getFieldsNum() from constructor, because there will be recursive call of constructor
        inline static const std::size_t fieldsNum = Base::template getFieldsNum<Derived>();
    };

} // namespace Serialization
