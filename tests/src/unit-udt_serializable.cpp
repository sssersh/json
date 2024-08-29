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

namespace
{
std::string json()
{
    return R"({"bool":true,"double":2.0,"float":-1.0,"int32_t":-32,"int64_t":-64,"uint32_t":32,"uint64_t":64})";
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

        CHECK(res == Serialization::json());
    }
    SECTION("base_deserialize")
    {
        auto obj = Serialization::deserialize<Serialization::BaseTypes>(Serialization::json());

        CHECK(*obj.b == true);
        CHECK(*obj.d == 2.0);
        CHECK(*obj.f == -1.0);

        CHECK(*obj.i32 == -32);
        CHECK(*obj.i64 == -64);
        CHECK(*obj.u32 == 32);
        CHECK(*obj.u64 == 64);
    }
}
