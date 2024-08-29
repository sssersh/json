#pragma once

#include <format>

#include "nlohmann/json.hpp"
#include "nlohmann/detail/serializable/serialized_type.h"
#include "nlohmann/detail/serializable/std_types_serializers.h"

#include "nlohmann/detail/serializable/serializable_class_defs.h"

namespace Serialization
{
    // TODO: строка может быть UTF
    template<typename ResultType>
    ResultType deserialize(const std::string& src)
    {
        JsonNlohmann::SerializedType wrapper;
        wrapper.json = nlohmann::json::parse(src);

        // TODO: check parsing errors?

        ResultType result;
        result.deserialize(wrapper);

        return result;
    }

    template<typename SrcType>
    std::string serialize(const SrcType& src)
    {
        JsonNlohmann::SerializedType wrapper;

        src.serialize(wrapper);

        return wrapper.json.dump();
    }

    template<typename SerializedType, typename SerializableType>
    void serialize(const SerializableType& src, SerializedType& result)
    {
        result.json = src;
    }

    template<typename SerializedType, typename SerializableType>
    void deserialize(SerializedType& src, SerializableType& result)
    {
        result = src.json.template get<SerializableType>();
    }

    template<>
    void SerializableClassImpl<JsonNlohmann::SerializedType, JsonNlohmann::NameType>::serialize(ISerializedType& result) const
    {
        auto& typesResult = static_cast<JsonNlohmann::SerializedType&>(result); // NOLINT cppcoreguidelines-pro-type-static-cast-downcast
        for (auto member : SerializableClassImpl::serializableMembers)
        {
            JsonNlohmann::SerializedType serializedMember;
            member->serialize(serializedMember);
            typesResult.json[member->name()] = serializedMember.json;
        }
    }

    template<>
    void SerializableClassImpl<JsonNlohmann::SerializedType, JsonNlohmann::NameType>::deserialize(ISerializedType& value)
    {
        auto& valueTyped = static_cast<JsonNlohmann::SerializedType&>(value);
        for (auto member : SerializableClassImpl::serializableMembers)
        {
            auto serializedMember = valueTyped.json.find(member->name());
            bool exists = serializedMember != valueTyped.json.end();
            if (!exists && member->optional())
            {
                continue;
            }

            if (!(exists || member->optional()))
            {
                throw std::runtime_error(std::format("Member {} not found", member->name()));
            }

            JsonNlohmann::SerializedType wrapper;
            wrapper.json = *serializedMember;
            member->deserialize(wrapper);
        }
    }

} // namespace Serialization
