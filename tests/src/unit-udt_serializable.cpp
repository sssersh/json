//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++ (supporting code)
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#include "doctest_compatibility.h"

#include <nlohmann/json.hpp>
using nlohmann::json;

// UDT - по ходу user defined types (потому что в udl - user-defined literals)

#include "nlohmann/serializable.h"
#include "nlohmann/serializer.h"

namespace Serialization
{
struct BaseTypes : JsonNlohmann::Serializable<BaseTypes>
{
    Serializable<bool> b = { this, { "bool" } };

    Serializable<float> f = { this, { "float" } };
    Serializable<double> d = { this, { "double" } };

    Serializable<int32_t> i32 = { this, { "int32_t" } };
    Serializable<int64_t> i64 = { this, { "int64_t" } };

    Serializable<uint32_t> u32 = { this, { "uint32_t" } };
    Serializable<uint64_t> u64 = { this, { "uint64_t" } };
};

struct TypeWithContainers : JsonNlohmann::Serializable<TypeWithContainers>
{
    Serializable<std::string> str = { this, { "string" } };
    Serializable<std::vector<int>> vector = { this, { "vector" } };

    // TODO (s.dobychin@vk.team): implement other containers
    // Serializable<std::array<int, 3>> array = { this, { "array_name" } };
    // Serializable<std::set<std::string>> set = { this, "set_name" };
};

namespace
{
std::string json_base_types()
{
    return R"({"bool":true,"double":2.0,"float":-1.0,"int32_t":-32,"int64_t":-64,"uint32_t":32,"uint64_t":64})";
}

std::string json_containers()
{
    return R"({"string":"str","vector":[1,2,3]})";
}

std::string emptyJson_containers()
{
    return R"({"string":"","vector":[]})";
}

} // namespace

} // namespace Serialization


TEST_CASE("Serialize/deserialize classes, defined as erializable" * doctest::test_suite("udt"))
{
    SECTION("base_serialize")
    {
        Serialization::BaseTypes obj;

        obj.b = true;
        obj.d = 2.0;
        obj.f = -1.0;
        obj.i32 = -32;
        obj.i64 = -64;
        obj.u32 = 32;
        obj.u64 = 64;

        auto res = Serialization::serialize(obj);

        CHECK(res == Serialization::json_base_types());
    }
    SECTION("base_deserialize")
    {
        auto obj = Serialization::deserialize<Serialization::BaseTypes>(Serialization::json_base_types());

        CHECK(*obj.b == true);
        CHECK(*obj.d == 2.0);
        CHECK(*obj.f == -1.0);

        CHECK(*obj.i32 == -32);
        CHECK(*obj.i64 == -64);
        CHECK(*obj.u32 == 32);
        CHECK(*obj.u64 == 64);
    }
    SECTION("deserialize_containers")
    {
        auto obj = Serialization::deserialize<Serialization::TypeWithContainers>(Serialization::json_containers());

        CHECK(*obj.str == "str");

        CHECK(obj.vector->size() == 3);
        CHECK(obj.vector->at(0) == 1);
        CHECK(obj.vector->at(1) == 2);
        CHECK(obj.vector->at(2) == 3);
    }
    SECTION("deserialize_empty_containers")
    {
        auto obj = Serialization::deserialize<Serialization::TypeWithContainers>(Serialization::emptyJson_containers());

        CHECK(*obj.str == "");
        CHECK(obj.vector->size() == 0);
    }
    SECTION("serialize_containers")
    {
        Serialization::TypeWithContainers obj;

        obj.str = "str";

        obj.vector->resize(3);
        obj.vector->at(0) = 1;
        obj.vector->at(1) = 2;
        obj.vector->at(2) = 3;

        auto res = Serialization::serialize<Serialization::TypeWithContainers>(obj);
        CHECK(res == Serialization::json_containers());
    }
    SECTION("serialize_empty_containers")
    {
        Serialization::TypeWithContainers obj;

        auto res = Serialization::serialize<Serialization::TypeWithContainers>(obj);

        CHECK(res == Serialization::emptyJson_containers());
    }
}
