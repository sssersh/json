#pragma once

#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include "i_serializable.h"

namespace Serialization
{
    template<typename T>
    concept IsStdString = std::is_same_v<T, std::basic_string<typename T::value_type>>;

    template<typename T>
    concept IsStdStringView = std::is_same_v<T, std::basic_string_view<typename T::value_type>>;

    template<typename T>
    concept AssociativeContainer = requires { T().begin(); T().end(); typename T::key_type; typename T::mapped_type; };

    template<typename T>
    concept NonAssociativeContainer = requires { T().begin(); T().end(); } && !(AssociativeContainer<T> || IsStdString<T> || IsStdStringView<T> || std::is_same_v<T, nlohmann::json>);

    template<typename T>
    concept IsStdOptional = std::is_same_v<T, std::optional<typename T::value_type>>;

    template<typename T>
    concept IsSerializableClass = std::is_base_of_v<ISerializable<typename T::NameType>, T>;

    // TODO: use unique ptr too
    template<typename T>
    concept IsSharedPtr = std::is_same_v<T, std::shared_ptr<typename T::element_type>>;

} // namespace Serialization
