#pragma once

namespace Serialization
{
    /**
     * @brief Separate class for parameters for calculating size of std::string_view
     *        (which is usually string literal) at compile time.
     *        It's not possible to use SerializableClass constructor in constexpr, because it give one paramenter os pointer.
     */
    template<typename NameType>
    class Name
    {
    public:
        constexpr Name() = default; // TODO: consteval?

        constexpr Name(NameType name) // TODO: consteval
            : val(name)
        {}

        constexpr NameType value() const { return val; }

    private:
        NameType val;
    };

} // namespace Serialization
