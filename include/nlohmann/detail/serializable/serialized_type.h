#pragma once

// Вот здесь и везде из nlohmann инклудить только необходимые файлы, а не всю либу целиком
#include "nlohmann/json.hpp"
#include "i_serialized_type.h"

namespace Serialization::JsonNlohmann
{
    struct SerializedType : ISerializedType
    {
        nlohmann::json json;
    };
} // namespace Serialization::JsonNlohmann
