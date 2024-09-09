#pragma once

#include "serializable_class_impl.h"

// TODO:
// В SerializableClass сделать параметр - аллокатор
// По умолчанию исползовать свой
// Для хранения members тоже свой использовать, чтобы количество аллокаций было детерминировано
// Аллокатор хранить в классе SerializableClass, сделать виртуальную функцию, возвращающую ссылку на него, ее вызывать из базового класса

namespace Serialization
{
    template<typename SerializedType, typename NameType, typename Derived>
    class SerializableClass : public SerializableClassImpl<SerializedType, NameType>
    {
    public:
        using Base = SerializableClassImpl<SerializedType, NameType>;
        using UserSerializableClass = Derived;

        constexpr SerializableClass()
          : Base(std::is_constant_evaluated() ? 0 : fieldsNum)
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

        // Short name for using in derived classes
        template<typename T>
        using _ = Serializable<T>;

    private:
        std::string className() const override
        {
            return typeid(Derived).name();
        }

    private:
        // Use separate static member, not direct call getFieldsNum() from constructor, because there will be recursive call of constructor
        inline static const std::size_t fieldsNum = Base::template getFieldsNum<Derived>();

    public:
        inline constexpr static std::size_t s = IsTrancientConstexprConstructible<Derived> ? Base::template getFieldsNumImpl<Derived>() : 0;

    };

} // namespace Serialization
