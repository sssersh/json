#pragma once

#include "nlohmann/json.hpp"
#include "i_serialized_type.h"

namespace Serialization::JsonNlohmann
{
    struct SerializedType : ISerializedType
    {
        nlohmann::json json;
    };
} // namespace Serialization::JsonNlohmann
