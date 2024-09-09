#pragma once

#include <vector>

#include "i_serializable.h"
#include "i_serializable_member.h"

template<typename T>
constexpr std::size_t check_is_constant_constructible()
{
    T obj;
    return 1;
}

template<typename T>
concept IsTrancientConstexprConstructible = requires { std::array<int, check_is_constant_constructible<T>()>(); };

//struct A
//{
//    int a = 0;
//};
//
//struct B
//{
//    constexpr B() = default;
//    constexpr ~B()
//    {
//        delete a;
//    }
//    int* a = new int();
//};
//
//static_assert(IsTrancientConstexprConstructible<A>, "");
//static_assert(IsTrancientConstexprConstructible<B>, "");

namespace Serialization
{
    template<typename ValueType, typename SerializedType, typename NameType, typename Parent>
    class SerializableMember;

    /**
     * @brief Parent serializable class
     */
    template<typename SerializedType, typename NameType>
    class SerializableClassImpl : public ISerializable<NameType>
    {
    public:
        constexpr SerializableClassImpl(std::size_t fieldsNum) { serializableMembers.reserve(fieldsNum); }

        /**
         * @brief Do now allow copy construction, because under move/copy construction member m_lhs on input argument overwrite.
         *        There may be problems under multithread copy construction from one instance.
         */
        constexpr SerializableClassImpl(const SerializableClassImpl&) = delete;

        constexpr SerializableClassImpl(SerializableClassImpl&& rhs)
            : ISerializable<NameType>(rhs.m_name)
        {
            rhs.m_lhs = this;
        }

        /**
         * @warning Do not declare this operator as default. Default implementation will copy members list from rhs.
         */
        constexpr SerializableClassImpl& operator=([[maybe_unused]] const SerializableClassImpl&) { return *this; }

        /**
         * @warning Do not declare this operator as default. Default implementation will copy members list from rhs.
         */
        constexpr SerializableClassImpl& operator=([[maybe_unused]] SerializableClassImpl&& rhs) { return *this; }

        void serialize(ISerializedType& result) const override;
        void deserialize(ISerializedType& value) override;

    private:
        friend class ISerializableMember<SerializedType, NameType>;

        constexpr void registerMember(ISerializableMember<SerializedType, NameType>* member) { serializableMembers.push_back(member); }

    protected:
        virtual std::string className() const = 0;

        template<typename Derived>
        constexpr static std::size_t getFieldsNumImpl()
        {
            Derived obj;
            return obj.serializableMembers.size();
        }

        template<typename Derived>
        static std::size_t getFieldsNum()
        {
            static const std::size_t value = getFieldsNumImpl<Derived>();
            return value;
        }

        template<typename Derived>
        constexpr static bool hasDublicates()
        {
            Derived obj;

            auto begin = obj.serializableMembers.begin();
            auto end = obj.serializableMembers.end();

            for (auto i = begin; i != end; ++i)
            {
                for (auto j = std::next(i); j != end; ++j)
                {
                    if ((*i)->name() == (*j)->name())
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        template<typename Derived>
        static bool throwIfHasDublicates()
        {
            static const auto res = hasDublicates<Derived>();
            if (res)
            {
                throw std::runtime_error( "Serializable class has dublicates in members names");
            }

            return res;
        }

    protected:
        std::vector<ISerializableMember<SerializedType, NameType>*> serializableMembers;
        SerializableClassImpl* m_lhs {}; //!< Is used only under copy/move construction of instance, which is copy/moved of this
    };
} // namespace Serialization
